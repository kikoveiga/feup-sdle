#ifndef CCOUNTER_H
#define CCOUNTER_H

#include <map>
#include <mutex>
#include <string>
#include <nlohmann/json.hpp>

using namespace std;
using nlohmann::json;

class CCounter {

    map<string, int> state; // Map of actor -> count
    mutable mutex mtx;

public:
    CCounter() = default;
    CCounter(const CCounter& other); // Copy constructor
    CCounter& operator=(const CCounter& other);
    void increment(const string& actor);
    void decrement(const string& actor);
    int get_value() const;
    void merge(const CCounter& other);

    json to_json() const;
    static CCounter from_json(const json& j);
};

#endif // CCOUNTER_H
