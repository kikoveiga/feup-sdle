#include "Message.h"

#include <iostream>

Message::Message() : operation(Operation::GET_ALL_LISTS), data(json::object()) {}

string Message::to_string() const {
    cout << "Here!";
    json j;
    to_json(j, *this);
    return j.dump();
}

Message Message::from_string(const string& json_str) {
    cout << "Here!";
    const json j = json::parse(json_str);
    cout << "Here!";
    Message msg;

    from_json(j, msg);

    return msg;
}

void to_json(json& j, const Message& msg) {
    j = json{
        {"operation", msg.operation},
        {"list_id", msg.list_id},
        {"data", msg.data}
    };
}

void from_json(const json& j, Message& msg) {
    j.at("operation").get_to(msg.operation);
    j.at("list_id").get_to(msg.list_id);
    j.at("data").get_to(msg.data);
}
