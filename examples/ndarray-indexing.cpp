#include "ndarray.hpp"

int main() {
    pp::Ndarray<int[3]> array = {
        {
            {0, 0, 0},
            {0, 0, 0}
        },
        {
            {1, 1, 1},
            {1, 1, 1}
        },
    };

    // Accessing elements
    // Equivalent to `array[0][1][2] = 777;`
    array(0, 1, 2) = 777;
}