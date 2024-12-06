#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include <ShoppingItem.h>
#include <unordered_map>

using namespace std;

class ShoppingList;

void to_json(json& j, const ShoppingList& list);
void from_json(const json& j, ShoppingList& list);

class ShoppingList {
    string list_id;
    unordered_map<string, ShoppingItem> items;

public:

    ShoppingList();
    explicit ShoppingList(string list_id);

    void add_item(const string& name);
    void add_item(const ShoppingItem& item);
    void mark_item_acquired(const string& name);

    const string& get_list_id() const;
    const unordered_map<string, ShoppingItem>& get_items() const;
    void merge(ShoppingList& other);
    void print() const;
};

#endif // SHOPPINGLIST_H
