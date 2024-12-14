#include "Database.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>

Database::Database() : pool{mongocxx::uri{}} {}

map<string, ShoppingList> Database::loadAllLists() const {

    const auto client = pool.acquire();

    const auto db = (*client)["server_db"];
    auto coll = db["shopping_lists"];

    map<string, ShoppingList> result;

    auto cursor = coll.find({});
    for (auto& doc : cursor) {
        auto json_str = bsoncxx::to_json(doc);
        auto j = json::parse(json_str);
       // ShoppingList list = ShoppingList::from_json(j);
       // result[list.getName()] = list;
    }

    cout << "Loaded " << result.size() << " lists from database." << endl;

    return result;
}



