#include "Server.h"

#include <iostream>

Server::Server() : context(1), router_socket(context, zmq::socket_type::router), publisher_socket(context, zmq::socket_type::pub) {
    router_socket.bind("tcp://*:5555");
    publisher_socket.bind("tcp://*:5556");
}

void Server::run() {
    cout << "Server running..." << endl;
    while(true) {
        zmq::message_t identity;
        zmq::message_t client_message;

        router_socket.recv(identity, zmq::recv_flags::none);
        string identity_str = identity.to_string();
        cout << "Received message from client: " << identity_str << endl;
        router_socket.recv(client_message, zmq::recv_flags::none);

        cout << "Received message from client" << endl;
        cout << "Message: " << client_message.to_string() << endl;
        Message msg = Message::from_string(client_message.to_string());
        cout << "Received message: " << msg.to_string() << endl;

        string response = "OK";
        router_socket.send(client_message, zmq::send_flags::none);
    }
}

void Server::addItemToList(const string& list_id, const json &data) {
    const ShoppingItem item(data.at("name"));
    shoppingLists[list_id].add_item(item);
}

void Server::publishUpdate(const string& list_id) {
    Message msg;
    msg.operation = Operation::UPDATE_LIST;
    msg.list_id = list_id;
    msg.data = shoppingLists[list_id];
    publisher_socket.send(zmq::buffer(msg.to_string()), zmq::send_flags::none);
}

int main() {
    Server server;
    server.run();
    return 0;
}