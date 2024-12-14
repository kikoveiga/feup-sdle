#include "Broker.h"

#include <iostream>
#include "zhelpers.hpp"

#define HEARTBEAT_LIVENESS 3
#define HEARTBEAT_INTERVAL 10000

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
}

void Broker::worker_delete(const string& identity) {
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
        cout << "Purging " << distance(it, workers.end()) << " inactive workers" << endl;
        workers.erase(it, workers.end());
    }

    while (!client_queue.empty()) {
        if (auto&[client_id, client_msg, enqueue_time] = client_queue.front(); clock - enqueue_time > 30000) { // 30 seconds timeout
            cerr << "E: Dropping " << client_id.to_string() << " request due to timeout" << endl;
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
            for (auto&[identity, expiry] : workers) {
                zmq::message_t worker_identity(identity.c_str(), identity.size());
                zmq::message_t empty;
                zmq::message_t heartbeat("HEARTBEAT", 9);

                backend.send(worker_identity, zmq::send_flags::sndmore);
                backend.send(empty, zmq::send_flags::sndmore);
                backend.send(heartbeat, zmq::send_flags::none);

                cout << "HEARTBEAT to " << identity << endl;
            }
            heartbeat_at = s_clock() + HEARTBEAT_INTERVAL;
        }

        queue_purge();
    }
}

void Broker::handle_worker_message() {
    zmq::message_t worker_id;
    zmq::message_t worker_msg;

    backend.recv(worker_id, zmq::recv_flags::none);
    backend.recv(worker_msg, zmq::recv_flags::none);

    const string worker_id_str = worker_id.to_string();
    const string worker_msg_str = worker_msg.to_string();

    if (worker_msg_str == "READY") {
        worker_append(worker_id_str);
        cout << worker_id_str << " is READY" << endl;

        // Dispatch queued Client requests if any
        if (!client_queue.empty()) {
            const string queued_worker_id = worker_dequeue();
            auto [client_id, client_msg, enqueue_time] = move(client_queue.front());
            client_queue.pop_front();

            zmq::message_t new_worker_identity(queued_worker_id.c_str(), queued_worker_id.size());

            // Forward client request to worker
            backend.send(new_worker_identity, zmq::send_flags::sndmore);
            backend.send(zmq::message_t(), zmq::send_flags::sndmore);
            backend.send(client_id, zmq::send_flags::sndmore);
            backend.send(client_msg, zmq::send_flags::none);

            cout << "Forwarded request from Client to Worker " << queued_worker_id << endl;
        }
    } else if (worker_msg_str == "HEARTBEAT") {
        worker_refresh(worker_id_str);
        cout << "HEARTBEAT from " << worker_id_str << endl;
    } else {
        // Assume it's a response to a client request
        cout << "Received response from " << worker_id_str << endl;
        frontend.send(zmq::message_t(), zmq::send_flags::sndmore);
        frontend.send(worker_msg, zmq::send_flags::none);
        worker_append(worker_id_str);
    }
}

void Broker::handle_client_message() {

    zmq::message_t client_id;
    zmq::message_t client_msg;
    frontend.recv(client_id, zmq::recv_flags::none);
    frontend.recv(client_msg, zmq::recv_flags::none);

    const string client_id_str = client_id.to_string();
    const string client_msg_str = client_msg.to_string();

    cout << client_id_str << " request: " << client_msg_str << endl;

    if (!workers.empty()) {
        const string worker_id = worker_dequeue();

        // Forward Client request to Worker
        backend.send(zmq::message_t(worker_id), zmq::send_flags::sndmore);
        backend.send(zmq::message_t(), zmq::send_flags::sndmore);
        backend.send(client_id, zmq::send_flags::sndmore);
        backend.send(client_msg, zmq::send_flags::none);

        cout << "Forwarded request from " << client_id_str << " to " << worker_id << endl;
    }

    else {

        if (client_queue.size() < MAX_QUEUE_SIZE) {
            cerr << "E: no workers available, enqueuing request from: " << client_id_str << endl;
            client_queue.push_back({move(client_id), move(client_msg), s_clock()});
        } else {
            cerr << "E: Client request queue is full, dropping request from: " << client_id_str << endl;
        }
    }
}








