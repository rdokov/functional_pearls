#ifndef SEQUENCE_GENERATOR_H
#define SEQUENCE_GENERATOR_H

#include <vector>
#include <random>
#include <unordered_set>

template<typename T, bool unique=false>
class SequenceGenerator
{
    public:
        SequenceGenerator(T min, T max, unsigned int seed=3738u) :
            _state(seed),
            _distribution(min, max)
        {
        }

        std::vector<T> generate(unsigned int n)
        {
            std::vector<T> result;
            std::unordered_set<T> selected;

            while (result.size() < n) {
                T tmp = _distribution(_state);
                if (unique && selected.find(tmp) != selected.end())
                    continue;
                if (unique)
                    selected.insert(tmp);
                result.push_back(tmp);
            }

            return result;
        }

    private:
        std::mt19937 _state;

        template<class U, bool is_int, bool is_float>
            struct distribution_type;
        template<class U>
            struct distribution_type<U, true, false> {
                typedef std::uniform_int_distribution<U> type;
            };
        template<class U>
            struct distribution_type<U, false, true> {
                typedef std::uniform_real_distribution<U> type;
            };
        typename distribution_type<T, std::is_integral<T>::value,
            std::is_floating_point<T>::value>::type _distribution;
};

#endif
