#include "CCounter.h"
#include <iostream>

CCounter::CCounter(const CCounter& other) {
    lock_guard lock(other.mtx);
    increments = other.increments;
    decrements = other.decrements;
}

CCounter& CCounter::operator=(const CCounter& other) {
    if (this != &other) {
        lock_guard lock1(mtx);
        lock_guard lock2(other.mtx);
        increments = other.increments;
        decrements = other.decrements;
    }
    return *this;
}

void CCounter::increment(const string& actor) {
    lock_guard lock(mtx);
    increments[actor]++;
}

void CCounter::decrement(const string& actor) {
    lock_guard lock(mtx);
    decrements[actor]++;
}

int CCounter::get_value() const {
    lock_guard lock(mtx);
    int total_increments = 0, total_decrements = 0;
    for (const auto&[actor, count] : increments) {
        total_increments += count;
    }

    for (const auto&[actor, count] : decrements) {
        total_decrements += count;
    }

    return max(0, total_increments - total_decrements);
}

void CCounter::merge(const CCounter& other) {

    cout << "Merging counters\n";
    lock_guard lock(mtx);
    lock_guard lock2(other.mtx);
    for (const auto& [actor, count] : other.increments) {
        if (increments.find(actor) != increments.end()) increments[actor] = max(increments[actor], count);
        else increments[actor] = count;
    }

    for (const auto& [actor, count] : other.decrements) {
        if (decrements.find(actor) != decrements.end()) decrements[actor] = max(decrements[actor], count);
        else decrements[actor] = count;
    }
}

json CCounter::to_json() const {
    lock_guard lock(mtx);
    json j;
    j["increments"] = json::object();
    j["decrements"] = json::object();

    for (const auto&[actor, count] : increments) {
        cout << "Actor: " << actor << " Count: " << count << "\n";
        j["increments"][actor] = count;
    }

    for (const auto&[actor, count] : decrements) {
        j["decrements"][actor] = count;
    }

    return j;
}

CCounter CCounter::from_json(const json& j) {
    CCounter counter;

    if (j.contains("increments") && j.at("increments").is_object()) {
        for (const auto& [actor, count] : j.at("increments").items()) {
            counter.increments[actor] = count.get<int>();
        }
    }

    if (j.contains("decrements") && j.at("decrements").is_object()) {
        for (const auto& [actor, count] : j.at("decrements").items()) {
            counter.decrements[actor] = count.get<int>();
        }
    }

    return counter;
}



