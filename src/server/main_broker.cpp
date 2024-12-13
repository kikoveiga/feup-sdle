#include "Broker.h"
#include <mongocxx/instance.hpp>

int main() {
    mongocxx::instance instance{};
    Broker broker;
    broker.run();
}