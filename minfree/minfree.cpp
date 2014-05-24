#include "../common/sequence_generator.h"
#include "../common/benchmark.h"

#include <algorithm>
#include <iostream>

using namespace std;

template<typename T>
T minfrom(T accum, typename vector<T>::iterator first,
                   typename vector<T>::iterator last)
{
    static_assert(is_unsigned<T>::value, "minfrom() is only defined for unsigned types");
    while (last - first > 0) {
        T split_value = accum + 1 + (last - first)/2;
        auto split = partition(first, last, [split_value](T x) {
                return x < split_value; });

        if (split - first == split_value - accum) {
            accum = split_value;
            first = split;
        } else {
            last = split;
        }
    }

    return accum;
}

template<typename T>
T minfree(vector<T> &v)
{
    return minfrom(static_cast<T>(1), begin(v), end(v));
}

int main()
{
    using T = unsigned int;
    const T N_small = 20;
    const T N_large = 1000000;

    duration_type dt;

    SequenceGenerator<T, true> small_seq(1, N_small + N_small/10);
    auto values = benchmark([&small_seq] { return small_seq.generate(N_small); }, dt);
    cout << "Small set generation: "
         << chrono::duration_cast<chrono::nanoseconds>(dt).count()
         << "ns" << endl;

    for (auto v : values)
        cout << v << " ";
    cout << endl;

    auto res = benchmark([&values] { return minfree(values); }, dt);
    cout << "Result: " << minfree(values) << endl;
    cout << "Small set computation: "
         << chrono::duration_cast<chrono::nanoseconds>(dt).count()
         << "ns" << endl;

    SequenceGenerator<T, true> large_seq(1, N_large + N_large/100);
    values = benchmark([&large_seq] { return large_seq.generate(N_large); }, dt);
    cout << "Large set generation: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;
    res = benchmark([&values] { return minfree(values); }, dt);
    cout << "Result: " << minfree(values) << endl;
    cout << "Large set computation: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;

    benchmark([&values] { sort(begin(values), end(values)); return 0; }, dt);
    cout << "Reference sort: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;

    return 0;
}
