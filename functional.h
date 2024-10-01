#pragma once

#include <cstddef>

namespace mystl {
// 定义一元，二元函数基类
template <class T, class U>
struct unarg_function {
    using arg_type = T;
    using res_type = U;
};

template <class T1, class T2, class U>
struct binary_function {
    using first_arg_type = T1;
    using second_arg_type = T2;
    using res_type = U;
};

// 函数对象：加法
template <class T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T &arg1, const T &arg2) const { return arg1 + arg2; }
};

// 函数对象：减法
template <class T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T &arg1, const T &arg2) const { return arg1 - arg2; }
};

// 函数对象：乘法
template <class T>
struct multiplies : public binary_function<T, T, T> {
    T operator()(const T &arg1, const T &arg2) const { return arg1 * arg2; }
};

// 函数对象：除法
template <class T>
struct divides : public binary_function<T, T, T> {
    T operator()(const T &arg1, const T &arg2) const { return arg1 / arg2; }
};

// 函数对象：模取
template <class T>
struct modulus : public binary_function<T, T, T> {
    T operator()(const T &arg1, const T &arg2) const { return arg1 % arg2; }
};

// 函数对象：取反
template <class T>
struct negate : public unarg_function<T, T> {
    T operator()(const T &arg) const { return -arg; }
};

// 函数对象：等于
template <class T>
struct equal : public binary_function<T, T, bool> {
    bool operator()(const T &arg1, const T &arg2) const { return arg1 == arg2; }
};

// 函数对象：不等于
template <class T>
struct not_equal : public binary_function<T, T, bool> {
    bool operator()(const T &arg1, const T &arg2) const { return arg1 != arg2; }
};

// 函数对象：大于
template <class T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T &arg1, const T &arg2) const { return arg1 > arg2; }
};

// 函数对象：小于
template <class T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T &arg1, const T &arg2) const { return arg1 < arg2; }
};

// 函数对象：大于等于
template <class T>
struct greater_equal : public binary_function<T, T, bool> {
    bool operator()(const T &arg1, const T &arg2) const { return arg1 >= arg2; }
};

// 函数对象：小于等于
template <class T>
struct less_equal : public binary_function<T, T, bool> {
    bool operator()(const T &arg1, const T &arg2) const { return arg1 <= arg2; }
};

// 函数对象：逻辑与
template <class T>
struct logical_and : public binary_function<T, T, bool> {
    bool operator()(const T &arg1, const T &arg2) const { return arg1 && arg2; }
};

// 函数对象：逻辑或
template <class T>
struct logical_or : public binary_function<T, T, bool> {
    bool operator()(const T &arg1, const T &arg2) const { return arg1 || arg2; }
};

// 函数对象：逻辑非
template <class T>
struct logical_not : public unarg_function<T, bool> {
    bool operator()(const T &arg1) const { return !arg1; }
};

// 证同函数，返回本身
template <class T>
struct identity : public unarg_function<T, bool> {
    T operator()(const T &arg) const { return arg; }
};

// 证同加法
template <class T>
T identity_element(plus<T>) {
    return T(0);
}

// 证同乘法
template <class T>
T identity_element(multiplies<T>) {
    return T(1);
}

// 选择函数，返回pair第一个参数
template <class Pair>
struct select_first : public unarg_function<Pair, typename Pair::first_type> {
    typename Pair::first_type operator()(const Pair &arg) const {
        return arg.first;
    }
};

// 选择函数，返回pair第二个参数
template <class Pair>
struct select_second : public unarg_function<Pair, typename Pair::second_type> {
    typename Pair::second_type operator()(const Pair &arg) const {
        return arg.second;
    }
};

// 映射函数，返回第一个参数
template <class T1, class T2>
struct project_first : public binary_function<T1, T2, T1> {
    T1 operator()(const T1 &arg1, const T2 &arg2) const { return arg1; }
};

// 映射函数，返回第二个参数
template <class T1, class T2>
struct project_second : public binary_function<T1, T2, T2> {
    T2 operator()(const T1 &arg1, const T2 &arg2) const { return arg2; }
};

// 哈希函数
template <class T>
struct hash {};

// 哈希函数，指针偏特化
template <class T>
struct hash<T *> {
    size_t operator()(T *p) const noexcept {
        return reinterpret_cast<size_t>(p);
    }
};

// 哈希函数，整型模板定义
#define MYSTL_TRIVIAL_HASH_FCN(TYPE)                 \
    template <>                                      \
    struct hash<TYPE> {                              \
        size_t operator()(TYPE arg) const noexcept { \
            return static_cast<size_t>(arg);         \
        }                                            \
    };

MYSTL_TRIVIAL_HASH_FCN(bool);
MYSTL_TRIVIAL_HASH_FCN(char);
MYSTL_TRIVIAL_HASH_FCN(signed char);
MYSTL_TRIVIAL_HASH_FCN(unsigned char);
MYSTL_TRIVIAL_HASH_FCN(short);
MYSTL_TRIVIAL_HASH_FCN(unsigned short);
MYSTL_TRIVIAL_HASH_FCN(int);
MYSTL_TRIVIAL_HASH_FCN(unsigned int);
MYSTL_TRIVIAL_HASH_FCN(long);
MYSTL_TRIVIAL_HASH_FCN(unsigned long);
MYSTL_TRIVIAL_HASH_FCN(long long);
MYSTL_TRIVIAL_HASH_FCN(unsigned long long);

#undef MYSTL_TRIVIAL_HASH_FCN

// 对于浮点数，逐位哈希
inline size_t bitwise_hash(const unsigned char *first, size_t count) {
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
    const size_t fnv_offset = 14695981039346656037ull;
    const size_t fnv_prime = 1099511628211ull;
#else
    const size_t fnv_offset = 2166136261u;
    const size_t fnv_prime = 16777619u;
#endif
    size_t result = fnv_offset;
    for (size_t i = 0; i < count; ++i) {
        result ^= static_cast<size_t>(first[i]);
        result *= fnv_prime;
    }
    return result;
}

// 哈希函数，浮点数
template <>
struct hash<float> {
    size_t operator()(const float &value) {
        return value == 0.0f
                   ? 0
                   : bitwise_hash((const unsigned char *)&value, sizeof(value));
    };
};

template <>
struct hash<double> {
    size_t operator()(const double &value) {
        return value == 0.0f
                   ? 0
                   : bitwise_hash((const unsigned char *)&value, sizeof(value));
    };
};

template <>
struct hash<long double> {
    size_t operator()(const long double &value) {
        return value == 0.0f
                   ? 0
                   : bitwise_hash((const unsigned char *)&value, sizeof(value));
    };
};
}  // namespace mystl