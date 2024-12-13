#ifndef SHOPPINGITEM_H
#define SHOPPINGITEM_H

#include <string>
#include "CCounter.h"

class ShoppingItem {
private:
    std::string name;
    CCounter counter;

public:
    ShoppingItem(const std::string& item_name);
    void increment(const std::string& actor);
    void decrement(const std::string& actor);
    int get_quantity() const;
    void merge(const ShoppingItem& other);
    std::string get_name() const;
};

#endif // SHOPPINGITEM_H
