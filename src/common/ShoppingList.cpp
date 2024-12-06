#include "ShoppingList.h"

#include <utility>

ShoppingList::ShoppingList() = default;

ShoppingList::ShoppingList(string list_id) : list_id(std::move(list_id)) {}

void ShoppingList::add_item(const string& name) {
    if (items.find(name) == items.end()) {
        items[name] = ShoppingItem(name);
    }

    items[name].increment();
}

void ShoppingList::add_item(const ShoppingItem& item) {
    if (items.find(item.getName()) == items.end()) {
        items[item.getName()] = item;
    } else {
        items[item.getName()].increment();
    }
}

void ShoppingList::mark_item_acquired(const string& name) {
    if (items.find(name) != items.end()) {
        items[name].decrement();
    }
}

const string& ShoppingList::get_list_id() const {
    return list_id;
}

const unordered_map<string, ShoppingItem>& ShoppingList::get_items() const {
    return items;
}

void ShoppingList::merge(ShoppingList& other) {
    for (auto& [name, item] : other.items) {
        if (items.find(name) == items.end()) {
            items[name] = move(item);
        } else {
            items[name].merge(item);
        }
    }
}

void ShoppingList::print() const {
    for (const auto& [name, item] : items) {
        item.print();
    }
}

void to_json(json& j, const ShoppingList& list) {
    j = json {
        {"list_id", list.get_list_id()},
        {"items", list.get_items()}
    };
}

void from_json(const json& j, ShoppingList& list) {
    list = ShoppingList(j.at("list_id").get<string>());

    for (const auto& item_json : j.at("items")) {
        auto item = item_json.get<ShoppingItem>();
        list.add_item(item);
    }
}