#include "ShoppingItem.h"

ShoppingItem::ShoppingItem(const std::string& item_name) : name(item_name) {}

void ShoppingItem::increment(const std::string& actor) {
    counter.increment(actor);
}

void ShoppingItem::decrement(const std::string& actor) {
    counter.decrement(actor);
}

int ShoppingItem::get_quantity() const {
    return counter.get_value();
}

void ShoppingItem::merge(const ShoppingItem& other) {
    counter.merge(other.counter);
}

std::string ShoppingItem::get_name() const {
    return name;
}
