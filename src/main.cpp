#include "ShoppingList.h"

int main() {
    ShoppingList list1, list2;

    // Add items to list1
    list1.add_item("Apples");
    list1.add_item("Apples");
    list1.add_item("Oranges");

    // Mark some items as acquired
    list1.mark_item_acquired("Apples");

    // Add items to list2
    list2.add_item("Apples");
    list2.add_item("Bananas");

    // Merge the two lists
    list1.merge(list2);

    // Print the merged list
    list1.print();

    return 0;
}