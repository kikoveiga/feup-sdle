//
// Created by kikoveiga on 09-12-2024.
//

#ifndef BROKER_H
#define BROKER_H

#include <deque>
#include <string>
#include <vector>
#include <zmq.hpp>

using namespace std;

struct Worker {
    string identity;
    int64_t expiry;
};

struct QueuedRequest {
    vector<zmq::message_t> message;
    int64_t enqueue_time;
};

class Broker {
    zmq::context_t context;
    zmq::socket_t frontend;
    zmq::socket_t backend;
    vector<Worker> workers;

    static constexpr size_t MAX_QUEUE_SIZE = 100;
    deque<QueuedRequest> client_queue;

    int64_t heartbeat_at;

    void worker_append(const string& identity);
    void worker_delete(const string& identity);
    void worker_refresh(const string& identity);
    void handle_worker_message();
    void handle_client_message();
    string worker_dequeue();
    void queue_purge();

public:
    Broker();
    [[noreturn]] void run();
};

#endif //BROKER_H
