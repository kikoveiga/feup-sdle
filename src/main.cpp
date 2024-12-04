// main.cpp
#include <iostream>
#include "PNCounter.h"

int main() {
    // Create two PN-Counters
    PNCounter counter1;
    PNCounter counter2;

    // Simulate operations on counter1
    counter1.increment();  // counter1 = 1
    counter1.increment();  // counter1 = 2
    counter1.decrement();  // counter1 = 1

    // Simulate operations on counter2
    counter2.increment();  // counter2 = 1
    counter2.decrement();  // counter2 = 0
    counter2.decrement();  // counter2 = -1

    // Print individual counter values
    std::cout << "Counter 1 value: " << counter1.get_value() << std::endl;
    std::cout << "Counter 2 value: " << counter2.get_value() << std::endl;

    // Merge counter1 and counter2
    counter1.merge(counter2);

    // Print after merging
    std::cout << "After merging counter2 into counter1:" << std::endl;
    counter1.print();

    std::cout << "Final counter1 value: " << counter1.get_value() << std::endl;

    return 0;
}
