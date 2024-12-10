#include <thread>

#include "Worker.h"
#include "Database.h"
#include <mongocxx/instance.hpp>

int main() {
    mongocxx::instance instance{};

    constexpr int WORKER_COUNT = 4;
    vector<thread> workers;
    for (int i = 0; i < WORKER_COUNT; ++i) {
        workers.emplace_back([i] {
            Database db;
            const Worker w("tcp://localhost:5556", i, db);
            w.run();
        });

    }
    cout << "Started " << WORKER_COUNT << " workers" << endl;
    for (auto &t : workers) {
        t.join();
    }
    return 0;
}