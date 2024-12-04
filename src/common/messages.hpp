#pragma once

#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class Operation {
    CREATE_LIST,
    DELETE_LIST,
    ADD_ITEM,
    REMOVE_ITEM,
    GET_LIST,
    GET_ALL_LISTS
};

class Message {
public:
    Operation operation;
    std::string list_id;
    json data;

    std::string to_string() const {
        json j;
        j["operation"] = operation;
        j["list_id"] = list_id;
        j["data"] = data;
        return j.dump();
    }

    static Message from_string(const std::string& json_str) {
        const json j = json::parse(json_str);
        Message msg;
        msg.operation = j.value("operation", Operation::GET_ALL_LISTS);
        msg.list_id = j.value("list_id", "");
        msg.data = j.value("data", json::object());
        return msg;
    }
};
