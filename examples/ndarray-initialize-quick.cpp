#include "ndarray-11.hpp"

int main() {
    using namespace pp;

    // equivalent to Ndarray<int>::dim<3> array1;
    Ndarray<int[3]> array1;

    // with initialization list
    Ndarray<int[3]> array2 = {
        {
            {0, 0, 0},
            {0, 0, 0}
        },
        {
            {1, 1, 1},
            {1, 1, 1}
        },
    };
}
