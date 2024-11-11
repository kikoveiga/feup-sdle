#ifndef SHOPPINGLIST_H
#define SHOPPINGLIST_H

#include <string>
#include <vector>

using namespace std;  

struct ShoppingItem {
    string name;
    int quantity;
    bool acquired = false;
};

class ShoppingList {
public:
    string id; 
    vector<ShoppingItem> items;

    void addItem(const string &name, int quantity);
    void removeItem(const string &name);
    void markItemAsAcquired(const string &name);
    void updateItemQuantity(const string &name, int newQuantity);
};

#endif // SHOPPINGLIST_H
