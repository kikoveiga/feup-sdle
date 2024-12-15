#include "Client.h"

#include <iostream>
#include <fstream>
#include <Message.h>

int Client::client_id_counter = 0;

Client::Client() : context(1), dealer_socket(context, zmq::socket_type::dealer) {
    client_id = "Client" + to_string(++client_id_counter);
    cout << client_id << " running..." << endl;
    dealer_socket.set(zmq::sockopt::routing_id, client_id);
    dealer_socket.connect("tcp://localhost:5555");

    loadFromLocalDatabase();
}

string Client::get_client_id() const {
    return client_id;
}

void Client::addShoppingList(const string &name, const ShoppingList &list) {
    localShoppingLists[name] = list;
}


void Client::syncWithServer() {
    send_request(Operation::SEND_ALL_LISTS, "", {});
    zmq::message_t reply;
    dealer_socket.recv(reply, zmq::recv_flags::none);

    auto response = json::parse(reply.to_string());
    for (const auto& list_json : response["lists"]) {
        auto list = ShoppingList::from_json(list_json);
        localShoppingLists[list.getName()] = list;

        saveToLocalDatabase();
    }
}

void Client::send_request(const Operation operation, const string& list_id, const json& data) {
    const Message msg(operation, list_id, data);

    string request_str = msg.to_string();
    cout << "Sending request: " << request_str << endl;

    dealer_socket.send(zmq::buffer(request_str), zmq::send_flags::none);

    zmq::message_t reply;
    if (const auto result = dealer_socket.recv(reply, zmq::recv_flags::none); !result) {
        cout << "Error receiving reply from server" << endl;
        return;
    }
    cout << "Server response: " << reply.to_string() << endl;
}

void Client::loadFromLocalDatabase() {

    ifstream infile("../clientsData/" + client_id + ".json");
    if (!infile.is_open()) {
        cout << "No local database found." << endl;
        return;
    }

    // Check if the file is empty
    infile.seekg(0, ios::end);
    if (infile.tellg() == 0) {
        cout << "Local database is empty." << endl;
        infile.close();
        return;
    }
    infile.seekg(0, ios::beg);

    json json_data;
    infile >> json_data;
    infile.close();

    for (const auto& list_json : json_data["shoppingLists"]) {
        auto list = ShoppingList::from_json(list_json);
        localShoppingLists[list.getName()] = list;
    }

    cout << "Loaded " << localShoppingLists.size() << " list" << (localShoppingLists.size() > 1 ? "s" : "") << " from local database." << endl;
}

void Client::saveToLocalDatabase() {
    using namespace std::filesystem; // Use the filesystem namespace

    // Ensure the directory exists
    const std::string directory = "../clientsData/";
    if (!exists(directory)) {
        if (!create_directory(directory)) {
            cerr << "Error creating directory: " << directory << endl;
            return;
        }
        cout << "Created directory: " << directory << endl;
    }

    // Save the lists to the JSON file
    cout << "Saving lists to local database..." << endl;
    json json_data;
    json_data["shoppingLists"] = json::array();

    for (const auto& [list_id, list] : localShoppingLists) {
        json_data["shoppingLists"].push_back(list.to_json());
    }

    ofstream outfile(directory + client_id + ".json");
    if (!outfile.is_open()) {
        cerr << "Error opening file" << endl;
        return;
    }

    outfile << json_data.dump(4);
    outfile.close();

    cout << "Saved " << localShoppingLists.size() << " lists to local database." << endl;
}


Client::~Client() {
    cout << "Client shutting down..." << endl;
    saveToLocalDatabase();
}

void Client::cli() {

    Client client;

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
        cin.ignore(); // To clear the newline character from the input buffer
        cout << endl << endl << endl << endl << endl;

        switch (choice) {
            case 1: {
                client.syncWithServer();
                cout << "Synchronized with server.\n";
                break;
            }
            case 2: {
                cout << "Enter the name of the shopping list: ";
                string list_name;
                getline(cin, list_name);

                if (client.localShoppingLists.count(list_name)) {
                    cout << "A shopping list with this name already exists.\n";
                } else {
                    ShoppingList new_list(list_name);
                    client.addShoppingList(list_name, new_list);
                    client.saveToLocalDatabase();
                    cout << "Shopping list added.\n";
                }
                break;
            }
            case 3: {
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
                    list.add_item(item_name, client.get_client_id());
                    client.saveToLocalDatabase();
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

                    auto& list = client.localShoppingLists[list_name];
                    list.mark_item_acquired(item_name, client.get_client_id());
                    client.saveToLocalDatabase();
                    cout << "Item marked as acquired.\n";
                }
                break;
            }
            case 5: {
                cout << "\nLocal Shopping Lists:\n";
                for (const auto& [list_name, list] : client.localShoppingLists) {
                    cout << "- " << list_name << "\n";
                    for (const auto&[fst, snd] : list.getItems()) {
                        cout << "  * " << snd.get_name() << " (Quantity: " << snd.get_quantity() << ")\n";
                    }
                }
                break;
            }
            case 6: {
                cout << "Exiting...\n";
                return; // Exit the function and program
            }
            default: {
                cout << "Invalid option. Please try again.\n";
                break;
            }
        }
    }

}

int main() {

    Client::cli();

    return 0;
}