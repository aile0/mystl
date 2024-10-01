#pragma once

#include <initializer_list>

#include "alloc.h"
#include "construct.h"
#include "iterator.h"

namespace mystl {
// 计算deque的缓冲区大小，默认每个缓冲区为1024/sizeof(T)或者4个
inline size_t deque_buf_size(size_t n, size_t sz) {
    return n != 0 ? n : (sz < 256 ? (1024 / sz) : static_cast<size_t>(4));
}

// deque的迭代器
template <typename T>
struct deque_iterator {
    using iterator_category = random_access_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    using size_type = size_t;
    using map_pointer = T **;

    using iterator = deque_iterator<T>;
    using const_iterator = const deque_iterator<T>;
    using self = deque_iterator;

    T *cur;
    T *first;
    T *last;
    map_pointer node;

   public:
    // 缓冲区大小
    static size_t buffer_size();

    // 更新迭代器的状态
    void set_node(map_pointer new_node);

    // 构造函数
    deque_iterator(T *value, map_pointer new_node);
    deque_iterator(const deque_iterator &rhs);
    deque_iterator();

    // 重载运算符
    difference_type operator-(const deque_iterator &rhs) const;

    reference operator*() const;
    pointer operator->() const;

    deque_iterator &operator++();
    deque_iterator operator++(int);
    deque_iterator &operator--();
    deque_iterator operator--(int);

    deque_iterator operator+(difference_type n) const;
    deque_iterator operator-(difference_type n) const;
    deque_iterator &operator+=(difference_type n);
    deque_iterator &operator-=(difference_type n);

    reference operator[](difference_type n) const;

    // 重载比较运算符
    bool operator==(const deque_iterator &rhs) const;
    bool operator!=(const deque_iterator &rhs) const;
    bool operator<(const deque_iterator &rhs) const;
    bool operator<=(const deque_iterator &rhs) const;
    bool operator>(const deque_iterator &rhs) const;
    bool operator>=(const deque_iterator &rhs) const;
};

// 缓冲区大小
template <typename T>
size_t deque_iterator<T>::buffer_size() {
    return deque_buf_size(0, sizeof(T));
}

// 更新迭代器的状态
template <typename T>
void deque_iterator<T>::set_node(map_pointer new_node) {
    node = new_node;
    first = *node;
    last = first + buffer_size();
}

// 构造函数
template <typename T>
deque_iterator<T>::deque_iterator(T *value, map_pointer new_node)
    : cur(value),
      first(*new_node),
      last(first + buffer_size()),
      node(new_node) {}

template <typename T>
deque_iterator<T>::deque_iterator(const deque_iterator &rhs)
    : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

template <typename T>
deque_iterator<T>::deque_iterator()
    : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

// 重载运算符
template <typename T>
typename deque_iterator<T>::difference_type deque_iterator<T>::operator-(
    const deque_iterator<T> &rhs) const {
    return (node - rhs.node) * buffer_size() + (cur - first) -
           (rhs.cur - rhs.first);
}

template <typename T>
typename deque_iterator<T>::reference deque_iterator<T>::operator*() const {
    return *cur;
}

template <typename T>
typename deque_iterator<T>::pointer deque_iterator<T>::operator->() const {
    return &(operator*());
}

template <typename T>
deque_iterator<T> &deque_iterator<T>::operator++() {
    ++cur;
    if (cur == last) {
        set_node(node + 1);
        cur = first;
    }
    return *this;
}

template <typename T>
deque_iterator<T> deque_iterator<T>::operator++(int) {
    deque_iterator tmp = *this;
    ++(*this);
    return tmp;
}

template <typename T>
deque_iterator<T> &deque_iterator<T>::operator--() {
    if (cur == first) {
        set_node(node - 1);
        cur = last;
    }
    --cur;
    return *this;
}

template <typename T>
deque_iterator<T> deque_iterator<T>::operator--(int) {
    deque_iterator tmp = *this;
    --(*this);
    return tmp;
}

template <typename T>
deque_iterator<T> deque_iterator<T>::operator+(difference_type n) const {
    deque_iterator tmp = *this;
    return tmp += n;
}

template <typename T>
deque_iterator<T> deque_iterator<T>::operator-(difference_type n) const {
    deque_iterator tmp = *this;
    return tmp -= n;
}

template <typename T>
deque_iterator<T> &deque_iterator<T>::operator+=(difference_type n) {
    difference_type offset = n + (cur - first);
    if (offset >= 0 && offset < buffer_size())
        cur += n;
    else {
        difference_type node_offset =
            offset > 0 ? offset / buffer_size()
                       : -(((-offset - 1) / buffer_size()) + 1);
        set_node(node + node_offset);
        cur = first + (offset - node_offset * buffer_size());
    }
    return *this;
}

template <typename T>
deque_iterator<T> &deque_iterator<T>::operator-=(difference_type n) {
    return *this += -n;
}

template <typename T>
typename deque_iterator<T>::reference deque_iterator<T>::operator[](
    difference_type n) const {
    return *(*this + n);
}

// 重载比较运算符
template <typename T>
bool deque_iterator<T>::operator==(const deque_iterator &rhs) const {
    return cur == rhs.cur;
}

template <typename T>
bool deque_iterator<T>::operator!=(const deque_iterator &rhs) const {
    return !(*this == rhs);
}

template <typename T>
bool deque_iterator<T>::operator<(const deque_iterator &rhs) const {
    return (node == rhs.node) ? (cur < rhs.cur) : (node < rhs.node);
}

template <typename T>
bool deque_iterator<T>::operator<=(const deque_iterator &rhs) const {
    return *this < rhs || *this == rhs;
}

template <typename T>
bool deque_iterator<T>::operator>(const deque_iterator &rhs) const {
    return (node == rhs.node) ? (cur > rhs.cur) : (node > rhs.node);
}

template <typename T>
bool deque_iterator<T>::operator>=(const deque_iterator &rhs) const {
    return *this > rhs || *this == rhs;
}
}  // namespace mystl
