//
// Created by kikoveiga on 04-12-2024.
//

#ifndef PNCOUNTER_H
#define PNCOUNTER_H

#include <mutex>

using namespace std;

class PNCounter {

    int positive;
    int negative;
    mutable mutex mtx;

public:

    PNCounter();
    PNCounter(const PNCounter& other); // Copy constructor
    PNCounter(PNCounter&& other) noexcept; // Move constructor

    PNCounter& operator=(const PNCounter& other); // Copy assignment operator
    PNCounter& operator=(PNCounter&& other) noexcept; // Move assignment operator

    void increment();
    void decrement();
    int get_value() const;
    void merge(const PNCounter& other);

    void print() const;
};

#endif // PNCOUNTER_H
