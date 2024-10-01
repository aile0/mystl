#pragma once

#include <cstddef>
#include <iterator>

namespace mystl
{

    struct input_iterator_tag
    {
    };
    struct output_iterator_tag
    {
    };
    struct forward_iterator_tag : public input_iterator_tag
    {
    };
    struct bidirectional_iterator_tag : public forward_iterator_tag
    {
    };
    struct random_access_iterator_tag : public bidirectional_iterator_tag
    {
    };

    template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T *, class Reference = T &>
    struct iterator
    {
        typedef Category iterator_category;
        typedef T value_type;
        typedef Distance difference_type;
        typedef Pointer pointer;
        typedef Reference reference;
    };

    template <class Iterator>
    struct iterator_traits
    {
        using iterator_category = typename Iterator::iterator_category;
        using value_type = typename Iterator::value_type;
        using difference_type = typename Iterator::difference_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
    };

    template <class T>
    struct iterator_traits<T *>
    {
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = T *;
        using reference = T &;
    };

    template <class T>
    struct iterator_traits<const T *>
    {
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = const T *;
        using reference = const T &;
    };

    template <typename Iterator>
    inline auto iterator_category(const Iterator &)
    {
        return typename iterator_traits<Iterator>::iterator_category{};
    }

    template <typename Iterator>
    inline auto value_type(const Iterator &)
    {
        return static_cast<typename iterator_traits<Iterator>::value_type *>(nullptr);
    }

    template <typename Iterator>
    inline auto difference_type(const Iterator &)
    {
        return static_cast<typename iterator_traits<Iterator>::difference_type *>(nullptr);
    }

    template <typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last, input_iterator_tag)
    {
        typename iterator_traits<InputIterator>::difference_type n = 0;
        while (first != last)
        {
            ++n;
            ++first;
        }
        return n;
    }

    template <typename RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type
    distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag)
    {
        return last - first;
    }

    template <typename InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last)
    {
        return distance(first, last, iterator_category(first));
    }

    template <typename InputIterator, typename Distance>
    inline void distance(InputIterator first, InputIterator last, Distance &n, input_iterator_tag)
    {
        while (first != last)
        {
            ++n;
            ++first;
        }
    }

    template <typename RandomAccessIterator, typename Distance>
    inline void distance(RandomAccessIterator first, RandomAccessIterator last, Distance &n, random_access_iterator_tag)
    {
        n += last - first;
    }

    template <typename InputIterator, typename Distance>
    inline void distance(InputIterator first, InputIterator last, Distance &n)
    {
        distance(first, last, n, iterator_category(first));
    }

    template <typename InputIterator, typename Distance>
    inline void advance(InputIterator &it, Distance n, input_iterator_tag)
    {
        while (n--)
        {
            ++it;
        }
    }

    template <typename BidirectionalIterator, typename Distance>
    inline void advance(BidirectionalIterator &it, Distance n, bidirectional_iterator_tag)
    {
        if (n >= 0)
        {
            while (n--)
            {
                ++it;
            }
        }
        else
        {
            while (n++)
            {
                --it;
            }
        }
    }

    template <typename InputIterator, typename Distance>
    inline void advance(InputIterator &it, Distance n)
    {
        advance(it, n, iterator_category(it));
    }

    // reverse_iterator
    template <typename RandomAccessIterator>
    class reverse_iterator
    {
    protected:
        RandomAccessIterator current;

    public:
        using iterator_type = RandomAccessIterator;
        using value_type = typename iterator_traits<RandomAccessIterator>::value_type;
        using difference_type = typename iterator_traits<RandomAccessIterator>::difference_type;
        using pointer = typename iterator_traits<RandomAccessIterator>::pointer;
        using reference = typename iterator_traits<RandomAccessIterator>::reference;

        reverse_iterator() {}
        explicit reverse_iterator(RandomAccessIterator it) : current(it) {}

        RandomAccessIterator base() const { return current; }

        reference operator*() const
        {
            return *(current - 1);
        }

        pointer operator->() const
        {
            return &(operator*());
        }

        reverse_iterator<RandomAccessIterator> &operator++()
        {
            --current;
            return *this;
        }
        reverse_iterator<RandomAccessIterator> &operator--()
        {
            ++current;
            return *this;
        }

        reverse_iterator<RandomAccessIterator> operator++(int)
        {
            RandomAccessIterator tmp = *this;
            --current;
            return tmp;
        }
        reverse_iterator<RandomAccessIterator> operator--(int)
        {
            RandomAccessIterator tmp = *this;
            ++current;
            return tmp;
        }

        reverse_iterator<RandomAccessIterator> operator+(difference_type n) const
        {
            return reverse_iterator(current - n);
        }
        reverse_iterator<RandomAccessIterator> operator-(difference_type n) const
        {
            return reverse_iterator(current + n);
        }

        reverse_iterator<RandomAccessIterator> &operator+=(difference_type n)
        {
            current -= n;
            return *this;
        }
        reverse_iterator<RandomAccessIterator> &operator-=(difference_type n)
        {
            current += n;
            return *this;
        }

        reference operator[](difference_type n) const
        {
            return *(*this + n);
        }

        friend bool operator==(const reverse_iterator<RandomAccessIterator> &lhs, const reverse_iterator<RandomAccessIterator> &rhs)
        {
            return lhs.current == rhs.current;
        }
        friend bool operator!=(const reverse_iterator<RandomAccessIterator> &lhs, const reverse_iterator<RandomAccessIterator> &rhs)
        {
            return !(lhs == rhs);
        }

        friend bool operator<(const reverse_iterator<RandomAccessIterator> &lhs, const reverse_iterator<RandomAccessIterator> &rhs)
        {
            return rhs.current < lhs.current;
        }
        friend bool operator<=(const reverse_iterator<RandomAccessIterator> &lhs, const reverse_iterator<RandomAccessIterator> &rhs)
        {
            return !(rhs.current < lhs.current);
        }

        friend bool operator>(const reverse_iterator<RandomAccessIterator> &lhs, const reverse_iterator<RandomAccessIterator> &rhs)
        {
            return rhs.current > lhs.current;
        }
        friend bool operator>=(const reverse_iterator<RandomAccessIterator> &lhs, const reverse_iterator<RandomAccessIterator> &rhs)
        {
            return !(rhs.current > lhs.current);
        }

        friend difference_type operator-(const reverse_iterator<RandomAccessIterator> &lhs, const reverse_iterator<RandomAccessIterator> &rhs)
        {
            return rhs.current - lhs.current;
        }
        friend reverse_iterator<RandomAccessIterator> operator+(reverse_iterator<RandomAccessIterator> n, const reverse_iterator<RandomAccessIterator> &it)
        {
            return reverse_iterator<RandomAccessIterator>(it.current - n);
        }
    };

}