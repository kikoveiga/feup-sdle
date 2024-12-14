#include <iostream>
#include <list>

#include "ShoppingList.h"

int main() {
    ShoppingList list1("list1");
    ShoppingList list2("list2");

    // Adding items by two different clients (actors)

    list1.add_item("Apples", "Client1");
    list1.add_item("Apples", "Client2");

    list1.add_item("Bananas", "Client1");

    list1.print();

    list2.add_item("Apples", "Client1");
    list2.mark_item_acquired("Apples", "Client1");

    list1.merge(list2);
    list1.print();

}
