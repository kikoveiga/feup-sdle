#include "Message.h"

void to_json(json& j, const Operation& op) {
    switch (op) {
        case Operation::CREATE_LIST: j = "CREATE_LIST"; break;
        case Operation::DELETE_LIST: j = "DELETE_LIST"; break;
        case Operation::ADD_ITEM_TO_LIST: j = "ADD_ITEM_TO_LIST"; break;
        case Operation::REMOVE_ITEM_FROM_LIST: j = "REMOVE_ITEM"; break;
        case Operation::GET_LIST: j = "GET_LIST"; break;
        case Operation::GET_ALL_LISTS: j = "GET_ALL_LISTS"; break;
        default: j = "UNKNOWN_OPERATION"; break;
    }
}

void from_json(const json& j, Operation& op) {
    const std::string op_str = j.get<std::string>();
    if (op_str == "CREATE_LIST") op = Operation::CREATE_LIST;
    else if (op_str == "DELETE_LIST") op = Operation::DELETE_LIST;
    else if (op_str == "ADD_ITEM_TO_LIST") op = Operation::ADD_ITEM_TO_LIST;
    else if (op_str == "REMOVE_ITEM") op = Operation::REMOVE_ITEM_FROM_LIST;
    else if (op_str == "GET_LIST") op = Operation::GET_LIST;
    else if (op_str == "GET_ALL_LISTS") op = Operation::GET_ALL_LISTS;
    else throw std::invalid_argument("Invalid operation type: " + op_str);
}

Message::Message() : operation(Operation::GET_ALL_LISTS), data(json::object()) {}

string Message::to_string() const {
    json j;
    to_json(j, *this);
    return j.dump();
}

Message Message::from_string(const string& json_str) {
    const json j = json::parse(json_str);
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
