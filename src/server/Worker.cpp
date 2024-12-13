#include "Worker.h"

#include <iostream>
#include <thread>
#include <zmq.hpp>
#include "zhelpers.hpp"


#define HEARTBEAT_LIVENESS 3
#define HEARTBEAT_INTERVAL 1000

Worker::Worker(string backend_url, const int worker_id, Database& db) : backend_url(move(backend_url)), worker_id(worker_id), db(db) {
    lock_guard lock(g_mutex);
    inMemoryShoppingLists = db.loadAllLists();
}

void Worker::run() const {
    zmq::context_t context(1);
    zmq::socket_t worker(context, ZMQ_DEALER);

    const string unique_id = s_set_id(worker); // Not sure if s_set_id is working correctly
    worker.set(zmq::sockopt::routing_id, unique_id);
    worker.connect(backend_url.c_str());

    cout << "Worker " << worker_id << " with ID " << unique_id << " connected to " << backend_url << endl;

    // Send READY message
    zmq::message_t ready_msg("READY", 5);
    worker.send(ready_msg, zmq::send_flags::none);
    cout << "Worker " << worker_id << " sent READY message" << endl;

    const auto timeout = chrono::milliseconds(HEARTBEAT_INTERVAL);

    while (true) {
        zmq::pollitem_t items[] = {{ static_cast<void *>(worker), 0, ZMQ_POLLIN, 0 }};
        zmq::poll(items, 1, timeout);

        if (items[0].revents & ZMQ_POLLIN) {
            zmq::message_t empty;
            zmq::message_t content;

            worker.recv(empty, zmq::recv_flags::none);
            worker.recv(content, zmq::recv_flags::none);

            string message(static_cast<char *>(content.data()), content.size());
            cout << "Worker " << worker_id << " received message: " << message << endl;

            if (message == "HEARTBEAT") {
                zmq::message_t hb_reply("HEARTBEAT", 9);
                worker.send(hb_reply, zmq::send_flags::none);
                cout << "Worker " << worker_id << " sent HEARTBEAT reply" << endl;
            } else {
                cout << "Worker " << worker_id << " processing request: " << message << endl;

                this_thread::sleep_for(chrono::milliseconds(500));
                string response = "Processed: " + message;
                zmq::message_t response_msg(response.c_str(), response.size());
                worker.send(response_msg, zmq::send_flags::none);
                cout << "Worker " << worker_id << " sent response: " << response << endl;
            }
        } else {
            // Handle heartbeat timeouts
        }
    }
}

void Worker::handle_message(const Message& msg, string& response) {

}


