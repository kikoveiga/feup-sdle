#include "ShoppingList.h"

ShoppingList::ShoppingList(const std::string& list_name) : name(list_name) {}

void ShoppingList::add_item(const std::string& item_name, const CCounter& counter) {
    items[item_name] = counter;
}


void ShoppingList::mark_item_acquired(const std::string& item_name, const std::string& actor) {
    if (items.find(item_name) != items.end()) {
        items[item_name].decrement(actor); // Decrement for acquired item
    }
}

void ShoppingList::merge(const ShoppingList& other) {
    for (const auto& item : other.items) {
        if (items.find(item.first) != items.end()) {
            items[item.first].merge(item.second); 
        } else {
            items[item.first] = item.second;
        }
    }
}

void ShoppingList::print() const {
    std::cout << "Shopping List: " << name << std::endl;
    for (const auto& item : items) {
        std::cout << item.first << " - Quantity: " << item.second.get_value() << std::endl;
    }
}

json ShoppingList::to_json() const {
    json j;
    for (const auto& item : items) {
        json item_json;
        item_json["name"] = item.first;
        item_json["quantity"] = item.second.get_value(); 
        j.push_back(item_json);
    }
    return j;
}

ShoppingList ShoppingList::from_json(const nlohmann::json& json) {
    ShoppingList list("Deserialized List");
    for (const auto& item : json) {
        std::string name = item["name"];
        CCounter counter;
        counter.increment(name); 
        list.add_item(name, counter);
    }
    return list;
}

