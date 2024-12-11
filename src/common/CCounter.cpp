#include "CCounter.h"

template <typename T>
CCounter<T>::CCounter() : value(0) {}

template <typename T>
CCounter<T>::CCounter(std::string id) : id(std::move(id)), value(0) {}

template <typename T>
void CCounter<T>::inc(T delta) {
    value += delta;
}

template <typename T>
void CCounter<T>::dec() {
    --value;
}

template <typename T>
T CCounter<T>::read() const {
    return value;
}

template <typename T>
void CCounter<T>::join(const CCounter<T>& other) {
    value = std::max(value, other.value);
}

template <typename T>
void CCounter<T>::reset() {
    value = 0;
}

template class CCounter<int>;
