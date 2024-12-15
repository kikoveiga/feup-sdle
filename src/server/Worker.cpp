#include "Worker.h"

#include <iostream>
#include "Message.h"
#include "ShoppingItem.h"
#include <thread>
#include <zmq.hpp>
#include "zhelpers.hpp"


#define HEARTBEAT_LIVENESS 3
#define HEARTBEAT_INTERVAL 2000

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

            cout << empty.to_string() << endl;
            cout << first_msg.to_string() << endl;

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

                    json data;
                    data["shoppingLists"] = json::array();

                    for (const auto& [listName, list] : inMemoryShoppingLists) {
                        data["shoppingLists"].push_back(list.to_json());
                    }

                    worker.send(zmq::buffer(first_msg_str), zmq::send_flags::sndmore);
                    worker.send(zmq::buffer(to_string(data)), zmq::send_flags::none);
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
    const string list_id = msg.list_id;
    json data = msg.data;

    inMemoryShoppingLists = centralDatabase.loadAllLists();

    switch (msg.operation) {
        case Operation::SEND_ALL_LISTS: {
            if (data.contains("shoppingLists")) {
                auto shoppingLists = data.at("shoppingLists"); // Retrieve the array of shopping lists
                
                // Iterate through each shopping list
                for (const auto& list_json : shoppingLists) {
                    if (!list_json.contains("name")) {
                        cerr << "Invalid shopping list: Missing 'name'.\n";
                        continue;
                    }
                    
                    // Get the list name
                    string listName = list_json.at("name");

                    cout << "Received shopping list: " << list_json << "\n";

                    // Check if the list already exists in inMemoryShoppingLists
                    auto it = inMemoryShoppingLists.find(listName);
                    if (it == inMemoryShoppingLists.end()) {
                        // Add a new shopping list
                        inMemoryShoppingLists[listName] = ShoppingList::from_json(list_json);
                    } else {
                        // Merge the existing list with the new data
                        it->second.merge(ShoppingList::from_json(list_json));
                        cout << it->second.to_json();
                        cout << "Merged shopping list: " << listName << "\n";
                    }
                }
            } else {
                cerr << "Error: Missing 'shoppingLists' in payload.\n";
            }
            break;
        }

        default:
            throw invalid_argument("Invalid operation");
    }

    centralDatabase.saveAllLists(inMemoryShoppingLists);
}



