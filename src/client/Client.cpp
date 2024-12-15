#include "Client.h"

#include <iostream>
#include <fstream>
#include <Message.h>


Client::Client() : context(1), dealer_socket(context, zmq::socket_type::dealer) {
    std::cout << "Client initialized. Awaiting user login...\n";
    dealer_socket.connect("tcp://localhost:5555");
}

void Client::initializeUser(const string& user) {
    this->username = user;

    const string file_path = "../clientsData/" + username + ".json";
    std::ifstream infile(file_path);
    if (!infile.is_open()) {
        std::cout << "No existing data for user: " << username << ". Creating new user...\n";
        // Create a default shopping list
        ShoppingList defaultList("My Shopping List");
        localShoppingLists[defaultList.getName()] = defaultList;

        // Default lastSync is empty -> interpreted as "Never"
        lastSync.clear();
        saveToLocalDatabase();
    } else {
        std::cout << "Loading data for user: " << username << "\n";
        loadFromLocalDatabase();
    }
}

string Client::getLastSync() const {
    return lastSync.empty() ? "Never" : lastSync;
}


void Client::addShoppingList(const string& name, const ShoppingList& list) {
    if (localShoppingLists.find(name) != localShoppingLists.end()) {
        std::cout << "List \"" << name << "\" already exists.\n";
        return;
    }
    localShoppingLists[name] = list;
    std::cout << "Added shopping list: " << name << "\n";
    saveToLocalDatabase();
}

void Client::syncWithServer() {
    // Prepare data to send (all local lists)
    json data;
    data["username"] = username;
    data["shoppingLists"] = json::array();
    for (const auto& [listName, list] : localShoppingLists) {
        data["shoppingLists"].push_back(list.to_json());
    }

    // Send request to the server
    send_request(Operation::SEND_ALL_LISTS, "", data);

    // Receive and process the server's reply
    zmq::message_t reply;
    if (!dealer_socket.recv(reply, zmq::recv_flags::none)) {
        std::cerr << "Error: No reply from server during sync.\n";
        return;
    }

    auto response = json::parse(reply.to_string());
    if (response.contains("lists")) {
        for (const auto& list_json : response["lists"]) {
            auto list = ShoppingList::from_json(list_json);
            localShoppingLists[list.getName()] = list;
        }
    }

    lastSync = getCurrentTimestamp();
    saveToLocalDatabase();
    std::cout << "Synchronized with server at: " << lastSync << "\n";
}

void Client::send_request(Operation operation, const string &list_id, const json &data) {
    Message msg(operation, list_id, data);
    string request_str = msg.to_string();
    std::cout << "Sending request: " << request_str << "\n";
    dealer_socket.send(zmq::buffer(request_str), zmq::send_flags::none);
}


void Client::loadFromLocalDatabase() {
    const string file_path = "../clientsData/" + username + ".json";
    std::ifstream infile(file_path);
    if (!infile.is_open()) {
        std::cerr << "Error: No local database found for user: " << username << "\n";
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
        std::cout << "Last synchronized at: " << lastSync << "\n";
    } else {
        lastSync.clear();
    }

    std::cout << "Loaded " << localShoppingLists.size() << " list"
              << (localShoppingLists.size() != 1 ? "s" : "") 
              << " for user: " << username << "\n";
}

void Client::saveToLocalDatabase() {
    using namespace std::filesystem;

    const string directory = "../clientsData/";
    if (!exists(directory)) {
        if (!create_directory(directory)) {
            std::cerr << "Error creating directory: " << directory << "\n";
            return;
        }
    }

    json json_data;
    json_data["username"] = username;
    json_data["lastSync"] = lastSync.empty() ? "Never" : lastSync;
    json_data["shoppingLists"] = json::array();

    for (const auto& [list_name, list] : localShoppingLists) {
        json_data["shoppingLists"].push_back(list.to_json());
    }

    ofstream outfile(directory + username + ".json");
    if (!outfile.is_open()) {
        std::cerr << "Error saving data for user: " << username << "\n";
        return;
    }

    outfile << json_data.dump(4);
    outfile.close();

    std::cout << "Saved " << localShoppingLists.size() << " list"
              << (localShoppingLists.size() != 1 ? "s" : "") 
              << " for user: " << username << "\n";
}

string Client::getCurrentTimestamp() const {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto now_c = system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_c), "%Y-%m-%dT%H:%M:%S");
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
    std::cout << "Enter your username: ";
    std::string user;
    std::cin >> user;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    client.initializeUser(user);

    while (true) {
        std::cout << "\n--- Shopping List Client CLI ---\n";
        std::cout << "1. Synchronize with Server\n";
        std::cout << "2. Add Shopping List\n";
        std::cout << "3. Add Item to List\n";
        std::cout << "4. Mark Item as Acquired\n";
        std::cout << "5. Display Local Shopping Lists\n";
        std::cout << "6. Exit\n";
        std::cout << "Choose an option: ";

        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        switch (choice) {
            case 1: {
                client.syncWithServer();
                break;
            }
            case 2: {
                std::cout << "Enter the name of the shopping list: ";
                std::string list_name;
                std::getline(std::cin, list_name);
                if (client.localShoppingLists.count(list_name)) {
                    std::cout << "A shopping list with this name already exists.\n";
                } else {
                    client.addShoppingList(list_name, ShoppingList(list_name));
                }
                break;
            }
            case 3: {
                std::cout << "Enter the name of the shopping list: ";
                std::string list_name;
                std::getline(std::cin, list_name);

                if (!client.localShoppingLists.count(list_name)) {
                    std::cout << "No shopping list found with this name.\n";
                } else {
                    std::cout << "Enter the name of the item: ";
                    std::string item_name;
                    std::getline(std::cin, item_name);

                    auto& list = client.localShoppingLists[list_name];
                    // Use client.getUsername() instead of client_id
                    list.add_item(item_name, client.getUsername());
                    client.saveToLocalDatabase();
                    std::cout << "Item added to the list.\n";
                }
                break;
            }
            case 4: {
                std::cout << "Enter the name of the shopping list: ";
                std::string list_name;
                std::getline(std::cin, list_name);

                if (!client.localShoppingLists.count(list_name)) {
                    std::cout << "No shopping list found with this name.\n";
                } else {
                    std::cout << "Enter the name of the item to mark as acquired: ";
                    std::string item_name;
                    std::getline(std::cin, item_name);

                    auto& list = client.localShoppingLists[list_name];
                    // Use client.getUsername() instead of client_id
                    list.mark_item_acquired(item_name, client.getUsername());
                    client.saveToLocalDatabase();
                    std::cout << "Item marked as acquired.\n";
                }
                break;
            }
            case 5: {
                std::cout << "\nLocal Shopping Lists:\n";
                for (const auto& [lname, list] : client.localShoppingLists) {
                    std::cout << "- " << lname << "\n";
                    for (const auto& itemPair : list.getItems()) {
                        const auto& item = itemPair.second;
                        std::cout << "  * " << item.get_name()
                                  << " (Quantity: " << item.get_quantity() << ")\n";
                    }
                }
                break;
            }
            case 6: {
                std::cout << "Exiting...\n";
                return;
            }
            default:
                std::cout << "Invalid option. Please try again.\n";
        }
    }
}


int main() {

    Client::cli();

    return 0;
}