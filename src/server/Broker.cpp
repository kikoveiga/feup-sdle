#include "Broker.h"

#include <iostream>
#include "zhelpers.hpp"

#define HEARTBEAT_LIVENESS 3
#define HEARTBEAT_INTERVAL 1000

Broker::Broker() : context(1), frontend(context, ZMQ_ROUTER), backend(context, ZMQ_ROUTER) {
    frontend.bind("tcp://*:5555");
    backend.bind("tcp://*:5556");
    heartbeat_at = s_clock() + HEARTBEAT_INTERVAL;
}

void Broker::worker_append(const string& identity) {
    for (auto&[w_identity, expiry] : workers) {
        if (w_identity == identity) {
            cerr << "E: duplicate worker identity (" << identity << ")" << endl;
            exit(1);
        }
    }

    Worker w;
    w.identity = identity;
    w.expiry = s_clock() + HEARTBEAT_LIVENESS * HEARTBEAT_INTERVAL;
    workers.push_back(w);

    cout << "I: worker ready: " << identity << endl;
}

void Broker::worker_delete(const string &identity) {
    for (auto it = workers.begin(); it < workers.end(); ++it) {
        if (it->identity == identity) {
            workers.erase(it);
            break;
        }
    }
}

void Broker::worker_refresh(const string& identity) {
    bool found = false;
    for (auto&[w_identity, expiry] : workers) {
        if (w_identity == identity) {
            expiry = s_clock() + HEARTBEAT_LIVENESS * HEARTBEAT_INTERVAL;
            found = true;
            break;
        }
    }

    if (!found) {
        cerr << "E: worker " << identity << " not ready" << endl;
    }
}

string Broker::worker_dequeue() {
    assert(!workers.empty());
    string identity = workers.front().identity;
    workers.erase(workers.begin());
    return identity;
}

void Broker::queue_purge() {
    const int64_t clock = s_clock();
    if (const auto it = remove_if(workers.begin(), workers.end(), [clock](const Worker& w) { return w.expiry < clock; }); it != workers.end()) {
        cout << "Broker: purging " << distance(it, workers.end()) << " inactive workers" << endl;
        workers.erase(it, workers.end());
    }

    while (!client_queue.empty()) {
        if (auto&[message, enqueue_time] = client_queue.front(); clock - enqueue_time > 30000) { // 30 seconds timeout
            cerr << "E: Dropping Client request due to timeout" << endl;
            client_queue.pop_front();
        } else {
            break;

        }
    }

}

void Broker::run() {
    cout << "Broker running..." << endl;

    while (true) {
        zmq::pollitem_t items [] = {
            { backend, 0, ZMQ_POLLIN, 0 },
            { frontend, 0, ZMQ_POLLIN, 0 }
        };

        cout << "Broker: polling for workers and clients..." << endl;
        zmq::poll(items, 2, chrono::milliseconds(HEARTBEAT_INTERVAL));

        // Backend (workers)
        if (items[0].revents & ZMQ_POLLIN) {
            handle_worker_message();
        }

        // Frontend (clients)
        if (items[1].revents & ZMQ_POLLIN) {
            handle_client_message();
        }

        // Heartbeats
        if (s_clock() > heartbeat_at) {
            cout << "Broker: sending HEARTBEAT to all workers..." << endl;
            for (auto&[identity, expiry] : workers) {
                zmq::message_t worker_identity(identity.c_str(), identity.size());
                zmq::message_t empty;
                zmq::message_t heartbeat("HEARTBEAT", 9);

                backend.send(worker_identity, zmq::send_flags::sndmore);
                backend.send(empty, zmq::send_flags::sndmore);
                backend.send(heartbeat, zmq::send_flags::none);

                cout << "Broker: sent HEARTBEAT to worker " << identity << endl;
            }
            heartbeat_at = s_clock() + HEARTBEAT_INTERVAL;
        }

        queue_purge();
    }
}

void Broker::handle_worker_message() {
    cout << "Broker: received message from worker" << endl;
    zmq::message_t identity;
    zmq::message_t empty;
    zmq::message_t content;

    backend.recv(identity, zmq::recv_flags::none);
    backend.recv(empty, zmq::recv_flags::none);
    backend.recv(content, zmq::recv_flags::none);

    const string worker_identity(static_cast<char*>(identity.data()), identity.size());
    const string message(static_cast<char*>(content.data()), content.size());

    cout << "Broker: worker identity: " << worker_identity << endl;
    cout << "Broker: worker message: " << message << endl;

    if (message == "READY") {
        worker_append(worker_identity);
        cout << "Broker: worker " << worker_identity << " is READY" << endl;

        // Dispatch queued Client requests if any
        if (!client_queue.empty()) {
            const string queued_worker_id = worker_dequeue();
            auto [message, enqueue_time] = move(client_queue.front());
            client_queue.pop_front();

            zmq::message_t new_worker_identity(queued_worker_id.c_str(), queued_worker_id.size());

            // Forward client request to worker
            backend.send(new_worker_identity, zmq::send_flags::sndmore);
            backend.send(zmq::message_t(), zmq::send_flags::sndmore);
            for (size_t i = 1; i < message.size(); ++i) {
                backend.send(message[i], (i == message.size() - 1) ? zmq::send_flags::none : zmq::send_flags::sndmore);
            }

            cout << "Broker: forwarded request from Client to Worker " << queued_worker_id << endl;
        }
    } else if (message == "HEARTBEAT") {
        worker_refresh(worker_identity);
        cout << "Broker: worker " << worker_identity << " sent HEARTBEAT" << endl;
    } else {
        // Assume it's a response to a client request
        cout << "Broker: received response from Worker " << worker_identity << endl;
        frontend.send(identity, zmq::send_flags::sndmore);
        frontend.send(zmq::message_t(), zmq::send_flags::sndmore);
        frontend.send(content, zmq::send_flags::none);
        worker_append(worker_identity);
    }
}

void Broker::handle_client_message() {
    cout << "Broker: Received message from Client" << endl;
    vector<zmq::message_t> client_msg;

    // Receive all message parts from the Client
    while (true) {
        zmq::message_t part;
        frontend.recv(part, zmq::recv_flags::none);
        client_msg.emplace_back(move(part));
        if (!frontend.get(zmq::sockopt::rcvmore)) {
            break;
        }
    }

    // Extract Client identity and request
    const string client_identity(static_cast<char*>(client_msg[0].data()), client_msg[0].size());
    const string request(static_cast<char*>(client_msg[2].data()), client_msg[2].size());

    cout << "Broker: Client Identity: " << client_identity << endl;
    cout << "Broker: Client Request: " << request << endl;

    if (!workers.empty()) {
        const string worker_id = worker_dequeue();
        zmq::message_t worker_identity(worker_id.c_str(), worker_id.size());

        // Forward Client request to Worker
        backend.send(worker_identity, zmq::send_flags::sndmore);
        backend.send(zmq::message_t(), zmq::send_flags::sndmore); // Empty delimiter
        for (size_t i = 1; i < client_msg.size(); ++i) {
            backend.send(client_msg[i], (i == client_msg.size() - 1) ? zmq::send_flags::none : zmq::send_flags::sndmore);
        }

        cout << "Broker: Forwarded request from Client " << client_identity << " to Worker " << worker_id << endl;
    }
    else {
        cerr << "E: no workers available, enqueuing request from Client " << client_identity << endl;
        client_queue.push_back({move(client_msg), s_clock()});
    }
}








