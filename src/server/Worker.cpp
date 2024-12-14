#include "Worker.h"

#include <iostream>
#include "Message.h"
#include <thread>
#include <zmq.hpp>
#include "zhelpers.hpp"


#define HEARTBEAT_LIVENESS 3
#define HEARTBEAT_INTERVAL 11000

Worker::Worker(string backend_url, const int worker_id, Database& db) : backend_url(move(backend_url)), worker_id(worker_id), centralDatabase(db) {
    lock_guard lock(g_mutex);
    inMemoryShoppingLists = centralDatabase.loadAllLists();
}

void Worker::run() {
    zmq::context_t context(1);
    zmq::socket_t worker(context, ZMQ_DEALER);

    const string unique_id = s_set_id(worker); // Not sure if s_set_id is working correctly
    worker.set(zmq::sockopt::routing_id, unique_id);
    worker.connect(backend_url.c_str());

    cout << "Worker " << worker_id << " with ID " << unique_id << " connected to " << backend_url << endl;

    // Send READY message
    zmq::message_t ready_msg("READY", 5);
    worker.send(ready_msg, zmq::send_flags::none);
    cout << "Worker " << worker_id << " sent READY message" << endl;

    const auto timeout = chrono::milliseconds(HEARTBEAT_INTERVAL);

    while (true) {
        zmq::pollitem_t items[] = {{ static_cast<void *>(worker), 0, ZMQ_POLLIN, 0 }};
        zmq::poll(items, 1, timeout);

        if (items[0].revents & ZMQ_POLLIN) {
            zmq::message_t empty;
            zmq::message_t first_msg;

            worker.recv(empty, zmq::recv_flags::none);
            worker.recv(first_msg, zmq::recv_flags::none);

            string first_msg_str = first_msg.to_string();

            if (first_msg_str == "HEARTBEAT") {
                zmq::message_t hb_reply("HEARTBEAT", 9);
                worker.send(hb_reply, zmq::send_flags::none);
                cout << "Worker " << worker_id << ": HEARTBEAT received and replied" << endl;
            } else {
                zmq::message_t client_msg;
                worker.recv(client_msg, zmq::recv_flags::none);

                cout << "Worker " << worker_id << " processing " << first_msg_str << " request: " << client_msg.to_string() << endl;

                try {
                    handleRequest(unique_id, Message::from_string(client_msg.to_string()));

                    string response = "OK";
                    worker.send(zmq::buffer(response), zmq::send_flags::none);
                    cout << "Worker " << worker_id << " sent response: " << response << endl;
                } catch (const exception& e) {
                    cerr << "Error processing message: " << e.what() << endl;
                    string response = "ERROR";
                    worker.send(zmq::buffer(response), zmq::send_flags::none);
                    cout << "Worker " << worker_id << " sent response: " << response << endl;
                }
            }
        } else {
            cout << "Worker " << worker_id << " heartbeat timeout" << endl;
        }
    }
}

void Worker::handleRequest(const string& client_id, const Message& msg) {

    string list_id = msg.list_id;
    json data = msg.data;

    switch (msg.operation) {

        case Operation::CREATE_LIST: {
            if (inMemoryShoppingLists.find(list_id) != inMemoryShoppingLists.end()) throw invalid_argument("List already exists: " + msg.list_id);

            const ShoppingList list(list_id);
            inMemoryShoppingLists[list_id] = list;
            cout << "Created list: " << list_id << endl;

            centralDatabase.saveList(list);
            break;
        }

        case Operation::DELETE_LIST:
            if (inMemoryShoppingLists.erase(list_id) == 0) throw invalid_argument("List not found: " + list_id);
            cout << "Deleted list: " << list_id << endl;
            break;

        case Operation::ADD_ITEM_TO_LIST: {
            if (inMemoryShoppingLists.find(list_id) == inMemoryShoppingLists.end()) throw invalid_argument("List not found: " + list_id);

            const ShoppingItem item = ShoppingItem::from_json(data);
            inMemoryShoppingLists[list_id].add_item(item);
            cout << "Added " << item.getQuantity() << ' ' << item.getName() << " to " << list_id << endl;

            break;
        }

        case Operation::REMOVE_ITEM_FROM_LIST: {
            const auto it = inMemoryShoppingLists.find(list_id);
            if (it == inMemoryShoppingLists.end()) throw invalid_argument("List not found: " + list_id);

            if (!data.contains("name") || !data["name"].is_string()) throw invalid_argument("Invalid data: 'item_id' is required and must be a string");

            const string item_id = data["name"].get<string>();

            try {
                it->second.mark_item_acquired(item_id);
                cout << "Item '" << item_id << "' marked as acquired in: " << list_id << endl;

                centralDatabase.saveList(it->second);
            } catch (const exception& e) {
                cerr << "Error marking item as acquired: " << e.what() << endl;
            }

            break;
        }

        case Operation::SEND_ALL_LISTS: {

            json client_lists = msg.data;

            map<string, ShoppingList> clientShoppingLists;
            for (const auto& list_json : client_lists) {
                auto list = ShoppingList::from_json(list_json);
                clientShoppingLists[list.get_list_id()] = list;
            }

            for (auto& [list_id, list] : clientShoppingLists) {
                if (inMemoryShoppingLists.find(list_id) == inMemoryShoppingLists.end()) {
                    inMemoryShoppingLists[list_id].merge(list);
                } else {
                    inMemoryShoppingLists[list_id] = list;
                }
            }
        }

        default:
            throw invalid_argument("Invalid operation");

    }
}


