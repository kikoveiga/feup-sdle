#include "ShoppingItem.h"

void ShoppingItem::setName(const string &name) {
    this->name = name;
}

void ShoppingItem::setQuantity(int quantity) {
    this->quantity = quantity;
}

void ShoppingItem::setAcquired(bool acquired) {
    this->acquired = acquired;
}

string ShoppingItem::getName() const {
    return name;
}

int ShoppingItem::getQuantity() const {
    return quantity;
}

bool ShoppingItem::isAcquired() const {
    return acquired;
}