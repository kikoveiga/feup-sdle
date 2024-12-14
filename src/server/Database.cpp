#include "Database.h"

#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>

Database::Database() : pool{mongocxx::uri{}} {}

void Database::saveList(const ShoppingList& list) const {

    const auto client = pool.acquire();

    const auto db = (*client)["server_db"];
    auto coll = db["shopping_lists"];

    const auto doc = bsoncxx::from_json(list.to_json().dump());

    bsoncxx::builder::stream::document filter, update;
    filter << "list_id" << list.get_list_id();
    update << "$set" << doc.view();

    coll.update_one(filter.view(), update.view(), mongocxx::options::update().upsert(true));
}

ShoppingList Database::loadList(const string& list_id) const {

    const auto client = pool.acquire();

    const auto db = (*client)["server_db"];
    auto coll = db["shopping_lists"];

    bsoncxx::builder::stream::document filter;
    filter << "list_id" << list_id;

    if (const auto maybe_doc = coll.find_one(filter.view())) {
        auto json_str = bsoncxx::to_json(*maybe_doc);
        const auto j = json::parse(json_str);
        return ShoppingList::from_json(j);
    }

    return ShoppingList(list_id);
}

map<string, ShoppingList> Database::loadAllLists() const {

    const auto client = pool.acquire();

    const auto db = (*client)["server_db"];
    auto coll = db["shopping_lists"];

    map<string, ShoppingList> result;

    auto cursor = coll.find({});
    for (auto& doc : cursor) {
        auto json_str = bsoncxx::to_json(doc);
        auto j = json::parse(json_str);
        ShoppingList list = ShoppingList::from_json(j);
        result[list.get_list_id()] = list;
    }

    return result;
}

void Database::deleteList(const string &list_id) const {

    const auto client = pool.acquire();

    const auto db = (*client)["server_db"];
    auto coll = db["shopping_lists"];

    bsoncxx::builder::stream::document filter;
    filter << "list_id" << list_id;
    coll.delete_one(filter.view());
}



