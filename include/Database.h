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
    map<string, ShoppingList> loadAllLists() const;
    void saveAllLists(const map<string, ShoppingList>& lists) const;
};

#endif //DATABASE_H
