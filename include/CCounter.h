#ifndef CCOUNTER_H
#define CCOUNTER_H

#include <map>
#include <mutex>
#include <string>

using namespace std;

class CCounter {
private:
    map<string, int> counts;
    mutable mutex mtx;

public:
    CCounter();
    CCounter(const CCounter& other); // Copy constructor
    CCounter(CCounter&& other) noexcept; // Move constructor
    CCounter& operator=(const CCounter& other); // Copy assignment
    CCounter& operator=(CCounter&& other) noexcept; // Move assignment

    void increment(const string& actor);
    void decrement(const string& actor);
    int get_value() const;
    void merge(const CCounter& other);
    void print() const;
};

#endif // CCOUNTER_H
