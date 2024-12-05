#include "ShoppingItem.h"

ShoppingItem::ShoppingItem() = default;

ShoppingItem::ShoppingItem(string name) : name(move(name)) {}

ShoppingItem::ShoppingItem(ShoppingItem&& other) noexcept : name(move(other.name)), quantity(move(other.quantity)) {}

ShoppingItem& ShoppingItem::operator=(ShoppingItem&& other) noexcept {
    if (this != &other) {
        name = move(other.name);
        quantity = move(other.quantity);
    }
    return *this;
}

void ShoppingItem::setName(const string &name) {
    this->name = name;
}

string ShoppingItem::getName() const {
    return name;
}

void ShoppingItem::increment() {
    quantity.increment();
}

void ShoppingItem::decrement() {
    quantity.decrement();
}

int ShoppingItem::getQuantity() const {
    return quantity.get_value();
}

void ShoppingItem::merge(const ShoppingItem &other) {
    if (name == other.name) {
        quantity.merge(other.quantity);
    }
}

void ShoppingItem::print() const {
    cout << "Item: " << name << ", Remaining: " << getQuantity() << endl;
}