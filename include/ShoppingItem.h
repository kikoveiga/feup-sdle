#ifndef SHOPPINGITEM_H
#define SHOPPINGITEM_H

#pragma once
#include "PNCounter.h"
#include <string>
#include <iostream>

using namespace std;

class ShoppingItem {
    string name;
    PNCounter quantity;

public:

    ShoppingItem();
    explicit ShoppingItem(string name);
    ShoppingItem(ShoppingItem&& other) noexcept;
    ShoppingItem& operator=(ShoppingItem&& other) noexcept;

    void setName(const string& name);
    string getName() const;

    void increment();
    void decrement();
    int getQuantity() const;

    void merge(const ShoppingItem& other);

    void print() const;
};

#endif // SHOPPINGITEM_H
