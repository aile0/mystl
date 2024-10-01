#pragma once

#include "alloc.h"
#include "construct.h"
#include "type_traits.h"
#include "iterator.h"

namespace mystl
{
    template <class T>
    class allocator
    {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = size_t;
        using difference_type = ptrdiff_t;

        static T *allocate();
        static T *allocate(size_type n);

        static void deallocate(T *p);
        static void deallocate(T *p, size_type n);

        static void construct(T *p);
        static void construct(T *p, const T &value);
        static void construct(T *p, T &&value);

        static void destroy(T *p);
        static void destroy(T *first, T *last);

        static T *address(reference x) { return &x; }
        static const T *address(const_reference x) { return &x; }
        static size_type max_size() { return size_t(-1) / sizeof(T); }
        template <typename U>
        struct rebind
        {
            using other = allocator<U>;
        };
    };

    template <typename T>
    T *allocator<T>::allocate()
    {
        return static_cast<T *>(::operator new(sizeof(T)));
    }

    template <typename T>
    T *allocator<T>::allocate(size_type n)
    {
        if (n == 0)
            return nullptr;
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    template <typename T>
    void allocator<T>::deallocate(T *p)
    {
        if (p != nullptr)
            ::operator delete(p);
    }

    template <typename T>
    void allocator<T>::deallocate(T *p, size_type n)
    {
        if (p != nullptr)
            ::operator delete(p);
    }

    template <typename T>
    void allocator<T>::construct(T *p)
    {
        mystl::construct(p);
    }

    template <typename T>
    void allocator<T>::construct(T *p, const T &value)
    {
        mystl::construct(p, value);
    }

    template <typename T>
    void allocator<T>::construct(T *p, T &&value)
    {
        mystl::construct(p, std::move(value));
    }

    template <typename T>
    void allocator<T>::destroy(T *p)
    {
        mystl::destroy(p);
    }

    template <typename T>
    void allocator<T>::destroy(T *first, T *last)
    {
        mystl::destroy(first, last);
    }
}