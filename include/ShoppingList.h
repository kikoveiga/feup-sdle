#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include <ShoppingItem.h>
#include <unordered_map>
#include <string>
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

class ShoppingList {
private:
    string list_id;
    unordered_map<string, ShoppingItem> items;  

public:
    ShoppingList();
    explicit ShoppingList(string list_id);

    void add_item(const string& name, const string& client_id);
    void add_item(const ShoppingItem& item, const string& client_id);
    void remove_item(const string& name, const string& client_id);
    void mark_item_acquired(const string& name, const string& client_id);

    const string& get_list_id() const;
    const unordered_map<string, ShoppingItem>& get_items() const;

    void merge(ShoppingList& other);

    void print() const;

    json to_json() const;
    static ShoppingList from_json(const json& j);
};

#endif // SHOPPINGLIST_H
