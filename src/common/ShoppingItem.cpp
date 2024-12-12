#include "ShoppingItem.h"
#include <iostream>

ShoppingItem::ShoppingItem() = default;

ShoppingItem::ShoppingItem(string name) : name(move(name)) {}

ShoppingItem::ShoppingItem(const ShoppingItem& other) : name(other.name), quantity(other.quantity) {}

ShoppingItem::ShoppingItem(ShoppingItem&& other) noexcept : name(move(other.name)), quantity(move(other.quantity)) {}

ShoppingItem& ShoppingItem::operator=(const ShoppingItem& other) noexcept {
    if (this != &other) {
        name = other.name;
        quantity = other.quantity;
    }
    return *this;
}

ShoppingItem& ShoppingItem::operator=(ShoppingItem&& other) noexcept {
    if (this != &other) {
        name = move(other.name);
        quantity = move(other.quantity);
    }
    return *this;
}

void ShoppingItem::setName(const string& name) {
    this->name = name;
}

string ShoppingItem::getName() const {
    return name;
}

void ShoppingItem::increment() {
    quantity.increment("local");
}

void ShoppingItem::decrement() {
    quantity.decrement("local");
}

int ShoppingItem::getQuantity() const {
    return quantity.get_value();
}

void ShoppingItem::merge(const ShoppingItem& other) {
    if (name == other.name) {
        quantity.merge(other.quantity);
    }
}

void ShoppingItem::print() const {
    cout << "Item: " << name << ", Remaining: " << getQuantity() << endl;
}

json ShoppingItem::to_json() const {
    return json{
        {"name", name},
        {"quantity", getQuantity()}
    };
}

ShoppingItem ShoppingItem::from_json(const json& j) {
    ShoppingItem item;
    item.name = j.at("name").get<string>();
    item.quantity = CCounter();
    const int quantity = j.at("quantity").get<int>();
    for (int i = 0; i < quantity; i++) {
        item.increment();
    }

    return item;
}
