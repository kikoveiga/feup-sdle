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
    PNCounter(PNCounter&& other) noexcept;
    PNCounter& operator=(PNCounter&& other) noexcept;

    PNCounter& operator=(const PNCounter& other);

    void increment();
    void decrement();
    int get_value() const;
    void merge(const PNCounter& other);

    void print() const;
};

#endif // PNCOUNTER_H
