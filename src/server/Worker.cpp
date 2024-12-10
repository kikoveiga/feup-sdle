#include "Worker.h"
#include "common_includes.h"

#define HEARTBEAT_LIVENESS 3
#define HEARTBEAT_INTERVAL 1000

Worker::Worker(const string &backend_url, const int worker_id, Database &db) : backend_url(backend_url), worker_id(worker_id), db(db) {
    lock_guard lock(g_mutex);
    inMemoryShoppingLists = db.loadAllLists();
}

void Worker::run() const {
    zmq::context_t context(1);
    zmq::socket_t worker(context, ZMQ_DEALER);
    s_set_id(worker);
    worker.connect(backend_url.c_str());

    s_send(static_cast<void*>(worker), "READY");
    const auto timeout = chrono::milliseconds(HEARTBEAT_INTERVAL);

    while (true) {
        zmq::pollitem_t items[] = {{ static_cast<void *>(worker), 0, ZMQ_POLLIN, 0 }};
        zmq::poll(items, 1, timeout);

        if (items[0].revents & ZMQ_POLLIN) {
            zmsg msg(worker);
        }
    }
}

