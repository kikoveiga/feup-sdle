#pragma once

#include <zmq.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <map>
#include <mutex>
#include <chrono>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include "zmsg.hpp"
#include "zhelpers.hpp"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;