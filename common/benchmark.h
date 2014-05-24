#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <chrono>

typedef std::chrono::steady_clock::duration duration_type;

template<class F>
auto benchmark(F f, duration_type &dt) -> decltype(f())
{
    std::chrono::steady_clock clock;
    auto t1 = clock.now();
    auto res = f();
    auto t2 = clock.now();
    dt = t2 - t1;
    return res;
}


#endif
