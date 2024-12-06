#include "Client.h"

#include <iostream>
#include <Message.h>

Client::Client() : context(1), dealer_socket(context, zmq::socket_type::dealer) {
    dealer_socket.set(zmq::sockopt::routing_id, "Client1");
    dealer_socket.connect("tcp://localhost:5555");
}

void Client::send_request(const Operation operation, const string &list_id, const json &data) {
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

int main() {
    Client client;
    client.send_request(Operation::ADD_ITEM, "list1", {{"name", "Milk"}});
    return 0;
}