#include "Message.h"

#include <utility>

void to_json(json& j, const Operation& op) {
    switch (op) {
        case Operation::CREATE_LIST: j = "CREATE_LIST"; break;
        case Operation::DELETE_LIST: j = "DELETE_LIST"; break;
        case Operation::ADD_ITEM_TO_LIST: j = "ADD_ITEM_TO_LIST"; break;
        case Operation::REMOVE_ITEM_FROM_LIST: j = "REMOVE_ITEM"; break;
        case Operation::SEND_ALL_LISTS: j = "SEND_ALL_LISTS"; break;
        default: j = "UNKNOWN_OPERATION"; break;
    }
}

void from_json(const json& j, Operation& op) {
    const std::string op_str = j.get<std::string>();
    if (op_str == "CREATE_LIST") op = Operation::CREATE_LIST;
    else if (op_str == "DELETE_LIST") op = Operation::DELETE_LIST;
    else if (op_str == "ADD_ITEM_TO_LIST") op = Operation::ADD_ITEM_TO_LIST;
    else if (op_str == "REMOVE_ITEM") op = Operation::REMOVE_ITEM_FROM_LIST;
    else if (op_str == "SEND_ALL_LISTS") op = Operation::SEND_ALL_LISTS;
    else throw std::invalid_argument("Invalid operation type: " + op_str);
}

Message::Message(const Operation& operation, string list_id, json data) : operation(operation), list_id(move(list_id)), data(move(data)) {}


string Message::to_string() const {
    return to_json(*this).dump();
}

Message Message::from_string(const string& json_str) {
    Message msg = from_json(json::parse(json_str));
    return msg;
}

json Message::to_json(const Message& msg) {
    return json{
        {"operation", msg.operation},
        {"list_id", msg.list_id},
        {"data", msg.data}
    };
}

Message Message::from_json(const json& j) {
    return Message {
        j.at("operation").get<Operation>(),
        j.at("list_id").get<string>(),
        j.at("data")
    };

}
