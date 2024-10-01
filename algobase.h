#pragma once

#include <cstring>

#include "iterator.h"
#include "util.h"

namespace mystl {

#ifdef max
#pragma message("max macro defined")
#undef max
#endif

#ifdef min
#pragma message("min macro defined")
#undef min
#endif

// max
template <class T>
const T &max(const T &arg1, const T &arg2) {
    return arg1 < arg2 ? arg2 : arg1;
}

// min
template <class T>
const T &min(const T &arg1, const T &arg2) {
    return arg1 < arg2 ? arg1 : arg2;
}

// comp version of max
template <class T, class Compare>
const T &max(const T &arg1, const T &arg2, Compare comp) {
    return comp(arg1, arg2) ? arg2 : arg1;
}

// comp version of min
template <class T, class Compare>
const T &min(const T &arg1, const T &arg2, Compare comp) {
    return comp(arg1, arg2) ? arg1 : arg2;
}

// iter_swap
template <class ForwardIterator1, class ForwardIterator2>
void iter_swap(ForwardIterator1 FIter1, ForwardIterator2 FIter2) {
    mystl::swap(*FIter1, *FIter2);
}

/**************************************************************************** */
// copy
// copy [first, last) to [result, result + (last - first))
/**************************************************************************** */
// input_iterator_tag
template <class InputIterator, class OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last,
                    OutputIterator result, input_iterator_tag) {
    for (; first != last; ++first, ++result) {
        *result = *first;
    }
    return result;
}

// random_access_iterator_tag
template <class RandomAccessIterator, class OutputIterator>
OutputIterator copy(RandomAccessIterator first, RandomAccessIterator last,
                    OutputIterator result, random_access_iterator_tag) {
    for (auto n = last - first; n > 0; --n, ++first, ++result) {
        *result = *first;
    }
    return result;
}

// copy
template <class InputIterator, class OutputIterator>
OutputIterator copy(InputIterator first, InputIterator last,
                    OutputIterator result) {
    return copy(first, last, result, iterator_category(first));
}

// copy for trivial type
template <class T1, class T2>
typename std::enable_if<
    std::is_same<typename std::remove_const<T1>::type, T2>::value &&
        std::is_trivially_copy_assignable<T2>::value,
    T2 *>::type
copy(T1 *first, T1 *last, T2 *result) {
    const auto n = static_cast<size_t>(last - first);
    if (n != 0) {
        std::memmove(result, first, n * sizeof(T2));
    }
    return result + n;
}

/**************************************************************************** */
// copy_backward
// copy [first, last) to [result - (last - first), result)
/**************************************************************************** */
// bidirectional_iterator_tag

}  // namespace mystl