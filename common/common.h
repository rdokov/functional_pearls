#ifndef COMMON_H
#define COMMON_H

#include <iostream>

template<class T>
void print_sequence(const T &s)
{
    for (auto v : s)
        std::cout << v << " ";
    std::cout << std::endl;
}

#endif
