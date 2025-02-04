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

using json = nlohmann::json;
using namespace std;

class Client {
    string username;
    unordered_map<string, ShoppingList> localShoppingLists;
    string lastSync; 

    static string getCurrentTimestamp() ;
    void loadFromLocalDatabase();
    void saveToLocalDatabase();

    zmq::context_t context;
    zmq::socket_t dealer_socket;

public:

    Client();
    ~Client();

    void loginUser(const string& username);
    string getLastSync() const;
    string getUsername() const;
    void syncWithServer();
    
    void addShoppingList(const string& name, const ShoppingList& list);
    void send_request(const Operation& operation, const json& data);
    static void cli();
};

#endif //CLIENT_H
