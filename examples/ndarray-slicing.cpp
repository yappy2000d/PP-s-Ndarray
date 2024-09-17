#include "ndarray-11.hpp"

int main() {
    pp::Ndarray<int[3]> array = {
        {
            {0, 1, 2},
            {3, 4, 5}
        },
        {
            {6, 7, 8},
            {9,10,11}
        },
    };

    // start at 1, end to 1, step 1
    pp::Slice s1("0:1");
    auto result1 = array(s1);
    // start <= i < end
    // pp::Ndarray<int[3]> result1 = {
    //     {
    //         {0, 1, 2},
    //         {3, 4, 5}
    //     }
    // };
    
    pp::Slice s2("1:");     // start at 1, to the end, step 1
    pp::Slice s3("::2");    // start at 0, to the end, step 2
    pp::Slice s4(":2");     // start at 0, end to 2, step 1
    pp::Slice s5("0:1:2");  // start at 0, end to 1, step 2
    
    // 2D Slicing
    auto result2 = array(s1, s2);
    // pp::Ndarray<int[3]> result2 = {
    //     {
    //         {3, 4, 5}
    //     }
    // };

    // 3D Slicing
    auto result3 = array(s1, s2, s3);
    // pp::Ndarray<int[3]> result3 = {
    //     {
    //         {3, 5}
    //     }
    // };
}