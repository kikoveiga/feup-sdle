#include "CCounter.h"
#include <iostream>

CCounter::CCounter(const CCounter& other) {
    lock_guard lock(other.mtx);
    state = other.state;
}

CCounter& CCounter::operator=(const CCounter& other) {
    if (this != &other) {
        lock_guard lock1(mtx);
        lock_guard lock2(other.mtx);
        state = other.state;
    }
    return *this;
}

void CCounter::increment(const string& actor) {
    lock_guard lock(mtx);
    state[actor]++;
}

void CCounter::decrement(const string& actor) {
    lock_guard lock(mtx);
    state[actor]--;
}

int CCounter::get_value() const {
    lock_guard lock(mtx);
    int total = 0;
    for (const auto&[actor, count] : state) {
        total += count;
    }
    return max(0, total);
}

void CCounter::merge(const CCounter& other) {
    lock_guard lock(mtx);
    lock_guard lock2(other.mtx);
    for (const auto& [actor, count] : other.state) {
        state[actor] += max(state[actor], count);
    }
}

json CCounter::to_json() const {
    lock_guard lock(mtx);
    return json {state};
}

CCounter CCounter::from_json(const json& j) {
    CCounter counter;
    lock_guard lock(counter.mtx);

    for (const auto& [actor, count] : j.items()) {
        counter.state[actor] = count;
    }
    return counter;

}



