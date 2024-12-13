#include <iostream>
#include "ShoppingList.h"

int main() {
    ShoppingList list1("list1");
    ShoppingList list2("list2");

    // Adding items by two different clients (actors)
    CCounter apples_counter;
    apples_counter.increment("ClientA");
    list1.add_item("Apples", apples_counter);

    apples_counter.increment("ClientB");
    list1.add_item("Apples", apples_counter);

    

    CCounter oranges_counter;
    oranges_counter.increment("ClientB");
    list1.add_item("Oranges", oranges_counter);

    CCounter bananas_counter;
    bananas_counter.increment("ClientA");
    list1.add_item("Bananas", bananas_counter);

    CCounter apples2_counter;
    apples2_counter.increment("ClientB");
    list2.add_item("Apples", apples2_counter);

    CCounter bananas2_counter;
    bananas2_counter.increment("ClientA");
    list2.add_item("Bananas", bananas2_counter);

    // Mark some items acquired
    list1.mark_item_acquired("Apples", "ClientA");
    list2.mark_item_acquired("Oranges", "ClientB");

    // Print the lists before merging
    std::cout << "Before Merging:" << std::endl;
    std::cout << "List1:" << std::endl;
    list1.print();
    std::cout << "List2:" << std::endl;
    list2.print();

    // Merging the two lists
    list1.merge(list2);

    // Print the merged list
    std::cout << "\nAfter Merging:" << std::endl;
    list1.print();

    // Serialize and deserialize the list
    json serialized = list1.to_json();
    std::cout << "\nSerialized List1: " << serialized.dump(4) << std::endl;

    ShoppingList deserializedList = ShoppingList::from_json(serialized);
    std::cout << "\nDeserialized List: " << std::endl;
    deserializedList.print();

    return 0;
}
