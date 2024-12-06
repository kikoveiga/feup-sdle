#ifndef SERVER_H
#define SERVER_H

#include <ShoppingList.h>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include "Message.h"

using json = nlohmann::json;
using namespace std;

class Server {

private:
    void handleRequest();
    void addItemToList(const string& list_id, const json& data);
    void markItemAcquired(const string& list_id, const json& data);
    void publishUpdate(const string& list_id);
    void handleSubscription();

public:
    zmq::context_t context;
    zmq::socket_t router_socket;
    zmq::socket_t publisher_socket;

    unordered_map<string, ShoppingList> shoppingLists;

    Server();
    void run();
};

#endif //SERVER_H
