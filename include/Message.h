#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

enum class Operation {
    CREATE_LIST,
    DELETE_LIST,
    ADD_ITEM_TO_LIST,
    REMOVE_ITEM_FROM_LIST,

    SEND_ALL_LISTS, // Use data = {} to just sync with server
};

void to_json(json& j, const Operation& op);
void from_json(const json& j, Operation& op);

class Message {

public:
    Operation operation;
    string list_id;
    json data;

    Message(const Operation& operation, string list_id, json data);

    [[nodiscard]] string to_string() const;
    static Message from_string(const string& json_str);

    static json to_json(const Message& msg);
    static Message from_json(const json& j);

};

#endif //MESSAGE_H
