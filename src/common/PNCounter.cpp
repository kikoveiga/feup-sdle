#include "PNCounter.h"

#include <iostream>

// Constructor to initialize counters
PNCounter::PNCounter() : positive(0), negative(0) {}

PNCounter::PNCounter(PNCounter&& other) noexcept : positive(other.positive), negative(other.negative) {}

PNCounter& PNCounter::operator=(PNCounter&& other) noexcept {
    if (this != &other) {
        positive = other.positive;
        negative = other.negative;
    }
    return *this;
}

PNCounter& PNCounter::operator=(const PNCounter& other) {
    if (this != &other) {
        positive = other.positive;
        negative = other.negative;
    }
    return *this;
}

// Increment the positive counter by 1
void PNCounter::increment() {
    lock_guard lock(mtx);  // Thread-safe increment
    positive++;
}

// Decrement the negative counter by 1
void PNCounter::decrement() {
    lock_guard lock(mtx);  // Thread-safe decrement
    negative++;
}

// Get the current value of the counter (positive - negative)
int PNCounter::get_value() const {
    return positive - negative;
}

// Merge the state of this counter with another counter
void PNCounter::merge(const PNCounter& other) {
    lock_guard lock(mtx);  // Thread-safe merge
    positive += other.positive;
    negative += other.negative;
}

// Print the state of the counter (for debugging)
void PNCounter::print() const {
    cout << "Positive: " << positive << ", Negative: " << negative << endl;
}
