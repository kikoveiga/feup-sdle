//
// Created by kikoveiga on 04-12-2024.
//

#ifndef PNCOUNTER_H
#define PNCOUNTER_H

#include <mutex>

class PNCounter {

    int positive;
    int negative;
    std::mutex mtx;

public:
    PNCounter();
    void increment();
    void decrement();
    int get_value() const;
    void merge(const PNCounter& other);

    void print() const;
};

#endif // PNCOUNTER_H
