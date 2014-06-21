#include "../common/sequence_generator.h"
#include "../common/common.h"

#include <vector>
#include <algorithm>

using namespace std;

template<class T, class I=typename vector<T>::const_iterator>
T smallest(int k,
           I left_first,  I left_last,
           I right_first, I right_last)
{
    while (left_first != left_last && right_first != right_last) {
        auto left_middle = left_first + (left_last - left_first)/2;
        auto left_size = left_middle - left_first;
        auto right_middle = right_first + (right_last - right_first)/2;
        auto right_size = right_middle - right_first;

        if (*left_middle < *right_middle) {
            if (left_size + right_size >= k) {
                right_last = right_middle;
            } else {
                k -= (left_size + 1);
                left_first = left_middle + 1;
            }
        } else {
            if (left_size + right_size >= k) {
                left_last = left_middle;
            } else {
                k -= (right_size + 1);
                right_first = right_middle + 1;
            }
        }
    }

    if (left_first == left_last)
        return *(right_first + k);
    else
        return *(left_first + k);
}

int main()
{
    SequenceGenerator<float> sequence(-5.0, 5.0);
    auto left = sequence.generate(10);
    sort(begin(left), end(left));

    auto right = sequence.generate(10);
    sort(begin(right), end(right));

    print_sequence(left);
    print_sequence(right);
    cout << "K=0th smallest: " << smallest<float>(0, begin(left), end(left), begin(right), end(right)) << endl;
    cout << "K=5th smallest: " << smallest<float>(5, begin(left), end(left), begin(right), end(right)) << endl;
    cout << "K=7th smallest: " << smallest<float>(7, begin(left), end(left), begin(right), end(right)) << endl;
    cout << "K=10th smallest: " << smallest<float>(10, begin(left), end(left), begin(right), end(right)) << endl;
    cout << "K=12th smallest: " << smallest<float>(12, begin(left), end(left), begin(right), end(right)) << endl;
    cout << "K=19th smallest: " << smallest<float>(19, begin(left), end(left), begin(right), end(right)) << endl;

    return 0;
}
