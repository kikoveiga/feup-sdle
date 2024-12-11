#ifndef CCOUNTER_H
#define CCOUNTER_H

#include <string>

template <typename T>
class CCounter {
private:
    std::string id;  
    T value;        

public:
    CCounter();  

    explicit CCounter(std::string id);

    void inc(T delta);

    void dec();

    T read() const;

    void join(const CCounter<T>& other);

    void reset();
};

#endif  // CCOUNTER_H
