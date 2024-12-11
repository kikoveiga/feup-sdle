#include <iostream>
#include "ShoppingList.h"
#include "ShoppingItem.h"
#include "nlohmann/json.hpp"

using namespace std;
using json = nlohmann::json;

int main() {
    ShoppingList list1,list2;

    // With the ccounter now we can know who changed the item 
    list1.add_item("Bananas", "client_1");
    list1.add_item("Apples", "client_2");
    list1.add_item("Apples", "client_1");

    list2.add_item("Apples", "client_1");
    list2.add_item("Oranges", "client_2");

    cout << "List 1 before merge:" << endl;
    list1.print();
    cout << "List 2 before mergee:" << endl;
    list2.print();

    list1.merge(list2);

    cout << "List 1 after merge" << endl;
    list1.print();


    return 0;
}
