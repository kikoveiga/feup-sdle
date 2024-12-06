//
// Created by kikoveiga on 06-12-2024.
//

#ifndef CLIENT_H
#define CLIENT_H

#include <Message.h>
#include <zmq.hpp>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace std;

class Client {
public:
    zmq::context_t context;
    zmq::socket_t dealer_socket;

    Client();
    void send_request(Operation operation, const string &list_id, const json &data);
};

#endif //CLIENT_H
