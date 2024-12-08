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

void to_json(json& j, const Operation& op);
void from_json(const json& j, Operation& op);

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
