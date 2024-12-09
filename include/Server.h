#ifndef SERVER_H
#define SERVER_H

#include <ShoppingList.h>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <mongocxx/client.hpp>
#include "Message.h"

using json = nlohmann::json;
using namespace std;

class Server {

    zmq::context_t context;
    zmq::socket_t router_socket;
    zmq::socket_t publisher_socket;

    mongocxx::client centralDatabase;
    unordered_map<string, ShoppingList> inMemoryShoppingLists;

    void initializeDatabase();
    void loadFromCentralDatabase();
    void saveListToCentralDatabase(const string& list_id);
    void deleteListFromCentralDatabase(const string& list_id) const;
    void shardData();

    void handleRequest(const string& client_id, const Message& msg);
    void addItemToList(const string& list_id, const json& data);
    void markItemAcquired(const string& list_id, const json& data);
    void publishUpdate(const string& list_id);
    void handleSubscription();

public:

    Server();
    void run();
};

#endif //SERVER_H
