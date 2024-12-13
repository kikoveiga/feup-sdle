#ifndef WORKER_H
#define WORKER_H

#include "Database.h"
#include "Message.h"
#include "ShoppingList.h"

class Worker {
    void handle_message(const Message& msg, string& response);

    string backend_url;
    int worker_id;
    Database& db;

    mutex g_mutex;
    map<string, ShoppingList> inMemoryShoppingLists;

public:
    Worker(string backend_url, int worker_id, Database& db);
    [[noreturn]] void run() const;
};

#endif //WORKER_H
