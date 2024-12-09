#ifndef WORKER_H
#define WORKER_H

#include "Message.h"
#include "ShoppingList.h"

class Worker {
    void handle_message(const Message& msg, string& response);

    string backend_url;
    int worker_id;

    static mutex g_mutex;
    static map<string, ShoppingList> inMemoryShoppingLists;

};
#endif //WORKER_H
