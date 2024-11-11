#include <iostream>
#include "include/ShoppingList.h"

int main() {
    ShoppingList myList;
    myList.id = "test_id";

    myList.addItem("Arroz", 2);
    myList.addItem("Feijão", 1);
    cout << "Itens após adicionar:\n";
    for (const auto &item : myList.items) {
        cout << item.name << " - " << item.quantity << " - " << (item.acquired ? "Acquired" : "Nt Acquired") << "\n";
    }

 

    return 0;
}
