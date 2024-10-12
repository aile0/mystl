#pragma once

#include <cstddef>

#include "type_traits.h"

namespace mystl {
// move
template <class T>
typename std::remove_reference<T>::type &&move(T &&arg) noexcept {
    return static_cast<typename std::remove_reference<T>::type &&>(arg);
}

// forward
template <class T>
T &&forward(typename std::remove_reference<T>::type &arg) noexcept {
    return static_cast<T &&>(arg);
}

template <class T>
T &&forward(typename std::remove_reference<T>::type &&arg) noexcept {
    static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
    return static_cast<T &&>(arg);
}

// swap
template <class T>
void swap(T &a, T &b) {
    T tmp = mystl::move(a);
    a = mystl::move(b);
    b = mystl::move(tmp);
}

template <class ForwardIterator1, class ForwardIterator2>
ForwardIterator2 swap_range(ForwardIterator1 first1, ForwardIterator1 last1,
                            ForwardIterator2 first2) {
    for (; first1 != last1; ++first1, (void)++first2)
        mystl::swap(*first1, *first2);
    return first2;
}

template <class T, std::size_t N>
void swap(T (&a)[N], T (&b)[N]) {
    mystl::swap_range(a, a + N, b);
}

// pair
template <class T1, class T2>
struct pair {
    using first_type = T1;
    using second_type = T2;

    first_type first;
    second_type second;

    // 默认构造函数
    template <class U1 = T1, class U2 = T2,
              typename = typename std::enable_if<
                  std::is_default_constructible<U1>::value &&
                      std::is_default_constructible<U2>::value,
                  void>::type>
    constexpr pair() : first(), second() {}

    // 隐式构造函数
    template <class U1 = T1, class U2 = T2,
              typename std::enable_if<
                  std::is_copy_constructible<U1>::value &&
                      std::is_copy_constructible<U2>::value &&
                      std::is_convertible<const U1 &, T1>::value &&
                      std::is_convertible<const U2 &, T2>::value,
                  int>::type = 0>
    constexpr pair(const T1 &arg1, const T2 &arg2)
        : first(arg1), second(arg2) {}

    // 显式构造函数
    template <class U1 = T1, class U2 = T2,
              typename std::enable_if<
                  std::is_copy_constructible<U1>::value &&
                      std::is_copy_constructible<U2>::value &&
                      (!std::is_convertible<const U1 &, T1>::value ||
                       !std::is_convertible<const U2 &, T2>::value),
                  int>::type = 0>
    explicit pair(const T1 &arg1, const T2 &arg2) : first(arg1), second(arg2) {}

    // 拷贝构造函数
    pair(const pair &rhs) = default;
    // 移动构造函数
    pair(pair &&rhs) = default;

    // other 1 other 2
    template <
        class U1, class U2,
        typename std::enable_if<std::is_constructible<T1, U1>::value &&
                                    std::is_constructible<T2, U2>::value &&
                                    std::is_convertible<U1 &&, T1>::value &&
                                    std::is_convertible<U2 &&, T2>::value,
                                int>::type = 0>
    constexpr pair(U1 &&arg1, U2 &&arg2)
        : first(mystl::forward<U1>(arg1)), second(mystl::forward<U2>(arg2)) {}

    // other 1 other 2
    template <
        class U1, class U2,
        typename std::enable_if<std::is_constructible<T1, U1>::value &&
                                    std::is_constructible<T2, U2>::value &&
                                    (!std::is_convertible<U1 &&, T1>::value ||
                                     !std::is_convertible<U2 &&, T2>::value),
                                int>::type = 0>
    explicit pair(U1 &&arg1, U2 &&arg2)
        : first(mystl::forward<U1>(arg1)), second(mystl::forward<U2>(arg2)) {}

    // other& pair
    template <class U1, class U2,
              typename std::enable_if<
                  std::is_constructible<T1, const U1 &>::value &&
                      std::is_constructible<T2, const U2 &>::value &&
                      std::is_convertible<const U1 &, T1>::value &&
                      std::is_convertible<const U2 &, T2>::value,
                  int>::type = 0>
    constexpr pair(const pair<U1, U2> &other)
        : first(other.first), second(other.second) {}

    // other& pair
    template <class U1, class U2,
              typename std::enable_if<
                  std::is_constructible<T1, const U1 &>::value &&
                      std::is_constructible<T2, const U2 &>::value &&
                      (!std::is_convertible<const U1 &, T1>::value ||
                       !std::is_convertible<const U2 &, T2>::value),
                  int>::type = 0>
    explicit pair(const pair<U1, U2> &other)
        : first(other.first), second(other.second) {}

    // other&& pair
    template <
        class U1, class U2,
        typename std::enable_if<std::is_constructible<T1, U1 &&>::value &&
                                    std::is_constructible<T2, U2 &&>::value &&
                                    std::is_convertible<U1 &&, T1>::value &&
                                    std::is_convertible<U2 &&, T2>::value,
                                int>::type = 0>
    constexpr pair(pair<U1, U2> &&other)
        : first(mystl::forward<U1>(other.first)),
          second(mystl::forward<U2>(other.second)) {}

    // other&& pair
    template <
        class U1, class U2,
        typename std::enable_if<std::is_constructible<T1, U1 &&>::value &&
                                    std::is_constructible<T2, U2 &&>::value &&
                                    (!std::is_convertible<U1 &&, T1>::value ||
                                     !std::is_convertible<U2 &&, T2>::value),
                                int>::type = 0>
    explicit pair(pair<U1, U2> &&other)
        : first(mystl::forward<U1>(other.first)),
          second(mystl::forward<U2>(other.second)) {}

    // copy pair for operator=
    pair &operator=(const pair &rhs) {
        if (this != &rhs) {
            first = rhs.first;
            second = rhs.second;
        }
        return *this;
    }

    // move pair for operator=
    pair &operator=(pair &&rhs) {
        if (this != &rhs) {
            first = mystl::move(rhs.first);
            second = mystl::move(rhs.second);
        }
        return *this;
    }

    // copy other pair for operator=
    template <class U1, class U2>
    pair &operator=(const pair<U1, U2> &rhs) {
        first = rhs.first;
        second = rhs.second;
        return *this;
    }

    // move other pair for operator=
    template <class U1, class U2>
    pair &operator=(pair<U1, U2> &&rhs) {
        first = mystl::forward<U1>(rhs.first);
        second = mystl::forward<U2>(rhs.second);
        return *this;
    }

    ~pair() = default;

    // swap
    void swap(pair &other) {
        if (this != &other) {
            mystl::swap(first, other.first);
            mystl::swap(second, other.second);
        }
    }
};

// 重载比较操作符
template <class T1, class T2>
bool operator==(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return lhs.first == rhs.first && lhs.second == rhs.second;
}

template <class T1, class T2>
bool operator!=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return !(lhs == rhs);
}

template <class T1, class T2>
bool operator<(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return lhs.first < rhs.first ||
           (!(rhs.first < lhs.first) && lhs.second < rhs.second);
}

template <class T1, class T2>
bool operator>(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return rhs < lhs;
}

template <class T1, class T2>
bool operator<=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return !(rhs < lhs);
}

template <class T1, class T2>
bool operator>=(const pair<T1, T2> &lhs, const pair<T1, T2> &rhs) {
    return !(lhs < rhs);
}

// 重载swap
template <class T1, class T2>
void swap(pair<T1, T2> &lhs, pair<T1, T2> &rhs) {
    lhs.swap(rhs);
}

// make_pair
template <class T1, class T2>
pair<T1, T2> make_pair(T1 &&first, T2 &&second) {
    return pair<T1, T2>(mystl::forward<T1>(first), mystl::forward<T2>(second));
}
}  // namespace mystl