#include "ShoppingList.h"

void ShoppingList::add_item(const string& name) {
    if (items.find(name) == items.end()) {
        items[name] = ShoppingItem(name);
    }

    items[name].increment();
}

void ShoppingList::mark_item_acquired(const string& name) {
    if (items.find(name) != items.end()) {
        items[name].decrement();
    }
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