// PNCounter.cpp
#include "PNCounter.h"
#include <iostream>

// Constructor to initialize counters
PNCounter::PNCounter() : positive(0), negative(0) {}

// Increment the positive counter by 1
void PNCounter::increment() {
    std::lock_guard<std::mutex> lock(mtx);  // Thread-safe increment
    positive++;
}

// Decrement the negative counter by 1
void PNCounter::decrement() {
    std::lock_guard<std::mutex> lock(mtx);  // Thread-safe decrement
    negative++;
}

// Get the current value of the counter (positive - negative)
int PNCounter::get_value() const {
    return positive - negative;
}

// Merge the state of this counter with another counter
void PNCounter::merge(const PNCounter& other) {
    std::lock_guard<std::mutex> lock(mtx);  // Thread-safe merge
    positive += other.positive;
    negative += other.negative;
}

// Print the state of the counter (for debugging)
void PNCounter::print() const {
    std::cout << "Positive: " << positive << ", Negative: " << negative << std::endl;
}
