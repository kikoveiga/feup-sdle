#include "Database.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>

Database::Database() : pool{mongocxx::uri{}} {}

map<string, ShoppingList> Database::loadAllLists() const {

    map<string, ShoppingList> result;

    try {
        const auto client = pool.acquire();

        const auto db = (*client)["server_db"];
        auto coll = db["shoppingLists"];


        auto cursor = coll.find({});
        for (auto& doc : cursor) {
            auto json_str = bsoncxx::to_json(doc);
            auto j = json::parse(json_str);
            ShoppingList list = ShoppingList::from_json(j);
            result[list.getName()] = list;
        }
    } catch (const exception& e) {
        cerr << "Error loading lists from database: " << e.what() << endl;
    }

    cout << "Loaded " << result.size() << " lists from database." << endl;

    return result;
}

void Database::saveAllLists(const map<string, ShoppingList>& lists) const {
    const auto client = pool.acquire();
    const auto db = (*client)["server_db"];
    auto coll = db["shoppingLists"];

    for (const auto& [list_name, list] : lists) {
        auto j = list.to_json(); // Convert ShoppingList to JSON
        bsoncxx::document::value doc = bsoncxx::from_json(j.dump());

        // Update the document if it exists, or insert a new one
        coll.replace_one(
            bsoncxx::builder::stream::document{} << "name" << list_name << bsoncxx::builder::stream::finalize,
            move(doc),
            mongocxx::options::replace{}.upsert(true)
        );
    }

    cout << "Saved " << lists.size() << " lists to database." << endl;
}



