﻿/**
 * @file
 * @author yappy2000d (yappy2000d (https://github.com/yappy2000d))
 * @brief A simple multi-dimensional array implementation in C++11
 * @version 11.2
 */


#ifndef __NDARRAY_HPP__
#define __NDARRAY_HPP__

#include <vector>
#include <sstream>
#include <initializer_list>
#include <type_traits>
#include <string>
#include <regex>

namespace pp
{
    /* Type Traits */

    /// C++11 implementation of std::conjunction which is only available in C++17
    template<class...> struct conjunction : std::true_type {};
    template<class B1> struct conjunction<B1> : B1 {};
    template<class B1, class... Bn>
    struct conjunction<B1, Bn...>
    : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};
    
    /// A class for slicing index
    struct Slice
    {
        int start;
        int stop;
        int step;
        bool has_stop;  ///< Flag to check if stop is given

        /**
         * Slicing constructor
         * 
         * use it like python slice, exapmle:
         *   - `Slice("1:2:3")` : start=1, stop=2, step=3
         *   - `Slice("1:2")` : start=1, stop=2, step=1
         *   - `Slice(":2")` : start=0, stop=2, step=1
         *   - `Slice("1:")` : start=1, stop=0, step=1
         *   - `Slice("::2")` : start=0, stop=0, step=2
         * 
         * > [!warning]
         * > Example that are not allowed:
         * > - `Slice("-1:2:3")` : can't use negative index
         * > - `Slice("2")` : must use colon
         *
         * @param str
         *
         * ### Example
         * @include ndarray-slicing.cpp
         *
         */
        Slice(const std::string &str)
        {
            std::smatch sm;
            if(std::regex_match(str, sm, std::regex("^\\s*(-?\\d+)?\\s*:\\s*(-?\\d+)?\\s*:\\s*(-?\\d+)?\\s*$")))
            {
                // for slice format like "1:2:3"
                start = (sm[1] == "")? 0: std::stoi( sm[1] );
                stop   = (sm[2] == "")? 0: std::stoi( sm[2] );
                step  = (sm[3] == "")? 1: std::stoi( sm[3] );

                has_stop = (sm[2] != "");
            }
            else if(std::regex_match(str, sm, std::regex("^\\s*(-?\\d+)?\\s*:\\s*(-?\\d+)?\\s*$")))
            {
                // for slice format like "1:2"
                start = (sm[1] == "")? 0: std::stoi( sm[1] );
                stop   = (sm[2] == "")? 0: std::stoi( sm[2] );
                step  = 1;

                has_stop = (sm[2] != "");
            }
            else
            {
                // not support slice format like "1"
                throw std::invalid_argument("Invalid slice format");
            }
        }
    };


    /// BaseVector class for common methods
    template< typename Dtype, typename Allocator = std::allocator<Dtype> >
    struct BaseVector : public std::vector<Dtype, Allocator>
    {
        using std::vector<Dtype, Allocator>::vector;
        ~BaseVector() = default;

        // toString for types that are arithmetic or std::string
        template <typename U = Dtype>
        auto toString(int indentLevel = 0) const -> 
        typename std::enable_if<std::integral_constant<bool, std::is_arithmetic<U>::value || std::is_same<U, std::string>::value>::value, std::string>::type
        {
            if (this->empty()) return "[ ]";
            std::stringstream ss;
            for (size_t i = 0; i < this->size(); ++i) {
                ss << (i==0? "[ ": "")
                   << this->at(i)
                   << (i != this->size()-1 ? ", " : " ]");
            }
            return ss.str();
        }

        // toString for types that have toString() method
        template <typename U = Dtype>
        auto toString(int indentLevel = 0) const ->
        decltype(std::declval<U>().toString())
        {
            if (this->empty()) return "[ ]";
            std::stringstream ss;
            std::string indent(indentLevel * 2, ' ');
            for (size_t i = 0; i < this->size(); ++i) {
                // Recursive call for nested vectors
                ss << (i==0? "[\n": "")
                   << indent
                   << "  " << (*this)[i].toString(indentLevel + 1)
                   << (i != this->size()-1 ? "," : "")
                   << "\n";
            }
            ss << indent << "]";
            return ss.str();
        }

        friend std::ostream& operator<<(std::ostream& os, const BaseVector<Dtype, Allocator>& vec)
        {
            return os << vec.toString();
        }
    };
    
    // primary template
    template<typename Dtype, std::size_t dim>
    struct Inner : public BaseVector<Inner<Dtype, dim - 1>>
    {
        static_assert(dim >= 1, "Dimension must be greater than zero!");

        // Construct in Recursive
        template<typename... Args>
        Inner(std::size_t n = 0, Args... args) : BaseVector<Inner<Dtype, dim - 1>>(n, Inner<Dtype, dim - 1>(args...))
        {}

        /// Constructor to handle initializer list for nested lists
        Inner(std::initializer_list<Inner<Dtype, dim - 1>> initList) : BaseVector<Inner<Dtype, dim - 1>>(initList)
        {}

        /// Copy from lower dimension
        template<typename T, std::size_t M, typename = typename std::enable_if<(M < dim)>::type>
        Inner(const Inner<T, M>& lowerDimInner)
        {
            this->push_back(Inner<T, dim - 1>(lowerDimInner));
        }

        /* Indexing */
        
        template<typename... Indices,
                 typename std::enable_if<(sizeof...(Indices) > 0), int>::type = 0,
                 typename std::enable_if<conjunction<std::is_integral<Indices>...>::value, int>::type = 0>
        Inner<Dtype, dim-1>& operator()(int idx, Indices... indices)
        {
            return this->at(idx)(indices...);
        }

        template<typename... Indices,
                 typename std::enable_if<(sizeof...(Indices) > 0), int>::type = 0,
                 typename std::enable_if<conjunction<std::is_integral<Indices>...>::value, int>::type = 0>
        const Inner<Dtype, dim-1>& operator()(int idx, Indices... indices) const
        {
            return this->at(idx)(indices...);
        }

        // For when there is only one index
        Inner<Dtype, dim-1>& operator()(int idx)
        {
            return this->at(idx);
        }

        const Inner<Dtype, dim-1>& operator()(int idx) const
        {
            return this->at(idx);
        }

        /* Slicing Index */

        template<typename... Slices,
                 typename std::enable_if<conjunction<std::is_same<Slice, typename std::decay<Slices>::type>...>::value, int>::type = 0>
        Inner<Dtype, dim> operator()(const Slice& s, const Slices... slices) const
        {
            Inner<Dtype, dim> tmp;
            for(int i=s.start; i<(s.has_stop? s.stop: this->size()); i+=s.step)
            {
                tmp.push_back(this->at(i)(slices...));
            }
            return tmp;
        }

        // For when there are no more arguments
        Inner<Dtype, dim> operator()(const Slice& s) const
        {
            Inner<Dtype, dim> tmp;
            for(int i=s.start; i<(s.has_stop? s.stop: this->size()); i+=s.step)
            {
                tmp.push_back( Inner<Dtype, dim-1>(this->at(i)) );
            }
            return tmp;
        }
    };
    
    // partial specialization where dimension is 1
    template<typename Dtype>
    struct Inner<Dtype, 1> : public BaseVector<Dtype>
    {
        Inner(std::size_t n = 0, const Dtype& val = Dtype{}) : BaseVector<Dtype>(n, val)
        {}

        Inner(std::initializer_list<Dtype> initList) : BaseVector<Dtype>(initList)
        {}

        /* Indexing */
        Dtype& operator()(int idx) {
            return this->at(idx);
        }

        const Dtype& operator()(int idx) const {
            return this->at(idx);
        }

        /* Slicing Index */
        Inner<Dtype, 1> operator()(const Slice& s) const
        {
            Inner<Dtype, 1> tmp;
            for(int i=s.start; i<(s.has_stop? s.stop: this->size()); i+=s.step)
            {
                tmp.push_back(this->at(i));
            }
            return tmp;
        }
    };

   
    
    /**
     * An interface to create an Ndarray object
     *
     * > Recommended to use this interface to create an Ndarray object
     * 
     * @tparam Dtype Data type of the array
     * @tparam dim Dimension of the array
     * 
     * ### Example
     * @include ndarray-initialize.cpp 
     * 
     */
    template<typename Dtype>
    struct Ndarray
    {
        template<std::size_t dimention>
        using dim = Inner<Dtype, dimention>;
    };

    /**
     * An alias to quickly create an Ndarray object
     *
     * > Use this alias to create an Ndarray object quickly
     *
     * @tparam Dtype Data type of the array
     * @tparam dim Dimension of the array
     *
     * ### Example
     * @include ndarray-initialize-quick.cpp
     */
    template<typename Dtype, std::size_t dim>
    struct Ndarray<Dtype[dim]> : public Inner<Dtype, dim>
    {
        using Inner<Dtype, dim>::Inner;
    };
}

#endif
