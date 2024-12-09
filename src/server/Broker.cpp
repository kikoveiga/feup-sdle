#include "Broker.h"
#include <iostream>
#include <zmq.hpp>
#define HEARTBEAT_LIVENESS 3
#define HEARTBEAT_INTERVAL 1000

Broker::Broker() : context(1), frontend(context, ZMQ_ROUTER), backend(context, ZMQ_ROUTER) {
    frontend.bind("tcp://*:5555");
    backend.bind("tcp://*:5556");
    heartbeat_at = s_clock() + HEARTBEAT_INTERVAL;
}

void Broker::worker_append(const string &identity) {
    for (auto& w : workers) {
        if (w.identity == identity) {
            cerr << "E: duplicate worker identity (" << identity << ")" << endl;
            exit(1);
        }
    }

    Worker w;
    w.identity = identity;
    w.expiry = s_clock() + HEARTBEAT_LIVENESS * HEARTBEAT_INTERVAL;
    workers.push_back(w);
}

void Broker::worker_delete(const string &identity) {
    for (auto it = workers.begin(); it < workers.end(); it++) {
        if (it->identity == identity) {
            workers.erase(it);
            break;
        }
    }
}

void Broker::worker_refresh(const string &identity) {
    bool found = false;
    for (auto& w : workers) {
        if (w.identity == identity) {
            w.expiry = s_clock() + HEARTBEAT_LIVENESS * HEARTBEAT_INTERVAL;
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
    int64_t clock = s_clock();
    for (auto it = workers.begin(); it < workers.end();) {
        if (clock > it->expiry) {
            it = workers.erase(it);
        } else ++it;
    }
}

void Broker::run() {
    while (true) {
        zmq::pollitem_t items [] = {
            { backend, 0, ZMQ_POLLIN, 0 },
            { frontend, 0, ZMQ_POLLIN, 0 }
        };

        int rc = (!workers.empty()) ? zmq::poll(items, 2, HEARTBEAT_INTERVAL) : zmq::poll(items, 1, HEARTBEAT_INTERVAL);

        // Backend (workers)
        if (items[0].revents & ZMQ_POLLIN) {
            zmsg msg(backend);
            std::string identity(msg.unwrap());

            if (msg.parts() == 1) {
                std::string command = msg.popstr();
                if (command == "READY") {
                    worker_delete(identity);
                    worker_append(identity);
                } else if (command == "HEARTBEAT") {
                    worker_refresh(identity);
                } else {
                    std::cerr << "E: invalid message from " << identity << std::endl;
                    msg.dump();
                }
            } else {
                // Reply from worker to client
                msg.send(frontend);
                worker_append(identity);
            }
        }

        // Frontend (clients)
        if (items[1].revents & ZMQ_POLLIN) {
            zmsg msg(frontend);
            if (!workers.empty()) {
                std::string worker_id = worker_dequeue();
                msg.wrap(worker_id.c_str(), NULL);
                msg.send(backend);
            } else {
                std::cerr << "E: no workers available, dropping request." << std::endl;
            }
        }

        // Heartbeats
        if (s_clock() > heartbeat_at) {
            for (auto &w : workers) {
                zmsg heartbeat("HEARTBEAT");
                heartbeat.wrap(w.identity.c_str(), NULL);
                heartbeat.send(backend);
            }
            heartbeat_at = s_clock() + HEARTBEAT_INTERVAL;
        }
        queue_purge();
    }
}







