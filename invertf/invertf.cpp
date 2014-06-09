#include <tuple>
#include <vector>
#include <iostream>
#include <functional>
#include <cmath>
#include <gmpxx.h>
#include <cassert>

using namespace std;

typedef mpz_class Integer;
//typedef int Integer;

typedef pair<Integer, Integer> Point;

template<class F>
Integer bsearch(const F &f, const Point &range, const Integer &value)
{
    Integer left = range.first;
    Integer right = range.second;
    while (left + Integer(1) < right) {
        Integer middle = (left + right)/2;
        if (f(middle) <= value) {
            left = middle;
        } else {
            right = middle;
        }
    }
    return left;
}

Point
extend(const Point &range)
{
    return make_pair(range.first - 1, range.second + 1);
}

template<class F>
void
find_in_rectangle(F &f, const Point &x_range, const Point &y_range, const Integer &value,
                  vector<Point> &result)
{
    Integer x_size = x_range.second - x_range.first;
    Integer y_size = y_range.second - y_range.first;
    
    if (x_size <= 0 || y_size <= 0)
        return;

    if (x_size > y_size) {
        Integer y_split = (y_range.first + y_range.second)/2;
        Integer x_split = bsearch([&f, y_split](Integer x) -> Integer { return f(x, y_split); }, extend(x_range), value);
        if (f(x_split, y_split) == value) {
            result.push_back(make_pair(x_split, y_split));
            find_in_rectangle(f, make_pair(x_range.first, x_split),
                              make_pair(y_split + 1, y_range.second), value, result);
        } else {
            find_in_rectangle(f, make_pair(x_range.first, x_split + 1),
                              make_pair(y_split + 1, y_range.second), value, result);
        }
        find_in_rectangle(f, make_pair(x_split + 1, x_range.second),
                          make_pair(y_range.first, y_split), value, result);
    } else {
        Integer x_split = (x_range.first + x_range.second)/2;
        Integer y_split = bsearch([&f, x_split](Integer y) -> Integer { return f(x_split, y); }, extend(y_range), value);
        if (f(x_split, y_split) == value) {
            result.push_back(make_pair(x_split, y_split));
            find_in_rectangle(f, make_pair(x_split + 1, x_range.second),
                              make_pair(y_range.first, y_split), value, result);
        } else {
            find_in_rectangle(f, make_pair(x_split + 1, x_range.second),
                              make_pair(y_range.first, y_split + 1), value, result);
        }
        find_in_rectangle(f, make_pair(x_range.first, x_split),
                          make_pair(y_split + 1, y_range.second), value, result);

    }
}

template<class F>
vector<Point>
invertf(F &f, const Integer &value)
{
    vector<Point> result;
    Integer x_max = bsearch([&f](Integer x) -> Integer { return f(x, 0); }, make_pair(Integer(0), value + 1), value) + 1;
    Integer y_max = bsearch([&f](Integer y) -> Integer { return f(0, y); }, make_pair(Integer(0), value + 1), value) + 1;
    find_in_rectangle(f, make_pair(Integer(0), x_max), make_pair(Integer(0), y_max), value, result);

    return result;
}

template<class F>
class FunctionWrapper
{
    public:
        FunctionWrapper(const F &f) :
            _f(f),
            _invokations(0)
        {
        }

        Integer operator()(Integer x, Integer y)
        {
            if (x < 0 || y < 0)
                return -1;
            _invokations += 1;
            return _f(x, y);
        }

        int getInvokationCount() const
        {
            return _invokations;
        }

    private:
        const F &_f;
        int _invokations;
};

template<class F>
vector<Point>
brute_force(F &f, const Integer &value)
{
    vector<Point> result;

    for (Integer x = 0; x <= value; ++x)
        for (Integer y = 0; y <= value; ++y)
            if (f(x, y) == value)
                result.push_back(make_pair(x, y));

    return result;
}

mpz_class
pow(const mpz_class &base, const mpz_class &exp)
{
    mpz_class result = 1;
    mpz_class tmp = base;
    for (mpz_class current(exp); current > 0; current /= 2) {
        if (current % 2 == 1)
            result *= tmp;
        tmp *= tmp;
    }

    return result;
}

template<class F>
void
test_inversion(const F &_f, Integer N)
{
    auto f = FunctionWrapper<F>(_f);
    auto result = invertf(f, N);
    sort(begin(result), end(result));
    cout << f.getInvokationCount() << " function calls" << endl;

    auto reference = brute_force(f, N);
    sort(begin(reference), end(reference));

    assert(equal(begin(result), end(result), begin(reference)));
}

int main()
{
    Integer N = 5000;

    test_inversion([] (Integer x, Integer y) -> Integer { return pow(Integer(2),  y)*(2*x + 1) - 1; }, N);
    test_inversion([] (Integer x, Integer y) -> Integer { return x*pow(Integer(2),  x) + y*pow(Integer(2), y) + 2*x + y; }, N);
    test_inversion([] (Integer x, Integer y) -> Integer { return x*3 + y*27 + y*y; }, N);
    test_inversion([] (Integer x, Integer y) -> Integer { return x*x + y*y + x + y; }, N);
    test_inversion([] (Integer x, Integer y) -> Integer { return x + pow(Integer(2),  y) + y - 1; }, N);

    return 0;
}
