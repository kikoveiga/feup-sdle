#ifndef SHOPPINGITEM_H
#define SHOPPINGITEM_H

#include <string>
#include <nlohmann/json.hpp>
#include "CCounter.h"

using namespace std;

class ShoppingItem {

    string name;
    CCounter counter;

public:
    ShoppingItem() = default;
    explicit ShoppingItem(string name);
    void increment(const string& actor);
    void decrement(const string& actor);
    int get_quantity() const;
    void merge(const ShoppingItem& other);
    string get_name() const;

    json to_json() const;
    static ShoppingItem from_json(const json& j);
};

#endif // SHOPPINGITEM_H
