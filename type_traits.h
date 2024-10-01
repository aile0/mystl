#pragma once

#include <type_traits>

namespace mystl
{

    template <class T, T v>
    struct integral_constant
    {
        static constexpr T value = v;
        using value_type = T;
        using type = integral_constant;
        constexpr operator value_type() const noexcept { return value; }
        constexpr value_type operator()() const noexcept { return value; }
    };

    using true_type = integral_constant<bool, true>;
    using false_type = integral_constant<bool, false>;

    template <class T>
    struct type_traits
    {
        using has_trivial_default_constructor = false_type;
        using has_trivial_copy_constructor = false_type;
        using has_trivial_assignment = false_type;
        using has_trivial_destructor = false_type;
        using is_POD_type = false_type;
    };

    // bool,char,signed char,unsigned char,wchar_t
    // short,unsigned short,int,unsigned int,long,unsigned long,long long,unsigned long long
    // float,double,long double

#define DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(T)               \
    template <>                                            \
    struct type_traits<T>                                  \
    {                                                      \
        using has_trivial_default_constructor = true_type; \
        using has_trivial_copy_constructor = true_type;    \
        using has_trivial_assignment = true_type;          \
        using has_trivial_destructor = true_type;          \
        using is_POD_type = true_type;                     \
    };

    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(bool);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(char);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(signed char);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(unsigned char);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(wchar_t);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(short);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(unsigned short);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(int);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(unsigned int);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(long);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(unsigned long);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(long long);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(unsigned long long);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(float);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(double);
    DEFINE_TYPE_TRAITS_FOR_ARITHMETIC(long double);

#undef DEFINE_TYPE_TRAITS_FOR_ARITHMETIC

    template <class T>
    struct type_traits<T *>
    {
        using has_trivial_default_constructor = true_type;
        using has_trivial_copy_constructor = true_type;
        using has_trivial_assignment = true_type;
        using has_trivial_destructor = true_type;
        using is_POD_type = true_type;
    };

    template <class T>
    struct is_const : false_type
    {
    };

    template <class T>
    struct is_const<const T> : true_type
    {
    };

    // is_pair
    template <class T1, class T2>
    struct pair;

    template <class T>
    struct is_pair : false_type
    {
    };

    template <class T1, class T2>
    struct is_pair<mystl::pair<T1, T2>> : true_type
    {
    };
}