#pragma once

#include "construct.h"
#include "iterator.h"
#include "type_traits.h"

namespace mystl
{
    template <typename InputIterator, typename ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, mystl::true_type)
    {
        return std::copy(first, last, result);
    }

    template <typename InputIterator, typename ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result, mystl::false_type)
    {
        ForwardIterator cur = result;
        try
        {
            for (; first != last; ++first, ++cur)
            {
                mystl::construct(&*cur, *first);
            }
            return cur;
        }
        catch (const std::exception &e)
        {
            mystl::destroy(result, cur);
            std::cerr << e.what() << '\n';
            throw;
        }
    }

    template <typename InputIterator, typename ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result)
    {
        using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
        return mystl::uninitialized_copy(first, last, result, typename mystl::type_traits<value_type>::is_POD_type());
    }

    template <typename ForwardIterator, typename T>
    void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &value, mystl::true_type)
    {
        std::fill(first, last, value);
    }

    template <typename ForwardIterator, typename T>
    void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &value, mystl::false_type)
    {
        ForwardIterator cur = first;
        try
        {
            for (; cur != last; ++cur)
            {
                mystl::construct(&*cur, value);
            }
        }
        catch (const std::exception &e)
        {
            mystl::destroy(first, cur);
            std::cerr << e.what() << '\n';
            throw;
        }
    }

    template <typename ForwardIterator, typename T>
    void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T &value)
    {
        using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
        mystl::uninitialized_fill(first, last, value, typename mystl::type_traits<value_type>::is_POD_type());
    }

    template <typename ForwardIterator, typename T>
    void uninitialized_fill_n(ForwardIterator first, size_t n, const T &value, mystl::true_type)
    {
        std::fill_n(first, n, value);
    }

    template <typename ForwardIterator, typename T>
    void uninitialized_fill_n(ForwardIterator first, size_t n, const T &value, mystl::false_type)
    {
        ForwardIterator cur = first;
        try
        {
            for (; n > 0; --n, ++cur)
            {
                mystl::construct(&*cur, value);
            }
        }
        catch (const std::exception &e)
        {
            mystl::destroy(first, cur);
            std::cerr << e.what() << '\n';
            throw;
        }
    }

    template <typename ForwardIterator, typename T>
    void uninitialized_fill_n(ForwardIterator first, size_t n, const T &value)
    {
        using value_type = typename std::iterator_traits<ForwardIterator>::value_type;
        mystl::uninitialized_fill_n(first, n, value, typename mystl::type_traits<value_type>::is_POD_type());
    }
}