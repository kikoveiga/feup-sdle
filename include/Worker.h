#ifndef WORKER_H
#define WORKER_H

#include "Database.h"
#include "Message.h"
#include "ShoppingList.h"

class Worker {
    void handleRequest(const string& client_id, const Message& msg);

    string backend_url;
    int worker_id;
    Database& centralDatabase;

    mutex g_mutex;
    map<string, ShoppingList> inMemoryShoppingLists;

public:
    Worker(string backend_url, int worker_id, Database& db);
    [[noreturn]] void run();
};

#endif //WORKER_H
