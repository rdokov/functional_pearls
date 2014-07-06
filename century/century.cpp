#include <vector>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <iterator>

#include "../common/benchmark.h"

using namespace std;

class Expression
{
    public:
        Expression() : terms_() {}

        Expression add_digit(int digit) const {
            Expression result(*this);
            Factor &last_factor = result.terms_.back().back();
            last_factor = last_factor*10 + digit;
            return result;
        }

        Expression add_factor(int digit) const {
            Expression result(*this);
            Term &last_term = result.terms_.back();
            last_term.push_back(digit);
            return result;
        }

        Expression add_term(int digit) const {
            Expression result(*this);
            Term new_term;
            new_term.push_back(digit);
            result.terms_.push_back(new_term);
            return result;
        }

        void pretty_print() const {
            const auto & term = terms_.front();
            cout << term.front();
            for (auto factor = begin(term) + 1; factor != end(term); ++factor)
                cout << "*" << *factor;

            for (auto term = begin(terms_) + 1; term != end(terms_); ++term) {
                cout << " + ";
                cout << term->front();
                for (auto factor = begin(*term) + 1; factor != end(*term); ++factor)
                    cout << "*" << *factor;
            }

            cout << " = " << value();
            cout << endl;
        }

        int value() const {
            return accumulate(begin(terms_), end(terms_), 0,
                    [](int a, const Term &t) {
                        return a + accumulate(begin(t), end(t), 1, multiplies<int>());
                    });
        }

        bool empty() const { return terms_.size() == 0; }

    private:
        typedef int Factor;
        typedef vector<Factor> Term;

        vector<Term> terms_;
};

vector<Expression>
extend(const vector<Expression> &expressions, int digit)
{
    vector<Expression> result;

    if (expressions.size() == 0) {
        auto expr = Expression();
        result.push_back(expr.add_term(digit));
    } else {
        for (const auto expr : expressions) {
            result.push_back(expr.add_digit(digit));
            result.push_back(expr.add_factor(digit));
            result.push_back(expr.add_term(digit));
        }
    }

    return result;
}

vector<Expression>
candidates(const vector<int> &digits)
{
    return accumulate(begin(digits), end(digits), vector<Expression>(), extend);
}

vector<Expression>
solutions(const vector<int> &digits, int target_value=100)
{
    auto expressions = candidates(digits);
    auto last_good = remove_if(begin(expressions), end(expressions),
            [target_value] (const Expression &e) {
                return e.value() != target_value;
            });
    expressions.erase(last_good, end(expressions));
    return expressions;
}



vector<Expression>
generate(const Expression &expr, int digit)
{
    vector<Expression> result;

    if (expr.empty()) {
        result.push_back(expr.add_term(digit));
    } else {
        result.push_back(expr.add_digit(digit));
        result.push_back(expr.add_factor(digit));
        result.push_back(expr.add_term(digit));
    }

    return result;
}

template<class Candidate, class Generator, class PredicateGood, class PredicateOK>
void
search_helper(vector<Candidate> &output,
              vector<int>::const_iterator first_digit,
              vector<int>::const_iterator last_digit,
              const Candidate &current,
              Generator g, PredicateGood good, PredicateOK ok) {

    if (first_digit == last_digit) {
        if (good(current))
            output.push_back(current);
        return;
    }

    auto candidates = g(current, *first_digit);
    for (auto candidate : candidates) {
        if (!ok(candidate))
            continue;
        search_helper(output, first_digit + 1, last_digit, candidate, g, good, ok);
    }
}

template<class Candidate, class Generator, class PredicateGood, class PredicateOK>
vector<Candidate>
search(const vector<int> &digits, const vector<Candidate> &initial, const Generator &g, PredicateGood good, PredicateOK ok)
{
    vector<Candidate> result;
    for (auto candidate : initial) {
        search_helper(result, begin(digits), end(digits), candidate, g, good, ok);
    }

    return result;
}


vector<Expression>
fast_solutions(const vector<int> &digits, int target_value=100)
{
    vector<Expression> initial;
    initial.push_back(Expression());

    auto expressions = search(digits, initial,
            [](const Expression & e, int digit) { return generate(e, digit); },
            [target_value](const Expression &e) { return e.value() == target_value; },
            [target_value](const Expression &e) { return e.value() <= target_value; });

    return expressions;
}

int main()
{
    vector<int> digits;
    for (int i = 1; i < 10; ++i)
        digits.push_back(i);

    duration_type dt;
    auto expressions = benchmark([digits] {return solutions(digits);}, dt);
    cout << "Slow solutions: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;
    for (const auto expr : expressions) {
        expr.pretty_print();
    }

    expressions = benchmark([digits] {return fast_solutions(digits);}, dt);
    cout << "Fast solutions: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;
    for (const auto expr : expressions) {
        expr.pretty_print();
    }

    auto pi_digits = "31415926535897";
    digits.clear();
    for (auto *c = pi_digits; *c != 0; ++c)
        digits.push_back(*c - '0');

    expressions = benchmark([digits] {return solutions(digits, 1000);}, dt);
    cout << "Slow solutions: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;
    cout << expressions.size() << endl;

    expressions = benchmark([digits] {return fast_solutions(digits, 1000);}, dt);
    cout << "Fast solutions: "
         << chrono::duration_cast<chrono::milliseconds>(dt).count()
         << "ms" << endl;
    cout << expressions.size() << endl;

    return 0;
}
