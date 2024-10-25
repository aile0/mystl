#pragma once

#include <initializer_list>

#include "alloc.h"

namespace mystl {
template <typename T>
struct list_node {
    T data;
    list_node<T> *next;
    list_node<T> *prev;
};

template <typename T>
class list_iterator {
   public:
    using iterator_category = bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = ptrdiff_t;
    using pointer = T *;
    using reference = T &;

    list_node<T> *node;

    // 构造函数
    list_iterator() : node(nullptr) {}
    list_iterator(list_node<T> *x) : node(x) {}
    list_iterator(list_iterator &rhs) : node(rhs.node) {}
    list_iterator(const list_iterator &rhs) : node(rhs.node) {}

    // 析构函数
    ~list_iterator() = default;

    // 重载操作符
    bool operator==(const list_iterator &rhs) const { return node == rhs.node; }
    bool operator!=(const list_iterator &rhs) const { return !(*this == rhs); }

    list_iterator &operator++()  // 前置++
    {
        node = node->next;
        return *this;
    }
    list_iterator operator++(int)  // 后置++
    {
        list_iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    list_iterator &operator--() {
        node = node->prev;
        return *this;
    }
    list_iterator operator--(int) {
        list_iterator tmp = *this;
        --(*this);
        return tmp;
    }

    T &operator*() { return node->data; }
    T *operator->() { return &(operator*()); }

    list_iterator operator+(difference_type n) {
        list_iterator tmp = *this;
        while (n--) {
            ++tmp;
        }
        return tmp;
    }
    list_iterator operator-(difference_type n) {
        list_iterator tmp = *this;
        while (n--) {
            --tmp;
        }
        return tmp;
    }
};

template <typename T, typename Alloc = mystl::simple_alloc<list_node<T>>>
class list {
   public:
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using iterator = list_iterator<T>;
    using const_iterator = const list_iterator<T>;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator =
        const mystl::reverse_iterator<const_iterator>;

    using link_type = list_node<T> *;

   protected:
    link_type node;

    // 辅助函数
    link_type get_node() { return Alloc::allocate(); }
    void put_node(link_type p) { Alloc::deallocate(p); }
    link_type create_node(const T &x) {
        link_type p = get_node();
        try {
            mystl::construct(&p->data, x);
            return p;
        } catch (const std::exception &e) {
            put_node(p);
            std::cerr << e.what() << '\n';
        }
    }
    void destroy_node(link_type p) {
        mystl::destroy(&p->data);
        put_node(p);
    }

    // 辅助函数
    void empty_init();
    void fill_init(size_type n, const T &value);

    template <typename InputIterator>
    void range_init(InputIterator first, InputIterator last);

    void transfer(iterator position, iterator first, iterator last);

   public:
    // 构造函数
    list() { empty_init(); }  // 默认构造函数

    // 指定大小的构造函数
    list(size_type n) { fill_init(n, T()); }
    list(int n) { fill_init(size_type(n), T()); }
    list(long n) { fill_init(size_type(n), T()); }
    list(size_type n, const T &value) { fill_init(n, value); }
    list(int n, const T &value) { fill_init(size_type(n), value); }
    list(long n, const T &value) { fill_init(size_type(n), value); }

    template <typename InputIterator>
    list(InputIterator first, InputIterator last) {
        range_init(first, last);
    }

    list(const list<T> &rhs) { range_init(rhs.begin(), rhs.end()); }
    list(const std::initializer_list<T> &il) {
        range_init(il.begin(), il.end());
    }

    list<T> &operator=(const list<T> &rhs);
    list<T> &operator=(std::initializer_list<T> il);
    ~list() {
        clear();
        put_node(node);
    }

    // 迭代器操作
    iterator begin() { return node->next; }
    const_iterator begin() const { return node->next; }
    iterator end() { return node; }
    const_iterator end() const { return node; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }

    // 容器容量操作
    bool empty() const { return node->next == node; }
    size_type size() const {
        return static_cast<size_type>(distance(begin(), end()));
    }
    size_type max_size() const { return size_type(-1) / sizeof(link_type); }

    // 元素访问操作
    reference front() { return *begin(); }
    const_reference front() const { return *begin(); }
    reference back() { return *(--end()); }
    const_reference back() const { return *(--end()); }
    reference operator[](size_type n) { return *(begin() + n); }

    // 修改链表操作
    void swap(list<T> &rhs) { std::swap(node, rhs.node); }

    void insert(iterator position, const T &x);
    void insert(iterator position, size_type n, const T &x);
    template <typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last);

    iterator erase(iterator position);
    iterator erase(iterator first, iterator last);

    void push_front(const T &x);
    void push_back(const T &x);
    void pop_front();
    void pop_back();

    void resize(size_type new_size);
    void resize(size_type new_size, const T &x);
    void clear();

    void remove(const T &value);
    void sort();
    void merge(list<T> &other_list);
    void reverse();
    void unique();

    // 链表移动，依靠tranfer实现
    void splice(iterator position, list &other_list);
    void splice(iterator position, list &other_list, iterator i);
    void splice(iterator position, list &other_list, iterator first,
                iterator last);

    // 比较操作
    template <typename T1, typename Alloc1>
    friend bool operator==(const list<T1> &lhs, const list<T1> &rhs);

    template <typename T1, typename Alloc1>
    friend bool operator<(const list<T1> &lhs, const list<T1> &rhs);
};
// 辅助函数
template <typename T, typename Alloc>
void list<T, Alloc>::empty_init() {
    node = get_node();
    node->next = node;
    node->prev = node;
}

template <typename T, typename Alloc>
void list<T, Alloc>::fill_init(size_type n, const T &value) {
    empty_init();
    try {
        insert(begin(), n, value);
    } catch (const std::exception &e) {
        clear();
        put_node(node);
        std::cerr << e.what() << '\n';
    }
}

template <typename T, typename Alloc>
template <typename InputIterator>
void list<T, Alloc>::range_init(InputIterator first, InputIterator last) {
    empty_init();
    try {
        insert(begin(), first, last);
    } catch (const std::exception &e) {
        clear();
        put_node(node);
        std::cerr << e.what() << '\n';
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::transfer(iterator pos, iterator first, iterator last) {
    if (pos != last) {
        // 1. 将 `[first, last)` 从当前位置分离出来
        first.node->prev->next =
            last.node;  // last.node 之前的下一个节点是 last.node
        last.node->prev->next =
            pos.node;  // first.node 之前的下一个节点是 pos.node
        pos.node->prev->next =
            first.node;  // pos.node 之前的下一个节点是 first.node

        // 2. 更新 prev 指针
        link_type tmp = pos.node->prev;
        pos.node->prev =
            last.node->prev;  // pos.node 之前的节点变成 last.node 的前一个节点
        last.node->prev =
            first.node
                ->prev;  // last.node 的前一个节点变成 first.node 的前一个节点
        first.node->prev =
            tmp;  // first.node 的前一个节点变成原 pos.node 的前一个节点
    }
}

template <typename T, typename Alloc>
list<T> &list<T, Alloc>::operator=(const list<T> &rhs) {
    if (this != &rhs) {
        clear();
        insert(begin(), rhs.begin(), rhs.end());
    }
}

template <typename T, typename Alloc>
list<T> &list<T, Alloc>::operator=(std::initializer_list<T> il) {
    clear();
    insert(begin(), il.begin(), il.end());
}

template <typename T, typename Alloc>
void list<T, Alloc>::insert(iterator position, const T &x) {
    link_type tmp = create_node(x);
    tmp->next = position.node;
    tmp->prev = position.node->prev;
    position.node->prev->next = tmp;
    position.node->prev = tmp;
}

template <typename T, typename Alloc>
void list<T, Alloc>::insert(iterator position, size_type n, const T &x) {
    for (size_type i = 0; i < n; ++i) {
        insert(position, x);
    }
}

template <typename T, typename Alloc>
template <typename InputIterator>
void list<T, Alloc>::insert(iterator position, InputIterator first,
                            InputIterator last) {
    for (; first != last; ++first) {
        insert(position, *first);
    }
}

template <typename T, typename Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator position) {
    link_type tmp = position.node;
    position.node->prev->next = position.node->next;
    position.node->next->prev = position.node->prev;
    destroy_node(tmp);
    return position;
}

template <typename T, typename Alloc>
typename list<T, Alloc>::iterator list<T, Alloc>::erase(iterator first,
                                                        iterator last) {
    while (first != last) {
        erase(first++);
    }
    return last;
}

template <typename T, typename Alloc>
void list<T, Alloc>::push_front(const T &x) {
    insert(begin(), x);
}

template <typename T, typename Alloc>
void list<T, Alloc>::push_back(const T &x) {
    insert(end(), x);
}

template <typename T, typename Alloc>
void list<T, Alloc>::pop_front() {
    erase(begin());
}

template <typename T, typename Alloc>
void list<T, Alloc>::pop_back() {
    erase(--end());
}

template <typename T, typename Alloc>
void list<T, Alloc>::resize(size_type new_size) {
    resize(new_size, T());
}

template <typename T, typename Alloc>
void list<T, Alloc>::resize(size_type new_size, const T &value) {
    size_type len = size();
    if (new_size < len) {
        erase(begin() + new_size, end());
    } else {
        insert(end(), new_size - len, value);
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::clear() {
    erase(begin(), end());
}

template <typename T, typename Alloc>
void list<T, Alloc>::remove(const T &value) {
    iterator first = begin();
    iterator last = end();
    while (first != last) {
        if (*first == value) {
            erase(first);
        } else {
            ++first;
        }
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::sort() {
    if (empty() || size() == 1) return;

    list<T, Alloc> tmp;
    list<T, Alloc> count[64];
    int fill = 0;
    while (!empty()) {
        tmp.splice(tmp.begin(), *this, begin());
        int i = 0;
        while (i < fill && !count[i].empty()) {
            count[i].merge(tmp);
            tmp.swap(count[i++]);
        }
        tmp.swap(count[i]);
        if (i == fill) ++fill;
    }
    for (int i = 1; i < fill; ++i) {
        count[i].merge(count[i - 1]);
    }
    swap(*this, count[fill - 1]);
}

// 默认有序
template <typename T, typename Alloc>
void list<T, Alloc>::merge(list<T> &other_list) {
    if (other_list.empty()) return;

    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = other_list.begin();
    iterator last2 = other_list.end();
    while (first2 != last2) {
        if (first1 == last1 || *first2 < *first1) {
            iterator next = first2;
            ++next;
            splice(first1, other_list, first2);
            first2 = next;
        } else {
            ++first1;
        }
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::reverse() {
    if (empty() || size() == 1) return;

    iterator first = begin();
    ++first;
    while (first != end()) {
        iterator old = first;
        ++first;
        tranfer(begin(), old, first);
    }
}

// 已经默认有序
template <typename T, typename Alloc>
void list<T, Alloc>::unique() {
    if (empty() || size() == 1) return;

    iterator first = begin();
    iterator last = end();
    iterator next = first;
    while (++next != last) {
        if (*first == *next) {
            first = erase(first);
        } else {
            first = next;
        }
    }
}

// splice实现
template <typename T, typename Alloc>
void list<T, Alloc>::splice(iterator position, list &other_list) {
    if (!other_list.empty()) {
        transfer(position, other_list.begin(), other_list.end());
    }
}

template <typename T, typename Alloc>
void list<T, Alloc>::splice(iterator position, list &other_list, iterator i) {
    iterator j = i;
    ++j;
    if (position == i || position == j) return;
    transfer(position, i, j);
}

template <typename T, typename Alloc>
void list<T, Alloc>::splice(iterator position, list &other_list, iterator first,
                            iterator last) {
    if (first != last) transfer(position, first, last);
}

template <typename T, typename Alloc>
inline bool operator==(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    auto lhs_first = lhs.begin();  // 返回cosnt和非cosnt错误
    auto lhs_last = lhs.end();
    auto rhs_first = rhs.begin();
    auto rhs_last = rhs.end();
    for (; lhs_first != lhs_last && rhs_first != rhs_last;
         ++lhs_first, ++rhs_first) {
        if (*lhs_first != *rhs_first) {
            return false;
        }
    }
    return (lhs_first == lhs_last) && (rhs_first == rhs_last);
}

template <typename T, typename Alloc>
inline bool operator<(const list<T, Alloc> &lhs, const list<T, Alloc> &rhs) {
    auto lhs_first = lhs.begin();
    auto lhs_last = lhs.end();
    auto rhs_first = rhs.begin();
    auto rhs_last = rhs.end();
    for (; lhs_first != lhs_last && rhs_first != rhs_last;
         ++lhs_first, ++rhs_first) {
        if (*lhs_first < *rhs_first) {
            return true;
        } else if (*rhs_first < *lhs_first) {
            return false;
        }
    }
    return (lhs_first == lhs_last) && (rhs_first != rhs_last);
}
}  // namespace mystl