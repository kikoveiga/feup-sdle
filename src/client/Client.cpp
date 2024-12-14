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

void Client::addShoppingList(const string &name, const ShoppingList &list) {
    localShoppingLists[name] = list;
}


void Client::syncWithServer() {
    send_request(Operation::SEND_ALL_LISTS, "", {});
    zmq::message_t reply;
    dealer_socket.recv(reply, zmq::recv_flags::none);

    auto response = json::parse(reply.to_string());
    for (const auto& list_json : response["lists"]) {
        auto list = ShoppingList::from_json(list_json);
        localShoppingLists[list.getName()] = list;

        saveToLocalDatabase();
    }
}

void Client::send_request(const Operation operation, const string& list_id, const json& data) {
    const Message msg(operation, list_id, data);

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

    ifstream infile("../clientsData/" + client_id + ".json");
    if (!infile.is_open()) {
        cout << "No local database found." << endl;
        return;
    }

    // Check if the file is empty
    infile.seekg(0, ios::end);
    if (infile.tellg() == 0) {
        cout << "Local database is empty." << endl;
        infile.close();
        return;
    }
    infile.seekg(0, ios::beg);

    json json_data;
    infile >> json_data;
    infile.close();

    for (const auto& list_json : json_data["shoppingLists"]) {
        auto list = ShoppingList::from_json(list_json);
        localShoppingLists[list.getName()] = list;
    }

    cout << "Loaded " << localShoppingLists.size() << " lists from local database." << endl;
}

void Client::saveToLocalDatabase() {

    cout << "Saving lists to local database..." << endl;
    json json_data;
    json_data["shoppingLists"] = json::array();

    for (const auto& [list_id, list] : localShoppingLists) {
        json_data["shoppingLists"].push_back(list.to_json());
    }

    ofstream outfile("../clientsData/" + client_id + ".json");
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

    const ShoppingList list1("Groceries");
    client.addShoppingList(list1.getName(), list1);

    while (true) {}

    return 0;
}