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

    // default constructiable
    template <class Other1 = T1, class Other2 = T2,
              typename = typename std::enable_if<
                  std::is_default_constructible<Other1>::value &&
                      std::is_default_constructible<Other2>::value,
                  void>::type>
    constexpr pair() : first(), second() {}

    // implicit constructiable for this type
    template <class U1 = T1, class U2 = T2,
              typename std::enable_if<
                  std::is_copy_constructible<U1>::value &&
                      std::is_copy_constructible<U2>::value &&
                      std::is_convertible<const U1 &, T1>::value &&
                      std::is_convertible<const U2 &, T2>::value,
                  int>::type = 0>
    constexpr pair(const T1 &a, const T2 &b) : first(a), second(b) {}

    // explicit constructible for this type
    template <class U1 = T1, class U2 = T2,
              typename std::enable_if<
                  std::is_copy_constructible<U1>::value &&
                      std::is_copy_constructible<U2>::value &&
                      (!std::is_convertible<const U1 &, T1>::value ||
                       !std::is_convertible<const U2 &, T2>::value),
                  int>::type = 0>
    explicit constexpr pair(const T1 &a, const T2 &b) : first(a), second(b) {}

    // 拷贝构造函数
    pair(const pair &rhs) = default;
    // 移动构造函数
    pair(pair &&rhs) = default;

    // copy pair for operator=
    pair &operator=(const pair &rhs) {
        if (this != &rhs) {
            first = rhs.first;
            second = rhs.second;
        }
        return *this;
    }

    // implicit constructiable for other type
    template <class Other1, class Other2,
              typename std::enable_if<
                  std::is_constructible<T1, Other1>::value &&
                      std::is_constructible<T2, Other2>::value &&
                      std::is_convertible<Other1 &&, T1>::value &&
                      std::is_convertible<Other2 &&, T2>::value,
                  int>::type = 0>
    constexpr pair(Other1 &&a, Other2 &&b)
        : first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}

    // explicit constructiable for other type
    template <
        class Other1, class Other2,
        typename std::enable_if<std::is_constructible<T1, Other1>::value &&
                                    std::is_constructible<T2, Other2>::value &&
                                    (!std::is_convertible<Other1, T1>::value ||
                                     !std::is_convertible<Other2, T2>::value),
                                int>::type = 0>
    explicit constexpr pair(Other1 &&a, Other2 &&b)
        : first(mystl::forward<Other1>(a)), second(mystl::forward<Other2>(b)) {}

    // implicit constructiable for other pair
    template <class Other1, class Other2,
              typename std::enable_if<
                  std::is_constructible<T1, const Other1 &>::value &&
                      std::is_constructible<T2, const Other2 &>::value &&
                      std::is_convertible<const Other1 &, T1>::value &&
                      std::is_convertible<const Other2 &, T2>::value,
                  int>::type = 0>
    constexpr pair(const pair<Other1, Other2> &other)
        : first(other.first), second(other.second) {}

    // explicit constructiable for other pair
    template <class Other1, class Other2,
              typename std::enable_if<
                  std::is_constructible<T1, const Other1 &>::value &&
                      std::is_constructible<T2, const Other2 &>::value &&
                      (!std::is_convertible<const Other1 &, T1>::value ||
                       !std::is_convertible<const Other2 &, T2>::value),
                  int>::type = 0>
    explicit constexpr pair(const pair<Other1, Other2> &other)
        : first(other.first), second(other.second) {}

    // implicit constructiable for other pair
    template <
        class Other1, class Other2,
        typename std::enable_if<std::is_constructible<T1, Other1>::value &&
                                    std::is_constructible<T2, Other2>::value &&
                                    std::is_convertible<Other1, T1>::value &&
                                    std::is_convertible<Other2, T2>::value,
                                int>::type = 0>
    constexpr pair(pair<Other1, Other2> &&other)
        : first(mystl::forward<Other1>(other.first)),
          second(mystl::forward<Other2>(other.second)) {}

    // explicit constructiable for other pair
    template <
        class Other1, class Other2,
        typename std::enable_if<std::is_constructible<T1, Other1>::value &&
                                    std::is_constructible<T2, Other2>::value &&
                                    (!std::is_convertible<Other1, T1>::value ||
                                     !std::is_convertible<Other2, T2>::value),
                                int>::type = 0>
    explicit constexpr pair(pair<Other1, Other2> &&other)
        : first(mystl::forward<Other1>(other.first)),
          second(mystl::forward<Other2>(other.second)) {}

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