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

    ShoppingItem();
    explicit ShoppingItem(string name);
    ShoppingItem(ShoppingItem&& other) noexcept;

    // Move assignment operator
    ShoppingItem& operator=(ShoppingItem&& other) noexcept;
    // Regular assignment operator
    ShoppingItem& operator=(const ShoppingItem& other) noexcept;

    void setName(const string& name);
    string getName() const;

    void increment();
    void decrement();
    int getQuantity() const;

    void merge(const ShoppingItem& other);

    void print() const;

    friend void to_json(json& j, const ShoppingItem& item);
    friend void from_json(const json& j, ShoppingItem& item);
};

#endif // SHOPPINGITEM_H
