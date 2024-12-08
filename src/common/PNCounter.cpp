#include "PNCounter.h"

#include <iostream>

// Constructor to initialize counters
PNCounter::PNCounter() : positive(0), negative(0) {}

PNCounter::PNCounter(const PNCounter& other) {
    lock_guard lock(other.mtx);
    positive = other.positive;
    negative = other.negative;
}

PNCounter::PNCounter(PNCounter&& other) noexcept {
    lock_guard lock(other.mtx);
    positive = other.positive;
    negative = other.negative;
    other.positive = 0;
    other.negative = 0;
}

PNCounter& PNCounter::operator=(const PNCounter& other) {
    if (this != &other) {
        scoped_lock lock(mtx, other.mtx);
        positive = other.positive;
        negative = other.negative;
    }
    return *this;
}

PNCounter& PNCounter::operator=(PNCounter&& other) noexcept {
    if (this != &other) {
        lock_guard lock(other.mtx);
        positive = other.positive;
        negative = other.negative;
        other.positive = 0;
        other.negative = 0;
    }
    return *this;
}

void PNCounter::increment() {
    lock_guard lock(mtx);  // Thread-safe increment
    positive++;
}

void PNCounter::decrement() {
    lock_guard lock(mtx);  // Thread-safe decrement
    negative++;
}

// Get the current value of the counter (positive - negative)
int PNCounter::get_value() const {
    lock_guard lock(mtx);
    return positive - negative;
}

// Merge the state of this counter with another counter
void PNCounter::merge(const PNCounter& other) {
    if (this == &other) return;

    scoped_lock lock(mtx, other.mtx);
    positive += other.positive;
    negative += other.negative;
}

// Print the state of the counter (for debugging)
void PNCounter::print() const {
    cout << "Positive: " << positive << ", Negative: " << negative << endl;
}
