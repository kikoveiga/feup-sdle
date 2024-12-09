//
// Created by kikoveiga on 09-12-2024.
//

#ifndef BROKER_H
#define BROKER_H

#include <string>
#include <vector>
#include <zmq.hpp>

using namespace std;

struct Worker {
    string identity;
    int64_t expiry;
};

class Broker {
    zmq::context_t context;
    zmq::socket_t frontend;
    zmq::socket_t backend;
    vector<Worker> workers;
    int64_t heartbeat_at;

    void worker_append(const string& identity);
    void worker_delete(const string& identity);
    void worker_refresh(const string& identity);
    string worker_dequeue();
    void queue_purge();

public:
    Broker();
    void run();
};

#endif //BROKER_H
