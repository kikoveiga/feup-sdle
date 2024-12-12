#include "CCounter.h"
#include <iostream>

CCounter::CCounter() {}

CCounter::CCounter(const CCounter& other) {
    lock_guard lock(other.mtx);
    counts = other.counts;
}

CCounter::CCounter(CCounter&& other) noexcept {
    lock_guard lock(other.mtx);
    counts = move(other.counts);
}

CCounter& CCounter::operator=(const CCounter& other) {
    if (this != &other) {
        lock_guard lock1(mtx);
        lock_guard lock2(other.mtx);
        counts = other.counts;
    }
    return *this;
}

CCounter& CCounter::operator=(CCounter&& other) noexcept {
    if (this != &other) {
        lock_guard lock1(mtx);
        lock_guard lock2(other.mtx);
        counts = move(other.counts);
    }
    return *this;
}

void CCounter::increment(const string& actor) {
    lock_guard lock(mtx);
    counts[actor]++;
}

void CCounter::decrement(const string& actor) {
    lock_guard lock(mtx);
    counts[actor]--;
}

int CCounter::get_value() const {
    lock_guard lock(mtx);
    int total = 0;
    for (const auto& entry : counts) {
        total += entry.second;
    }
    return total;
}

void CCounter::merge(const CCounter& other) {
    lock_guard lock(mtx);
    for (const auto& entry : other.counts) {
        counts[entry.first] += entry.second;
    }
}

void CCounter::print() const {
    lock_guard lock(mtx);
    for (const auto& entry : counts) {
        cout << "Actor: " << entry.first << " Count: " << entry.second << endl;
    }
}
