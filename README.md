# Ndarray-11

[![Static Badge](https://img.shields.io/badge/C%2B%2B-11-blue?style=flat-square&logo=cplusplus)](https://isocpp.org/std/the-standard)

A simple yet powerful multi-dimensional array implementation in C++11, inspired by Python's NumPy. This library provides an intuitive interface for working with multi-dimensional arrays, supporting slicing, indexing, and initialization through initializer lists.

## Features

- Multi-dimensional arrays: Supports arrays with arbitrary dimensions, enabling complex data structures.
- Initializer list support: Easily initialize arrays with nested lists.
- Indexing and slicing: Access and manipulate data through familiar Python-like syntax.
- C++11 support: Fully compatible with C++11 and upper, using modern type traits and std::initializer_list.

## Installation

To use Ndarray-11, simply download or clone the repository and include the header file in your project.

Then, include the header file in your code:
```cpp
#include "ndarray-11.hpp"
```

## Usage

- For more examples, see the [examples](./examples/) directory.
- Documentation is generated using Doxygen, and can be found [here](https://yappy2000d.github.io/PPs-Ndarray/).

### Basic Initialization

Create a 3D array with pre-defined dimensions:

```cpp
pp::Ndarray<int[3]> array = {
    {
        {0, 0, 0},
        {0, 0, 0}
    },
    {
        {1, 1, 1},
        {1, 1, 1}
    }
};
```

### Accessing Elements

Access elements with an intuitive function call syntax, similar to Python's NumPy:

```cpp
array(0, 1, 2) = 777; // Equivalent to array[0][1][2] = 777;
```

### Slicing

Perform slicing operations with a Python-like syntax using the `Range` class:

```cpp
auto sliced_array = array["0:1, ::1"];
```

### Printing Arrays

Print the array using the `<<` operator:

```cpp
std::cout << array << std::endl;
```

### Full Example

```cpp
#include "ndarray-11.hpp"

int main() {
    pp::Ndarray<int[3]> array = {
        {
            {0, 0, 0},
            {0, 0, 0}
        },
        {
            {1, 1, 1},
            {1, 1, 1}
        }
    };

    // Set value at [0][1][2]
    array(0, 1, 2) = 777;

    // Print the array
    std::cout << array << std::endl;

    return 0;
}
```

## Limitations

- Negative indices when slicing are not supported yet.
- Broadcasting is not supported yet.
- Dimensions must remain the same after slicing.
- Dimensions must be specified at compile time.

## Cooming not so soon

Check out the [Milestones](https://github.com/yappy2000d/PPs-Ndarray/milestones)

## Contributions

Contributions are welcome! If you find bugs or have suggestions for improvements, feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE.md) file for details.

## Acknowledgements

- @rexwolflan for technical guidance and code review.
