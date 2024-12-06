#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

enum class Operation {
    CREATE_LIST,
    DELETE_LIST,
    ADD_ITEM,
    REMOVE_ITEM,
    UPDATE_LIST,
    GET_LIST,
    GET_ALL_LISTS
};

inline void to_json(json& j, const Operation& op) {
    switch (op) {
        case Operation::CREATE_LIST: j = "CREATE_LIST"; break;
        case Operation::DELETE_LIST: j = "DELETE_LIST"; break;
        case Operation::ADD_ITEM: j = "ADD_ITEM"; break;
        case Operation::REMOVE_ITEM: j = "REMOVE_ITEM"; break;
        case Operation::GET_LIST: j = "GET_LIST"; break;
        case Operation::GET_ALL_LISTS: j = "GET_ALL_LISTS"; break;
        case Operation::UPDATE_LIST: j = "UPDATE_LIST"; break;
        default: j = "UNKNOWN_OPERATION"; break;
    }
}

class Message {

public:
    Operation operation;
    string list_id;
    json data;

    Message();

    [[nodiscard]] string to_string() const;

    static Message from_string(const string& json_str);

    friend void to_json(json& j, const Message& msg);
    friend void from_json(const json& j, Message& msg);

};

#endif //MESSAGE_H
