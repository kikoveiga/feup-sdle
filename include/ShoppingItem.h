#ifndef SHOPPINGITEM_H
#define SHOPPINGITEM_H

#include "PNCounter.h"
#include "nlohmann/json.hpp"
#include <string>

using json = nlohmann::json;
using namespace std;

class ShoppingItem {
    string name;
    PNCounter quantity;

public:

    // Constructors
    ShoppingItem();
    explicit ShoppingItem(string name);

    // Copy constructor
    ShoppingItem(const ShoppingItem& other) = default;

    // Move constructor
    ShoppingItem(ShoppingItem&& other) noexcept;

    // Copy assignment operator
    ShoppingItem& operator=(const ShoppingItem& other) noexcept;

    // Move assignment operator
    ShoppingItem& operator=(ShoppingItem&& other) noexcept;

    void setName(const string& name);
    string getName() const;

    void increment();
    void decrement();
    int getQuantity() const;

    void merge(const ShoppingItem& other);

    void print() const;

    json to_json() const;
    static ShoppingItem from_json(const json& j);
};

#endif // SHOPPINGITEM_H
