#include "Client.h"

#include <iostream>
#include <fstream>
#include <Message.h>


Client::Client() : context(1), dealer_socket(context, zmq::socket_type::dealer) {
    cout << "Client initialized. Awaiting user login...\n";
}

void Client::loginUser(const string& username) {
    this->username = username;
    dealer_socket.set(zmq::sockopt::routing_id, username);
    dealer_socket.set(zmq::sockopt::sndtimeo, 5000);
    dealer_socket.set(zmq::sockopt::rcvtimeo, 30000);
    dealer_socket.connect("tcp://localhost:5555");

    loadFromLocalDatabase();
}

string Client::getLastSync() const {
    return lastSync.empty() ? "Never" : lastSync;
}


void Client::addShoppingList(const string& name, const ShoppingList& list) {
    if (localShoppingLists.find(name) != localShoppingLists.end()) {
        cout << "List \"" << name << "\" already exists.\n";
        return;
    }
    localShoppingLists[name] = list;
    cout << "Added shopping list: " << name << "\n";
}

void Client::syncWithServer() {
    // Prepare data to send (all local lists)
    json data;
    data["shoppingLists"] = json::array();
    for (const auto& [listName, list] : localShoppingLists) {
        data["shoppingLists"].push_back(list.to_json());
    }

    // Send request to the server
    send_request(Operation::SEND_ALL_LISTS, data);

    // Receive and process the server's reply
    zmq::message_t reply;
    if (!dealer_socket.recv(reply, zmq::recv_flags::none)) {
        cerr << "Error: No reply from server during sync.\n";
        return;
    }

    try {
        auto response = json::parse(reply.to_string());
        cout << "Received response from server: " << response.dump(4) << "\n";

        if (response.contains("shoppingLists")) {
            localShoppingLists.clear();
            for (const auto& list_json : response["shoppingLists"]) {
                auto list = ShoppingList::from_json(list_json);
                localShoppingLists[list.getName()] = list;
            }
        }
    } catch (const exception& e) {
        cerr << "Error parsing server response: " << e.what() << "\n";
        return;
    }

    lastSync = getCurrentTimestamp();
    saveToLocalDatabase();
    cout << "Synchronized with server at: " << lastSync << "\n";
}

void Client::send_request(const Operation& operation, const json& data) {
    const Message msg(operation, "", data);
    string request_str = msg.to_string();
    cout << "Sending request: " << request_str << "\n";
    try {
        dealer_socket.send(zmq::buffer(request_str), zmq::send_flags::none);
        cout << "Request sent successfully.\n";
    } catch (const exception& e) {
        cerr << "Error sending request: " << e.what() << "\n";
    }
}

void Client::loadFromLocalDatabase() {
    const string file_path = "../clientsData/" + username + ".json";
    ifstream infile(file_path);
    if (!infile.is_open()) {
        cerr << "No local database found for user: " << username << "\n";
        return;
    }

    json json_data;
    infile >> json_data;
    infile.close();

    localShoppingLists.clear();
    for (const auto& list_json : json_data["shoppingLists"]) {
        auto list = ShoppingList::from_json(list_json);
        localShoppingLists[list.getName()] = list;
    }

    if (json_data.contains("lastSync")) {
        lastSync = json_data["lastSync"];
        cout << "Last synchronized at: " << lastSync << "\n";
    } else {
        lastSync.clear();
    }

    cout << "Loaded " << localShoppingLists.size() << " list"
              << (localShoppingLists.size() != 1 ? "s" : "") 
              << " for: " << username << "\n";
}

void Client::saveToLocalDatabase() {
    using namespace filesystem;

    const string directory = "../clientsData/";
    if (!exists(directory)) {
        if (!create_directory(directory)) {
            cerr << "Error creating directory: " << directory << "\n";
            return;
        }
    }

    json json_data;
    json_data["lastSync"] = lastSync.empty() ? "Never" : lastSync;
    json_data["shoppingLists"] = json::array();

    for (const auto& [list_name, list] : localShoppingLists) {
        json_data["shoppingLists"].push_back(list.to_json());
    }

    ofstream outfile(directory + username + ".json");
    if (!outfile.is_open()) {
        cerr << "Error saving data for user: " << username << "\n";
        return;
    }

    outfile << json_data.dump(4);
    outfile.close();

    cout << "Saved " << localShoppingLists.size() << " list"
         << (localShoppingLists.size() != 1 ? "s" : "") 
         << " for user: " << username << "\n";
}

string Client::getCurrentTimestamp() {
    using namespace chrono;
    auto now = system_clock::now();
    auto now_c = system_clock::to_time_t(now);

    ostringstream oss;
    oss << put_time(localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

string Client::getUsername() const {
    return username;
}


Client::~Client() {
    cout << "Client shutting down..." << endl;
    saveToLocalDatabase();
}

void Client::cli() {
    Client client;
    cout << "Enter your username: ";
    string user;
    cin >> user;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    client.loginUser(user);

    while (true) {
        cout << "\n--- Shopping List Client CLI ---\n";
        cout << "1. Synchronize with Server\n";
        cout << "2. Add Shopping List\n";
        cout << "3. Add Item to List\n";
        cout << "4. Mark Item as Acquired\n";
        cout << "5. Display Local Shopping Lists\n";
        cout << "6. Exit\n";
        cout << "Choose an option: ";

        int choice;
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        cout << endl << endl << endl << endl << endl;

        switch (choice) {
            case 1: {
                client.syncWithServer();
                break;
            }
            case 2: {
                cout << "\n================= Local Shopping Lists =================\n";
                for (const auto& [list_name, list] : client.localShoppingLists) {
                    string border(list_name.size() + 24, '=');
                    cout << border << "\n";
                    cout << "|           " << list_name << "           |\n";
                    cout << border << "\n";

                    if (list.getItems().empty()) {
                        cout << "  (No items in this list)\n";
                    } else {
                        for (const auto& [item_name, item] : list.getItems()) {
                            cout << "  * " << item.get_name() << " (Quantity: " << item.get_quantity() << ")\n";
                        }
                    }

                    cout << string(border.size(), '-') << "\n";
                }
                cout << "=========================================================\n";
                cout << "Enter the name of the shopping list: ";
                string list_name;
                getline(cin, list_name);
                if (client.localShoppingLists.count(list_name)) {
                    cout << "A shopping list with this name already exists.\n";
                } else {
                    client.addShoppingList(list_name, ShoppingList(list_name));
                }
                break;
            }
            case 3: {
                cout << "\n================= Local Shopping Lists =================\n";
                for (const auto& [list_name, list] : client.localShoppingLists) {
                    string border(list_name.size() + 24, '=');
                    cout << border << "\n";
                    cout << "|           " << list_name << "           |\n";
                    cout << border << "\n";

                    if (list.getItems().empty()) {
                        cout << "  (No items in this list)\n";
                    } else {
                        for (const auto& [item_name, item] : list.getItems()) {
                            cout << "  * " << item.get_name() << " (Quantity: " << item.get_quantity() << ")\n";
                        }
                    }

                    cout << string(border.size(), '-') << "\n";
                }
                cout << "=========================================================\n";
                cout << "Enter the name of the shopping list: ";
                string list_name;
                getline(cin, list_name);

                if (!client.localShoppingLists.count(list_name)) {
                    cout << "No shopping list found with this name.\n";
                } else {
                    cout << "Enter the name of the item: ";
                    string item_name;
                    getline(cin, item_name);

                    auto& list = client.localShoppingLists[list_name];
                    list.add_item(item_name, client.getUsername());
                    cout << "Item added to the list.\n";
                }
                break;
            }
            case 4: {
                cout << "Enter the name of the shopping list: ";
                string list_name;
                getline(cin, list_name);

                if (!client.localShoppingLists.count(list_name)) {
                    cout << "No shopping list found with this name.\n";
                } else {
                    cout << "Enter the name of the item to mark as acquired: ";
                    string item_name;
                    getline(cin, item_name);

                    if(!client.localShoppingLists[list_name].getItems().count(item_name)) {
                        cout << "No item found with this name.\n";
                        break;
                    }

                    auto& list = client.localShoppingLists[list_name];
                    // Use client.getUsername() instead of client_id
                    list.mark_item_acquired(item_name, client.getUsername());
                    client.saveToLocalDatabase();
                    cout << "Item marked as acquired.\n";
                }
                break;
            }
            case 5: {
                cout << "\n================= Local Shopping Lists =================\n";
                for (const auto& [list_name, list] : client.localShoppingLists) {
                    string border(list_name.size() + 24, '=');
                    cout << border << "\n";
                    cout << "|           " << list_name << "           |\n";
                    cout << border << "\n";

                    if (list.getItems().empty()) {
                        cout << "  (No items in this list)\n";
                    } else {
                        for (const auto& [item_name, item] : list.getItems()) {
                            cout << "  * " << item.get_name() << " (Quantity: " << item.get_quantity() << ")\n";
                        }
                    }

                    cout << string(border.size(), '-') << "\n";
                }
                cout << "=========================================================\n";
                break;
            }
            case 6: {
                cout << "Exiting...\n";
                return;
            }
            default:
                cout << "Invalid option. Please try again.\n";
        }
    }
}

void displayShoppingLists(const unordered_map<string, ShoppingList>& shoppingLists) {
    cout << "\n================= Local Shopping Lists =================\n";
    for (const auto& [list_name, list] : shoppingLists) {
        string border(list_name.size() + 24, '=');
        cout << border << "\n";
        cout << "|           " << list_name << "           |\n";
        cout << border << "\n";

        if (list.getItems().empty()) {
            cout << "  (No items in this list)\n";
        } else {
            for (const auto& [item_name, item] : list.getItems()) {
                cout << "  * " << item.get_name() << " (Quantity: " << item.get_quantity() << ")\n";
            }
        }

        cout << string(border.size(), '-') << "\n";
    }
    cout << "=========================================================\n";
}



int main() {

    Client::cli();

    return 0;
}