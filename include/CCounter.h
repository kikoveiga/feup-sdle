#ifndef CCOUNTER_H
#define CCOUNTER_H

#include <map>
#include <mutex>
#include <string>

class CCounter {
private:
    std::map<std::string, int> counts;
    mutable std::mutex mtx;

public:
    CCounter();
    CCounter& operator=(const CCounter& other);
    void increment(const std::string& actor);
    void decrement(const std::string& actor);
    int get_value() const;
    void merge(const CCounter& other);
};

#endif // CCOUNTER_H
