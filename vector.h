#pragma once

#include "alloc.h"
#include "uninitialized.h"
#include <initializer_list>
#include <algorithm>

namespace mystl
{
    template <typename T, typename Alloc = simple_alloc<T>>
    class vector
    {
    public:
        using value_type = T;
        using allocator_type = Alloc;

        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using iterator = value_type *;
        using const_iterator = const value_type *;

        using size_type = size_t;
        using difference_type = ptrdiff_t;

        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    protected:
        iterator start;
        iterator finish;
        iterator capacity;

        void insert_aux(iterator position, const value_type &value);

        void deallocate()
        {
            if (start)
            {
                allocator_type::deallocate(start, static_cast<size_type>(capacity - start));
            }
        }

        void fill_initialize(size_type n, const value_type &value);

        template <typename InputIterator>
        void copy_initialize(InputIterator first, InputIterator last);

    public:
        // 构造函数
        vector() : start(nullptr), finish(nullptr), capacity(nullptr) {}
        vector(int n)
        {
            fill_initialize(n, value_type());
        }
        vector(size_type n)
        {
            fill_initialize(n, value_type());
        }
        vector(int n, const value_type &value)
        {
            fill_initialize(n, value);
        }
        vector(size_type n, const value_type &value)
        {
            fill_initialize(n, value);
        }

        vector(const vector<T> &v)
        {
            copy_initialize(v.begin(), v.end());
        }

        template <typename InputIterator>
        vector(InputIterator first, InputIterator last)
        {
            copy_initialize(first, last);
        }
        vector(std::initializer_list<value_type> il)
        {
            copy_initialize(il.begin(), il.end());
        }
        vector<T> &operator=(const vector<T> &v)
        {
            if (&v != this)
            {
                size_type new_size = v.size();
                if (new_size > cap())
                {
                    iterator new_start = Alloc::allocate(new_size);
                    capacity = uninitialized_copy(v.begin(), v.end(), new_start);
                    destroy(start, finish);
                    deallocate();
                    start = new_start;
                }
                else if (new_size < size())
                {
                    iterator iter = std::copy(v.begin(), v.end(), start);
                    destroy(iter, finish);
                }
                else
                {
                    std::copy(v.begin(), v.begin() + size(), start);
                    uninitialized_copy(v.begin() + size(), v.end(), finish);
                }
                finish = start + new_size;
            }
            return *this;
        }
        vector<T> &operator=(std::initializer_list<value_type> il)
        {
            vector<T> tmp(il.begin(), il.end());
            this->swap(tmp);
            return *this;
        }

        // 析构函数
        ~vector()
        {
            destroy(start, finish);
            deallocate();
        }

        // 迭代器操作
        iterator begin() { return start; }
        const_iterator begin() const { return start; }
        iterator end() { return finish; }
        const_iterator end() const { return finish; }

        iterator rbegin() { return reverse_iterator(finish); }
        const_iterator rbegin() const { return const_reverse_iterator(finish); }
        iterator rend() { return reverse_iterator(start); }
        const_iterator rend() const { return const_reverse_iterator(start); }

        const_iterator cbegin() const { return start; }
        const_iterator cend() const { return finish; }
        const_reverse_iterator crbegin() const { return reverse_iterator(finish); }
        const_reverse_iterator crend() const { return reverse_iterator(start); }

        // 容器操作
        size_type size() const { return static_cast<size_type>(finish - start); }
        size_type max_size() const { return allocator_type::max_size(); }
        bool empty() const { return start == finish; }
        size_type cap() const { return static_cast<size_t>(capacity - start); }
        // void reserve(size_type new_capacity) {

        // 访问操作
        reference at(size_type n) { return *(start + n); }
        reference front() const { return *start; }
        // const_reference front() const { return *start; }
        reference back() const { return *(finish - 1); }
        // const_reference back() const { return *(finish - 1); }
        reference operator[](size_type n) { return *(start + n); }
        // const_reference operator[](size_type n) { return *(start + n); }

        // 容器修改操作
        void push_back(const value_type &value);
        void pop_back();
        void swap(vector<T, Alloc> &rhs);
        iterator insert(iterator position, const T &value);
        void insert(iterator position, size_type n, const T &value);
        iterator erase(iterator position);
        iterator erase(iterator first, iterator last);
        void resize(size_type new_size, const T &value);
        void resize(size_type new_size);
        void clear();
    };

    template <typename T, typename Alloc>
    inline void vector<T, Alloc>::insert_aux(iterator position, const value_type &value)
    {
        if (finish != capacity)
        {
            construct(finish, *(finish - 1));
            ++finish;
            std::copy_backward(position, finish - 2, finish - 1);
            *position = value;
        }
        else
        {
            const size_type old_size = size();
            const size_type new_size = old_size == 0 ? 1 : 2 * old_size;
            iterator new_start = allocator_type::allocate(new_size);
            iterator new_finish = new_start;
            try
            {
                new_finish = uninitialized_copy(start, position, new_start);
                construct(new_finish, value);
                new_finish = uninitialized_copy(position, finish, new_finish + 1);
                destroy(start, finish);
                deallocate();
                start = new_start;
                finish = new_finish;
                capacity = new_start + new_size;
            }
            catch (const std::exception &e)
            {
                destroy(new_start, new_finish);
                allocator_type::deallocate(new_start, new_size);
                std::cerr << e.what() << '\n';
                throw;
            }
        }
    }

    template <typename T, typename Alloc>
    inline void vector<T, Alloc>::fill_initialize(size_type n, const value_type &value)
    {
        start = allocator_type::allocate(n);
        try
        {
            uninitialized_fill(start, start + n, value);
            finish = start + n;
            capacity = start + n;
        }
        catch (const std::exception &e)
        {
            allocator_type::deallocate(start, n);
            std::cerr << e.what() << '\n';
            throw;
        }
    }

    template <typename T, typename Alloc>
    template <typename InputIterator>
    inline void vector<T, Alloc>::copy_initialize(InputIterator first, InputIterator last)
    {
        start = allocator_type::allocate(last - first);
        try
        {
            finish = uninitialized_copy(first, last, start);
            capacity = finish;
        }
        catch (const std::exception &e)
        {
            allocator_type::deallocate(start, size());
            std::cerr << e.what() << '\n';
            throw;
        }
    }

    template <typename T, typename Alloc>
    void vector<T, Alloc>::push_back(const value_type &value)
    {
        if (finish != capacity)
        {
            construct(finish, value);
            ++finish;
        }
        else
        {
            insert_aux(finish, value);
        }
    }

    template <typename T, typename Alloc>
    void vector<T, Alloc>::pop_back()
    {
        --finish;
        destroy(finish);
    }

    template <typename T, typename Alloc>
    void vector<T, Alloc>::swap(vector<T, Alloc> &rhs)
    {
        std::swap(start, rhs.start);
        std::swap(finish, rhs.finish);
        std::swap(capacity, rhs.capacity);
    }

    template <typename T, typename Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::insert(iterator position, const T &value)
    {
        size_t n = position - start;
        if (finish != capacity && position == finish)
        {
            construct(finish, value);
            ++finish;
        }
        else
        {
            insert_aux(position, value);
        }
        return start + n;
    }

    template <typename T, typename Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const T &value)
    {
        if (n != 0)
        {
            size_t len = position - start;
            if (finish + n > capacity)
            {
                const size_type old_size = size();
                const size_type new_size = old_size + std::max(old_size, n);
                iterator new_start = allocator_type::allocate(new_size);
                iterator new_finish = new_start;
                try
                {
                    new_finish = uninitialized_copy(start, position, new_start);
                    new_finish = uninitialized_fill_n(new_finish, n, value);
                    new_finish = uninitialized_copy(position, finish, new_finish + n);
                    destroy(start, finish);
                    deallocate();
                    start = new_start;
                    finish = new_finish;
                    capacity = new_start + new_size;
                }
                catch (const std::exception &e)
                {
                    destroy(new_start, new_finish);
                    allocator_type::deallocate(new_start, new_size);
                    std::cerr << e.what() << '\n';
                    throw;
                }
            }
            else
            {
                iterator old_finish = finish;
                finish = finish + n;
                if (position == old_finish)
                {
                    uninitialized_fill_n(finish - n, n, value);
                }
                else
                {
                    uninitialized_copy(position, old_finish, position + n);
                    uninitialized_fill_n(position, n, value);
                }
            }
        }
    }

    template <typename T, typename Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator position)
    {
        return erase(position, position + 1);
    }

    template <typename T, typename Alloc>
    typename vector<T, Alloc>::iterator vector<T, Alloc>::erase(iterator first, iterator last)
    {
        iterator new_finish = uninitialized_copy(last, finish, first);
        destroy(new_finish, finish);
        finish = new_finish;
        return first;
    }

    template <typename T, typename Alloc>
    void vector<T, Alloc>::resize(size_type new_size, const T &value)
    {
        if (new_size < size())
        {
            erase(start + new_size, finish);
        }
        else
        {
            insert(finish, new_size - size(), value);
        }
    }

    template <typename T, typename Alloc>
    void vector<T, Alloc>::resize(size_type new_size)
    {
        resize(new_size, T());
    }

    template <typename T, typename Alloc>
    void vector<T, Alloc>::clear()
    {
        erase(start, finish);
    }

    template <typename T, typename Alloc>
    bool operator==(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
    {
        return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template <typename T, typename Alloc>
    bool operator<(const vector<T, Alloc> &lhs, const vector<T, Alloc> &rhs)
    {
        auto lhs_first = lhs.begin();
        auto lhs_last = lhs.end();
        auto rhs_first = rhs.begin();
        auto rhs_last = rhs.end();
        for (; lhs_first != lhs_last && rhs_first != rhs_last; ++lhs_first, ++rhs_first)
        {
            if (*lhs_first < *rhs_first)
            {
                return true;
            }
            else if (*rhs_first < *lhs_first)
            {
                return false;
            }
        }
        return lhs_first == lhs_last && rhs_first != rhs_last;
    }
}
