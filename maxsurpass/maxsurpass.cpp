#include "../common/benchmark.h"
#include "../common/sequence_generator.h"

#include <algorithm>
#include <iostream>

using namespace std;

typedef int var_t;

struct table_entry_t : public pair<var_t, unsigned int>
{
    table_entry_t()
    {
    }

    table_entry_t(const var_t &value, unsigned int count)
        : pair<var_t, unsigned int>(value, count)
    {
    }

    bool operator<(const table_entry_t &other)
    {
        return this->first < other.first;
    }
};

vector<table_entry_t>::iterator
merge_tables(vector<table_entry_t>::const_iterator first_left,
      vector<table_entry_t>::const_iterator last_left,
      vector<table_entry_t>::const_iterator first_right,
      vector<table_entry_t>::const_iterator last_right,
      vector<table_entry_t>::iterator result)
{
    while (true) {
        if (first_left == last_left) {
            result = copy(first_right, last_right, result);
            break;
        }
        if (first_right == last_right) {
            result = copy(first_left, last_left, result);
            break;
        }

        if (*first_left < *first_right) {
            auto n = last_right - first_right;
            *result = table_entry_t(first_left->first, first_left->second + n);
            ++result;
            ++first_left;
        } else {
            *result = *first_right;
            ++result;
            ++first_right;
        }
    }
    return result;
}

vector<table_entry_t>
table(const vector<var_t> &values)
{
    vector<table_entry_t> current_row(values.size());
    vector<table_entry_t> next_row(values.size());

    transform(values.begin(), values.end(), current_row.begin(),
            [](const var_t &v) {return table_entry_t(v, 0u);});

    for (auto scale = 1u; scale < current_row.size(); scale *= 2) {
        auto first_left = current_row.begin();
        auto output = next_row.begin();
        while (first_left < current_row.end()) {
            auto last_left = min(first_left + scale, current_row.end());
            auto first_right = last_left;
            auto last_right = min(first_right + scale, current_row.end());
            output = merge_tables(first_left, last_left, first_right, last_right, output);
            first_left = last_right;
        }
        swap(current_row, next_row);
    }
    return current_row;
}

unsigned int max_surpasser(const vector<var_t> &values)
{
    auto table_values = table(values);
    auto max = max_element(table_values.begin(), table_values.end(),
                    [](const table_entry_t &a, const table_entry_t &b) {
                        return a.second < b.second;
                    });
    return max->second;
}

int main()
{
    const unsigned int N_small = 20;
    const unsigned int N_large = 1000000;
    duration_type dt;

    SequenceGenerator<var_t> small_seq(1, N_small + N_small/10);
    auto values = small_seq.generate(N_small);

    for (auto v : values)
        cout << v << " ";
    cout << endl;

    auto res = benchmark([&values]() {return max_surpasser(values);}, dt);
    cout << "Result: " << res << endl;
    cout << "Small set computation: "
         << chrono::duration_cast<chrono::nanoseconds>(dt).count()
         << "ns" << endl;


    SequenceGenerator<var_t> large_seq(1, N_large + N_large/10);
    values = benchmark([&large_seq] {return large_seq.generate(N_large); }, dt);
    cout << "Large set generation: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;

    res = benchmark([&values]() {return max_surpasser(values);}, dt);
    cout << "Result: " << res << endl;
    cout << "Large set computation: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;

    benchmark([&values] {sort(begin(values), end(values)); return 0;}, dt);
    cout << "Reference sort: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;

    return 0;
}
