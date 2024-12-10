//
// Created by kikoveiga on 09-12-2024.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <mongocxx/client.hpp>
#include "ShoppingList.h"

class Database {
    mongocxx::uri uri;

public:
    Database();
    void saveList(const ShoppingList& list) const;
    ShoppingList loadList(const string& list_id) const;
    map<string, ShoppingList> loadAllLists() const;
    void deleteList(const string& list_id) const;

};

#endif //DATABASE_H
