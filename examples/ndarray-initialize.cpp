#include "ndarray-11.hpp"

int main() {
    // Ndint is an alias for Ndarray<int>
    using NdInt = pp::Ndarray<int>;

    // `array1` is a 3D array of integers
    NdInt<int>::dim<3> array1;

    // `array2` is a 2D array of integers with 2 rows and 3 columns
    NdInt::dim<2> array2(2, 3);

    // with initialization list
    NdInt::dim<2> array3 = {
        {0, 1, 2},
        {3, 4, 5}
    };
    
    // `array4` is a 2D array of integers with 2 rows, 3 columns and filled with 4
    // The last element in the list is the value to initialize the array with.
    // Because the first three elements are used to initialize the dimensions, 
    //     the last element is the value to initialize the array with.
    // This is equivalent to:
    // ```
    // Ndarray<int>::dim<2> array4 = {
    //     {4, 4, 4},
    //     {4, 4, 4}
    // };
    // ```
    NdInt::dim<2> array4(2, 3, 4);
}
