#include "ShoppingItem.h"

#include <utility>

ShoppingItem::ShoppingItem(string name) : name(move(name)) {}

void ShoppingItem::increment(const string& actor) {
    counter.increment(actor);
}

void ShoppingItem::decrement(const string& actor) {
    counter.decrement(actor);
}

int ShoppingItem::get_quantity() const {
    return counter.get_value();
}

void ShoppingItem::merge(const ShoppingItem& other) {
    counter.merge(other.counter);
}

string ShoppingItem::get_name() const {
    return name;
}

json ShoppingItem::to_json() const {
    json j;
    j["name"] = name;
    j["counter"] = counter.to_json();
    return j;
}

ShoppingItem ShoppingItem::from_json(const json& j) {
    ShoppingItem item(j.at("name").get<string>());

    if (j.contains("counter") && j.at("counter").is_object()) {
        item.counter = CCounter::from_json(j.at("counter"));
    }
    return item;
}
