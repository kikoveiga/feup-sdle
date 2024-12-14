//
// Created by kikoveiga on 09-12-2024.
//

#ifndef DATABASE_H
#define DATABASE_H

#include <mongocxx/pool.hpp>
#include "ShoppingList.h"

class Database {
    mutable mongocxx::pool pool;

public:
    Database();
    void saveList(const ShoppingList& list) const;
    map<string, ShoppingList> loadAllLists() const;
    void deleteList(const string& list_id) const;

};

#endif //DATABASE_H
