#include "Client.h"

#include <iostream>
#include <fstream>
#include <Message.h>

int Client::client_id_counter = 0;

Client::Client() : context(1), dealer_socket(context, zmq::socket_type::dealer) {
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

        saveToLocalDatabase();
    }
}


void Client::send_request(const Operation operation, const string& list_id, const json& data) {
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

    ifstream infile("shoppingLists.json");
    if (!infile.is_open()) {
        cout << "No local database found." << endl;
        return;
    }

    json json_data;
    infile >> json_data;
    infile.close();

    for (const auto& list_json : json_data["shoppingLists"]) {
        auto list = ShoppingList::from_json(list_json);
        localShoppingLists[list.get_list_id()] = list;
    }

    cout << "Loaded " << localShoppingLists.size() << " lists from local database." << endl;
}

void Client::saveToLocalDatabase() {
    json json_data;
    json_data["shoppingLists"] = json::array();

    for (const auto& [list_id, list] : localShoppingLists) {
        json_data["shoppingLists"].push_back(list.to_json());
    }

    ofstream outfile("shoppingLists.json");
    if (!outfile.is_open()) {
        cerr << "Error opening file" << endl;
        return;
    }

    outfile << json_data.dump(4);
    outfile.close();

    cout << "Saved " << localShoppingLists.size() << " lists to local database." << endl;
}

Client::~Client() {
    cout << "Client shutting down..." << endl;
    saveToLocalDatabase();
}

int main() {

    Client client;

    const json data = {
        {"name", "Milk"},
        {"quantity", 2}
    };

    client.send_request(Operation::CREATE_LIST, "list1", {});
    // client.send_request(Operation::ADD_ITEM_TO_LIST, "list1", data);


    return 0;
}