#include "ShoppingItem.h"
#include <iostream>

ShoppingItem::ShoppingItem() : name("") {}

ShoppingItem::ShoppingItem(string name) : name(move(name)) {}

ShoppingItem::ShoppingItem(const ShoppingItem& other) : name(other.name), client_counters(other.client_counters) {}

ShoppingItem::ShoppingItem(ShoppingItem&& other) noexcept : name(move(other.name)), client_counters(move(other.client_counters)) {}

ShoppingItem& ShoppingItem::operator=(const ShoppingItem& other) noexcept {
    if (this != &other) {
        name = other.name;
        client_counters = other.client_counters;
    }
    return *this;
}

ShoppingItem& ShoppingItem::operator=(ShoppingItem&& other) noexcept {
    if (this != &other) {
        name = move(other.name);
        client_counters = move(other.client_counters);
    }
    return *this;
}

void ShoppingItem::setName(const string& name) {
    this->name = name;
}

string ShoppingItem::getName() const {
    return name;
}

void ShoppingItem::increment(const string& client_id) {
    if (client_counters.find(client_id) == client_counters.end()) {
        client_counters[client_id] = CCounter<int>(client_id);
    }
    client_counters[client_id].inc(1);  
}


void ShoppingItem::decrement(const string& client_id) {
    if (client_counters.find(client_id) != client_counters.end()) {
        client_counters[client_id].dec();
    }
}

int ShoppingItem::getQuantity() const {
    int total = 0;
    for (const auto& pair : client_counters) {
        total += pair.second.read();
    }
    return total;
}

void ShoppingItem::merge(const ShoppingItem& other) {
    for (const auto& [client_id, counter] : other.client_counters) {
        if (client_counters.find(client_id) == client_counters.end()) {
            client_counters[client_id] = counter;
        } else {
            client_counters[client_id].join(counter);
        }
    }
}

void ShoppingItem::print() const {
    std::cout << "Item: " << name << ", Quantity: " << getQuantity() << std::endl;
}

json ShoppingItem::to_json() const {
    json item_json;
    item_json["name"] = name;
    item_json["quantity"] = getQuantity();
    return item_json;
}

ShoppingItem ShoppingItem::from_json(const json& j) {
    ShoppingItem item(j["name"].get<string>());
    return item;
}
