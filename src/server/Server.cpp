#include "Server.h"

#include <iostream>
#include <mongocxx/instance.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

Server::Server() : context(1), router_socket(context, zmq::socket_type::router), publisher_socket(context, zmq::socket_type::pub), centralDatabase(mongocxx::uri{}) {
    router_socket.bind("tcp://*:5555");
    publisher_socket.bind("tcp://*:5556");
    loadFromCentralDatabase();
}

void Server::loadFromCentralDatabase() {
    const auto db = centralDatabase["server_db"];
    auto collection = db["shoppingLists"];
    auto cursor = collection.find({});

    for (const auto& doc : cursor) {
        auto json_str = bsoncxx::to_json(doc);
        auto json_obj = json::parse(json_str);

        ShoppingList list = ShoppingList::from_json(json_obj);

        inMemoryShoppingLists[list.get_list_id()] = list;
    }

    cout << "Loaded " << inMemoryShoppingLists.size() << " list" << (inMemoryShoppingLists.size() == 1 ? "" : "s") << " from central database" << endl;
}

void Server::saveListToCentralDatabase(const string& list_id) {
    try {
        const auto db = centralDatabase["server_db"];
        auto collection = db["shoppingLists"];

        const json list_json = inMemoryShoppingLists[list_id].to_json();
        const bsoncxx::document::value doc = bsoncxx::from_json(list_json.dump());

        bsoncxx::builder::stream::document filter_builder, update_builder;
        filter_builder << "list_id" << list_id;
        update_builder << "$set" << doc.view();

        collection.update_one(filter_builder.view(), update_builder.view(), mongocxx::options::update().upsert(true));

        cout << "Saved list to central database: " << list_id << endl;
    } catch (const exception& e) {
        cerr << "Error saving list to central database: " << e.what() << endl;
    }
}

void Server::deleteListFromCentralDatabase(const string &list_id) const {
    try {
        const auto db = centralDatabase["server_db"];
        auto collection = db["shoppingLists"];

        bsoncxx::builder::stream::document filter_builder;
        filter_builder << "list_id" << list_id;

        collection.delete_one(filter_builder.view());

        cout << "Deleted list from central database: " << list_id << endl;
    } catch (const exception& e) {
        cerr << "Error deleting list from central database: " << e.what() << endl;
    }
}



void Server::run() {
    cout << "Server running..." << endl;
    while(true) {
        zmq::message_t identity;
        zmq::message_t client_message;

        router_socket.recv(identity, zmq::recv_flags::none);
        string client_id(static_cast<char*>(identity.data()), identity.size());

        router_socket.recv(client_message, zmq::recv_flags::none);
        string received_str(static_cast<char*>(client_message.data()), client_message.size());
        cout << "Received message from " << client_id << ": " << received_str << endl;

        try {
            Message msg = Message::from_string(received_str);
            handleRequest(client_id, msg);

            string response = "OK";
            router_socket.send(identity, zmq::send_flags::sndmore);
            router_socket.send(client_message, zmq::send_flags::none);
        } catch (const exception& e) {
            cout << "Error processing message: " << e.what() << endl;

            string response = "ERROR";
            router_socket.send(identity, zmq::send_flags::sndmore);
            router_socket.send(zmq::buffer(response), zmq::send_flags::none);
        }
    }
}

void Server::handleRequest(const string& client_id, const Message& msg) {
    switch (msg.operation) {

        case Operation::CREATE_LIST:
            if (inMemoryShoppingLists.find(msg.list_id) != inMemoryShoppingLists.end()) throw invalid_argument("List already exists: " + msg.list_id);
            inMemoryShoppingLists[msg.list_id] = ShoppingList(msg.list_id);
            cout << "Created list: " << msg.list_id << endl;

            saveListToCentralDatabase(msg.list_id);
            break;

        case Operation::DELETE_LIST:
            if (inMemoryShoppingLists.erase(msg.list_id) == 0) throw invalid_argument("List not found: " + msg.list_id);
            cout << "Deleted list: " << msg.list_id << endl;
            break;

        case Operation::ADD_ITEM_TO_LIST:
            addItemToList(msg.list_id, msg.data);
            break;

        case Operation::REMOVE_ITEM_FROM_LIST:
            markItemAcquired(msg.list_id, msg.data);
            cout << "Marked item acquired in list: " << msg.list_id << endl;
            break;

        case Operation::GET_LIST:
            if (inMemoryShoppingLists.find(msg.list_id) == inMemoryShoppingLists.end()) throw invalid_argument("List not found: " + msg.list_id);

            {
                Message response;
                response.operation = Operation::GET_LIST;
                response.list_id = msg.list_id;
                response.data = inMemoryShoppingLists[msg.list_id].to_json();
                router_socket.send(zmq::buffer(response.to_string()), zmq::send_flags::none);
            }
            cout << "Retrieved list: " << msg.list_id << endl;
            break;

        case Operation::GET_ALL_LISTS: {
            Message response;
            response.operation = Operation::GET_ALL_LISTS;

            json all_lists = json::array();
            for (const auto& [list_id, list] : inMemoryShoppingLists) {
                all_lists.push_back(list.to_json());
            }
            response.data = all_lists;
            router_socket.send(zmq::buffer(response.to_string()), zmq::send_flags::none);

            cout << "Retrieved all lists" << endl;
            break;
        }

        default:
            throw invalid_argument("Invalid operation");

    }
}

void Server::addItemToList(const string& list_id, const json &data) {
    if (inMemoryShoppingLists.find(list_id) == inMemoryShoppingLists.end()) throw invalid_argument("List not found: " + list_id);

    const ShoppingItem item = ShoppingItem::from_json(data);
    inMemoryShoppingLists[list_id].add_item(item);
    cout << "Added " << item.getQuantity() << ' ' << item.getName() << " to " << list_id << endl;
}

void Server::markItemAcquired(const string &list_id, const json& data) {
    const auto it = inMemoryShoppingLists.find(list_id);
    if (it == inMemoryShoppingLists.end()) throw invalid_argument("List not found: " + list_id);

    if (!data.contains("name") || !data["name"].is_string()) throw invalid_argument("Invalid data: 'item_id' is required and must be a string");

    const string item_id = data["name"].get<string>();

    try {
        it->second.mark_item_acquired(item_id);
        cout << "Item '" << item_id << "' marked as acquired in list: " << list_id << endl;

        saveListToCentralDatabase(list_id);
    } catch (const exception& e) {
        cerr << "Error marking item as acquired: " << e.what() << endl;
    }
}

int main() {

    mongocxx::instance instance{};

    Server server;
    server.run();
    return 0;
}