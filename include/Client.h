//
// Created by kikoveiga on 06-12-2024.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <Message.h>
#include <ShoppingList.h>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <string>
#include <mongocxx/client.hpp>

using json = nlohmann::json;
using namespace std;

class Client {
    static int client_id_counter;
    string client_id;

    mongocxx::client localDatabase;
    unordered_map<string, ShoppingList> localShoppingLists;

    void syncWithServer();
    void loadFromLocalDatabase();
    void saveToLocalDatabase(const ShoppingList& list);

    zmq::context_t context;
    zmq::socket_t dealer_socket;

public:

    Client();
    string get_client_id() const;
    void send_request(Operation operation, const string &list_id, const json &data);
};

#endif //CLIENT_H
