#include "ShoppingList.h"

#include <utility>
#include <iostream>

ShoppingList::ShoppingList(string name) : name(move(name)) {}

ShoppingList::ShoppingList(const ShoppingList& other) {
    lock_guard lock(other.mtx);
    name = other.name;
    items = other.items;
}

ShoppingList& ShoppingList::operator=(const ShoppingList& other) {
    if (this == &other) {
        return *this;
    }

    lock_guard lock(mtx);
    lock_guard lock2(other.mtx);

    name = other.name;
    items = other.items;

    return *this;
}


void ShoppingList::add_item(const string& name, const string& actor) {
    lock_guard lock(mtx);
    if (items.find(name) == items.end()) {
        items[name] = ShoppingItem(name);
    }
    items[name].increment(actor);
}


void ShoppingList::mark_item_acquired(const string& name, const string& actor) {
    lock_guard lock(mtx);
    if (items.find(name) != items.end()) {
        items[name].decrement(actor);

        if (items[name].get_quantity() == 0) {
            items.erase(name);
        }
    }
}

string ShoppingList::getName() const {
    lock_guard lock(mtx);
    return name;
}

void ShoppingList::merge(const ShoppingList& other) {
    lock_guard lock(mtx);
    lock_guard lock2(other.mtx);

    for (const auto& [name, item] : other.items) {
        if (items.find(name) == items.end()) {
            items[name] = item;
        } else {
            items[name].merge(item);
        }
    }
}

void ShoppingList::print() const {
    lock_guard lock(mtx);
    cout << "Shopping List: " << name << endl;
    for (const auto&[name, item] : items) {
        cout << "  - " << item.get_name() << " (" << item.get_quantity() << ")" << endl;
    }
}

json ShoppingList::to_json() const {
    lock_guard lock(mtx);
    json j;
    j["name"] = name;

    for (const auto& [name, item] : items) {
        j["items"][name] = item.to_json();
    }
    return j;
}

ShoppingList ShoppingList::from_json(const json& j) {
    auto list = ShoppingList(j.at("name").get<string>());

    if (j.contains("items")) {
        for (const auto& [key, value] : j.at("items").items()) {
            list.items[key] = ShoppingItem::from_json(value);
        }
    }

    return list;
}


