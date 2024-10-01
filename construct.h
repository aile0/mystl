#pragma once

#include "iterator.h"
#include "type_traits.h"
#include <new>

namespace mystl
{
    // template<typename T1, typename T2>
    // inline void construct(T1* p, const T2& value)
    // {
    //     new (p) T1(value);
    // }

    // template<typename T1, typename T2>
    // inline void construct(T1* p, T2&& value)
    // {
    //     new (p) T1(std::forward<T2>(value));
    // }

    // template<typename T>
    // inline void construct(T* p)
    // {
    //     new (p) T();
    // }

    /**
     * @brief Construct an object of type T at location p.
     *
     * @param p The location at which to construct the object.
     * @param args The arguments to forward to T's constructor.
     *
     * @note This function is a low-level utility function and should not be
     * used directly. Instead, use container classes like vector or
     * allocator objects to manage the lifetime of objects.
     */
    template <typename T, typename... Args>
    inline void construct(T *p, Args &&...args)
    {
        new (static_cast<void *>(p)) T(std::forward<Args>(args)...);
    }

    template <typename T>
    inline void destroy(T *p)
    {
        p->~T();
    }

    template <typename ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last, mystl::true_type)
    {
    }

    template <typename ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last, mystl::false_type)
    {
        for (; first != last; ++first)
        {
            destroy(&*first);
        }
    }

    template <typename ForwardIterator>
    inline void destroy(ForwardIterator first, ForwardIterator last)
    {
        using value_type = typename iterator_traits<ForwardIterator>::value_type;
        destroy(first, last, typename type_traits<value_type>::has_trivial_destructor{});
    }
}