#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include <map>
#include <ShoppingItem.h>
#include <string>
#include <nlohmann/json.hpp>
#include "CCounter.h"

using namespace std;

class ShoppingList {

    string name;
    map<string, ShoppingItem> items;
    mutable mutex mtx;

public:
    ShoppingList() = default;
    explicit ShoppingList(string name);
    ShoppingList(const ShoppingList& other);
    ShoppingList& operator=(const ShoppingList& other);

    void add_item(const string& name, const string& actor);
    
    void mark_item_acquired(const string& name, const string& actor);

    [[nodiscard]] string getName() const;
    
    void merge(const ShoppingList& other);
    
    void print() const;

    [[nodiscard]] json to_json() const;
    static ShoppingList from_json(const json& j);
};

#endif // SHOPPINGLIST_H
