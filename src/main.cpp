#include <iostream>
#include "ShoppingList.h"
#include "ShoppingItem.h"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

int main() {
    ShoppingList list1, list2;

    list1.add_item("Apples");
    list1.add_item("Apples");
    list1.add_item("Oranges");

    list1.mark_item_acquired("Apples");

    list2.add_item("Apples");
    list2.add_item("Bananas");

    list1.merge(list2);

    list1.print();

    json list_json = list1.to_json();
    cout << "Serialized List1: " << list_json.dump(4) << endl;

    ShoppingList new_list = ShoppingList::from_json(list_json);
    cout << "Deserialized List: " << endl;
    new_list.print();

    return 0;
}