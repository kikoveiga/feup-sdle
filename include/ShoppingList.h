#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#pragma once
#include <ShoppingItem.h>
#include <unordered_map>

using namespace std;  

class ShoppingList {
    unordered_map<string, ShoppingItem> items;

public:
    void add_item(const string& name);
    void mark_item_acquired(const string& name);
    void merge(ShoppingList& other);
    void print() const;
};

#endif // SHOPPINGLIST_H
