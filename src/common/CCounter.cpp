#include "CCounter.h"
#include <iostream>

CCounter::CCounter() {}

void CCounter::increment(const std::string& actor) {
    std::lock_guard<std::mutex> lock(mtx);
    counts[actor]++;
}

void CCounter::decrement(const std::string& actor) {
    std::lock_guard<std::mutex> lock(mtx);
    counts[actor]--;
}

int CCounter::get_value() const {
    std::lock_guard<std::mutex> lock(mtx);
    int total = 0;
    for (const auto& entry : counts) {
        total += entry.second;
    }
    return total;
}

void CCounter::merge(const CCounter& other) {
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& entry : other.counts) {
        counts[entry.first] += entry.second;
    }
}

CCounter& CCounter::operator=(const CCounter& other) {
    if (this != &other) {
        std::lock_guard lock1(mtx);
        std::lock_guard lock2(other.mtx);
        counts = other.counts;
    }
    return *this;
}
