#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include <iostream>
#include <map>
#include <string>
#include <nlohmann/json.hpp>
#include "CCounter.h"

using json = nlohmann::json;

class ShoppingList {
private:
    std::string name;
    std::map<std::string, CCounter> items; // ORMap: item name -> CCounter

public:
    ShoppingList(const std::string& list_name);
    
    // Adds an item or increments the counter for an existing item
    void add_item(const std::string& item_name, const CCounter& counter);
    
    // Marks an item as acquired by decrementing its counter
    void mark_item_acquired(const std::string& item_name, const std::string& actor);
    
    // Merges another shopping list into this one
    void merge(const ShoppingList& other);
    
    // Prints the shopping list
    void print() const;

    // Serializes the shopping list to JSON
    json to_json() const;
    
    // Deserializes the shopping list from JSON
    static ShoppingList from_json(const json& json);
};

#endif // SHOPPINGLIST_H
