#include "ShoppingList.h"
#include <algorithm>



void ShoppingList::addItem(const string &name, int quantity) {
    ShoppingItem item{name, quantity};
    items.push_back(item);
}

void ShoppingList::removeItem(const string &name) {
    items.erase(remove_if(items.begin(), items.end(),
               [&name](const ShoppingItem& item) { return item.name == name; }),
               items.end());
}

void ShoppingList::markItemAsAcquired(const string &name) {
    for (auto &item : items) {
        if (item.name == name) {
            item.acquired = true;
            break;
        }
    }
}

void ShoppingList::updateItemQuantity(const string &name, int newQuantity) {
    for (auto &item : items) {
        if (item.name == name) {
            item.quantity = newQuantity;
            break;
        }
    }
}
