#ifndef SHOPPINGITEM_H
#define SHOPPINGITEM_H

#include "CCounter.h"
#include "nlohmann/json.hpp"
#include <string>
#include <unordered_map>

using json = nlohmann::json;
using namespace std;

class ShoppingItem {
private:
    string name;
    unordered_map<string, CCounter<int>> client_counters; 

public:
    ShoppingItem();
    explicit ShoppingItem(string name);

    ShoppingItem(const ShoppingItem& other);

    ShoppingItem(ShoppingItem&& other) noexcept;

    ShoppingItem& operator=(const ShoppingItem& other) noexcept;

    ShoppingItem& operator=(ShoppingItem&& other) noexcept;

    void setName(const string& name);
    string getName() const;

    void increment(const string& client_id);
    void decrement(const string& client_id);
    int getQuantity() const;

    void merge(const ShoppingItem& other);

    void print() const;

    json to_json() const;
    static ShoppingItem from_json(const json& j);
};

#endif // SHOPPINGITEM_H
