/**
 * @file
 * @author yappy2000d (yappy2000d (https://github.com/yappy2000d))
 * @brief A simple multi-dimensional array implementation in C++11
 * @version 11.3.2
 */

#ifndef __NDARRAY_HPP__
#define __NDARRAY_HPP__

#include <vector>
#include <sstream>
#include <initializer_list>
#include <type_traits>
#include <string>
#include <regex>
#include <array>

namespace pp
{

    /**
     * \addtogroup type_traits Type traits 
     * Type traits for C++11
     * @{
     */
    
    // source: https://en.cppreference.com/w/cpp/types/conjunction#Possible_implementation
    template<class...> struct conjunction : std::true_type {};  /**< C++11 std::conjunction implementation. */
    template<class B1> struct conjunction<B1> : B1 {};          /**< @copydoc conjunction */
    template<class B1, class... Bn>
    struct conjunction<B1, Bn...> 
    : std::conditional<bool(B1::value), conjunction<Bn...>, B1>::type {};  /**< @copydoc conjunction */
    
    // source: https://stackoverflow.com/a/49672613 by @max66
    template <std::size_t ...>
    struct index_sequence
    { };
    template <std::size_t N, std::size_t ... Next>
    struct indexSequenceHelper : public indexSequenceHelper<N-1U, N-1U, Next...>
    { };
    template <std::size_t ... Next>
    struct indexSequenceHelper<0U, Next ... >
    { using type = index_sequence<Next ... >; };
    template <std::size_t N>
    using make_index_sequence = typename indexSequenceHelper<N>::type;
    
    /** @} */

    /// Class for slicing index
    struct Range
    {
        int start;
        int stop;
        int step;
        bool has_stop;  ///< Flag to check if stop is given

        /**
         * Slicing constructor
         *
         * @param str
         *
         * ### Example
         * @include ndarray-slicing.cpp
         *
         */
        Range() : start(0), stop(0), step(1), has_stop(false) {}
        
        Range(const std::string &str) : Range(parseRange(str))
        {}

        Range(int start, int stop, int step, bool has_stop=true) : start(start), stop(stop), step(step), has_stop(has_stop)
        {}

        static Range parseRange(const std::string &str)
        {
            std::smatch sm;
            int start, stop, step;
            bool has_stop;

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

            return {start, stop, step, has_stop};
        }
    };


    /// Class with common methods
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
            (void)indentLevel;
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
                   << "  " << this->at(i).toString(indentLevel + 1)
                   << (i != this->size()-1 ? "," : "")
                   << "\n";
            }
            ss << indent << "]";
            return ss.str();
        }

        Dtype& at(int idx)
        {
            if (idx < 0 ) {
                idx += this->size();
                if (idx < 0) throw std::out_of_range("Index out of range");
            }

            return std::vector<Dtype>::at(idx);
        }

        const Dtype& at(int idx) const 
        {
            if (idx < 0 ) {
                idx += this->size();
                if (idx < 0) throw std::out_of_range("Index out of range");
            }

            return std::vector<Dtype>::at(idx);
        }

        friend std::ostream& operator<<(std::ostream& os, const BaseVector<Dtype, Allocator>& vec)
        {
            return os << vec.toString();
        }
    };

    /**
     * @addtogroup inner Inner
     * Implementation of Ndarray.
     * @{
     */
    
    /// Class for multi-dimensional array
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

        /**
         * @name Indexing
         *
         * Indexing for Ndarray
         * Unlike Slicing, Indexing returns a reference to the element.
         */
        
        template<typename... Indices,
                 typename std::enable_if<(sizeof...(Indices) > 0), int>::type = 0,
                 typename std::enable_if<conjunction<std::is_integral<Indices>...>::value, int>::type = 0>
        auto operator()(int idx, Indices... indices) -> decltype(this->at(idx)(indices...)) 
        {
            return this->at(idx).operator()(indices...);
        }

        template<typename... Indices,
                 typename std::enable_if<(sizeof...(Indices) > 0), int>::type = 0,
                 typename std::enable_if<conjunction<std::is_integral<Indices>...>::value, int>::type = 0>
        const Inner<Dtype, dim-1>& operator()(int idx, Indices... indices) const
        {
            return this->at(idx).operator()(indices...);
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

        /**
         * @name Slicing
         * 
         * Slicing index for Ndarray
         * Unlike Indexing, Slicing returns a new Ndarray.
         */
        
        Inner<Dtype, dim> operator[](const std::string& input) const
        {
            std::string str(input);
            str.erase(std::remove(str.begin(), str.end(), ' '), str.end());

            std::array<Range, dim> slices;
            std::size_t i = 0;

            std::regex re(",");
            std::sregex_token_iterator first{str.begin(), str.end(), re, -1}, last;
            for (; first != last; ++first) {
                slices[i++] = Range::parseRange(*first);
            }

            return sliceHelper(slices, i, make_index_sequence<dim>());
        }

        template<std::size_t... Is>
        Inner<Dtype, dim> sliceHelper(const std::array<Range, dim>& slices, std::size_t n, index_sequence<Is...>) const
        {
            (void)n;
            return sliceImpl(slices[Is]...);
        }

        template<typename... Ranges>
        typename std::enable_if<sizeof...(Ranges) == dim, Inner<Dtype, dim>>::type
        sliceImpl(const Ranges&... ranges) const
        {
            return sliceRecursive(ranges...);
        }

        template<typename... Ranges>
        Inner<Dtype, dim>
        sliceRecursive(const Range& r, const Ranges&... ranges) const
        {
            Inner<Dtype, dim> tmp;
            for(size_t i=r.start; i<(r.has_stop? r.stop: this->size()); i+=r.step)
            {
                tmp.push_back(this->at(i).sliceRecursive(ranges...));
            }
            return tmp;
        }

        Inner<Dtype, dim> sliceRecursive(const Range& r) const
        {
            Inner<Dtype, dim> tmp;
            std::size_t end = (r.has_stop? r.stop: this->size());
            for(size_t i=r.start; i<end; i+=r.step)
            {
                tmp.push_back(Inner<Dtype, dim-1>(this->at(i)));
            }
            return tmp;
        }
    };
    
    /// Class for 1-dimensional array
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
        Inner<Dtype, 1> operator[](const std::string& input) const
        {
            std::string str(input);
            str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    
            Range s = Range::parseRange(str);
            return sliceRecursive(s);
        }
    
        Inner<Dtype, 1> sliceRecursive(const Range& r) const
        {
            Inner<Dtype, 1> tmp;
            std::size_t end = (r.has_stop? r.stop: this->size());
            for(size_t i=r.start; i<end; i+=r.step)
            {
                tmp.push_back(this->at(i));
            }
            return tmp;
        }
    };
    /** @} */


    /**
     * @addtogroup ndarray Ndarray
     * Interface to create Ndarray.
     * > Recommended to use this interface to create Ndarray.
     * 
     * @tparam Dtype Data type of the array
     * @tparam dim Dimension of the array
     * 
     * ### Examples
     * #### Initialize a Ndarray.
     * @include ndarray-initialize.cpp 
     *
     * ### Initialize a Ndarray in quick way. 
     * @include ndarray-initialize-quick.cpp
     * 
     * @{
     */
    
    /**
     * An interface to create Ndarray.
     *
     * Recommended to use this interface to create Ndarray.
     * 
     * @tparam Dtype Data type of the array
     * @tparam dim Dimension of the array
     * 
     */
    template<typename Dtype>
    struct Ndarray
    {
        template<std::size_t dimention>
        using dim = Inner<Dtype, dimention>;
    };

    /**
     * An alias to quickly create Ndarray.
     *
     * Use this alias to create Ndarray quickly.
     *
     * @tparam Dtype Data type of the array
     * @tparam dim Dimension of the array
     */
    template<typename Dtype, std::size_t dim>
    struct Ndarray<Dtype[dim]> : public Inner<Dtype, dim>
    {
        using Inner<Dtype, dim>::Inner;
    };

    /** @} */
}

#endif