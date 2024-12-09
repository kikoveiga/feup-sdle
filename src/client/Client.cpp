#include "Client.h"

#include <iostream>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <Message.h>

int Client::client_id_counter = 0;

Client::Client() : localDatabase(mongocxx::uri{}), context(1), dealer_socket(context, zmq::socket_type::dealer) {
    client_id = "Client" + to_string(++client_id_counter);
    cout << client_id << " running..." << endl;
    dealer_socket.set(zmq::sockopt::routing_id, client_id);
    dealer_socket.connect("tcp://localhost:5555");

    loadFromLocalDatabase();
}

string Client::get_client_id() const {
    return client_id;
}

void Client::syncWithServer() {
    send_request(Operation::GET_ALL_LISTS, "", {});
    zmq::message_t reply;
    dealer_socket.recv(reply, zmq::recv_flags::none);

    auto response = json::parse(reply.to_string());
    for (const auto& list_json : response["lists"]) {
        auto list = ShoppingList::from_json(list_json);
        localShoppingLists[list.get_list_id()] = list;

        saveToLocalDatabase(list);
    }
}


void Client::send_request(const Operation operation, const string &list_id, const json& data) {
    Message msg;
    msg.operation = operation;
    msg.list_id = list_id;
    msg.data = data;

    string request_str = msg.to_string();
    cout << "Sending request: " << request_str << endl;

    dealer_socket.send(zmq::buffer(request_str), zmq::send_flags::none);

    zmq::message_t reply;
    if (const auto result = dealer_socket.recv(reply, zmq::recv_flags::none); !result) {
        cout << "Error receiving reply from server" << endl;
        return;
    }
    cout << "Server response: " << reply.to_string() << endl;
}

void Client::loadFromLocalDatabase() {

    const auto db = localDatabase["client_db"];
    auto collection = db["shoppingLists"];
    auto cursor = collection.find({});

    for (const auto& doc : cursor) {
        string json_str = bsoncxx::to_json(doc);
        auto json_obj = json::parse(json_str);

        auto list = ShoppingList::from_json(json_obj);

        localShoppingLists[list.get_list_id()] = list;
    }

    cout << "Loaded " << localShoppingLists.size() << " lists from local database." << endl;
}

void Client::saveToLocalDatabase(const ShoppingList &list) {
    const auto db = localDatabase["client_db"];
    auto collection = db["shoppingLists"];

    collection.delete_many({});

    for (const auto& [list_id, list] : localShoppingLists) {
        json json_obj = list.to_json();
    }


}


int main() {

    mongocxx::instance instance{};

    Client client;

    const json data = {
        {"name", "Milk"},
        {"quantity", 2}
    };

    // client.send_request(Operation::CREATE_LIST, "list1", {});
    client.send_request(Operation::ADD_ITEM_TO_LIST, "list1", data);


    return 0;
}