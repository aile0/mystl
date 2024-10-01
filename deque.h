#pragma once

#include <initializer_list>

#include "deque_iterator.h"
#include "uninitialized.h"

namespace mystl {
template <typename T>
class deque {
   public:
    // 数据类型
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    // 指针，引用类型
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;

    // 迭代器类型
    using iterator = deque_iterator<T>;
    using const_iterator = const deque_iterator<T>;

    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

    // 映射类型
    using map_pointer = pointer *;
    using map_size_type = size_type;

   protected:
    // value,map分配器
    using data_alloc = mystl::simple_alloc<value_type>;
    using map_alloc = mystl::simple_alloc<pointer>;

    // deque内部结构
    iterator start;
    iterator finish;
    map_pointer map;
    map_size_type map_size;

    // 辅助函数
    // 根据默认大小和T的大小，计算map的大小
    static size_type get_map_size();
    static size_type get_initial_map_size();

    // 分配，销毁单个map
    pointer allocate_node();
    void deallocate_node(pointer ptr);

    // 根据元素个数创建和销毁map数组和每个map，并分配空间
    void allocate_map(size_type num_elements);
    void deallocate_map();

    // 重新分配map
    void reallocate_map(size_type added_node_size, bool added_front);

    // 向前，向后扩展n个缓存区
    void expand_map_front(size_type n = 1);
    void expand_map_back(size_type n = 1);

    // 向前，向后扩展n个元素
    iterator expand_front(size_type n = 1);
    iterator expand_back(size_type n = 1);

    // 首部，尾部销毁map
    void destroy_map_front(iterator before_start);
    void destroy_map_back(iterator after_finish);

    // 插入，复制，填充
    template <typename InputIterator>
    void copy_init(InputIterator first, InputIterator last);

    void insert_aux(iterator position, size_type n, const value_type &value);
    void fill_init(size_type n, const value_type &value);

   public:
    // 参数构造函数
    deque();

    template <typename T1>
    deque(const T1 n);

    template <typename T1, typename T2>
    deque(const T1 n, const T2 &value);

    // 多类型参数模板构造函数取代
    //  deque(size_type n);
    //  deque(size_type n, const value_type &value);
    deque(const deque &rhs);

    // 初始化列表
    deque(std::initializer_list<value_type> il);

    // 迭代器构造函数
    template <typename InputIterator>
    deque(InputIterator first, InputIterator last);

    // 析构函数
    ~deque();

    // 赋值函数
    deque &operator=(const deque &rhs);

    // 迭代器相关接口
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    // 反向迭代器
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    // 访问取值接口
    reference operator[](size_type n);
    const_reference operator[](size_type n) const;
    reference at(size_type n);
    const_reference at(size_type n) const;

    // 访问首尾
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    // 容器相关接口
    size_type size() const;
    size_type max_size() const;
    bool empty() const;

    // 修改容器接口
    void swap(deque &rhs);

    // 插入删除接口
    void insert(iterator position);
    void insert(iterator position, const value_type &value);
    void insert(iterator position, size_type n, const value_type &value);

    template <typename InputIterator>
    void insert(iterator position, InputIterator first, InputIterator last);

    void erase(iterator position);
    void erase(iterator first, iterator last);
    void clear();

    void push_front(const value_type &value);
    void push_back(const value_type &value);
    void pop_front();
    void pop_back();

    // 重新分配大小
    void resize(size_type new_size);
    void resize(size_type new_size, const value_type &value);

    // 重载比较运算符
    bool operator==(const deque &rhs) const;
    bool operator!=(const deque &rhs) const;
    bool operator<(const deque &rhs) const;
    bool operator<=(const deque &rhs) const;
    bool operator>(const deque &rhs) const;
    bool operator>=(const deque &rhs) const;
};

// 辅助函数
template <typename T>
typename deque<T>::size_type deque<T>::get_map_size() {
    return deque_buf_size(0, sizeof(T));
}

template <typename T>
typename deque<T>::size_type deque<T>::get_initial_map_size() {
    return 8;
}

template <typename T>
typename deque<T>::pointer deque<T>::allocate_node() {
    return data_alloc::allocate(get_map_size());
}

template <typename T>
void deque<T>::deallocate_node(deque<T>::pointer ptr) {
    data_alloc::deallocate(ptr);
}

template <typename T>
void deque<T>::allocate_map(deque<T>::size_type num_elements) {
    // 确定map数组的大小
    deque<T>::size_type new_map_size = num_elements / get_map_size() + 1;
    // if (num_elements % get_map_size() != 0)
    //     ++new_map_size;

    map_size = std::max(new_map_size, get_initial_map_size() + 2);
    map = map_alloc::allocate(map_size);

    deque<T>::map_pointer new_start = map + (map_size - new_map_size) / 2;
    deque<T>::map_pointer new_finish = new_start + new_map_size - 1;
    deque<T>::map_pointer cur;
    try {
        for (cur = new_start; cur <= new_finish; ++cur) *cur = allocate_node();
    } catch (const std::exception &e) {
        while (cur != new_start) {
            --cur;
            deallocate_node(*cur);
        }
        deque<T>::map_alloc::deallocate(map, map_size);
        std::cout << e.what() << std::endl;
    }

    start.set_node(new_start);
    finish.set_node(new_finish);
    start.cur = start.first;
    finish.cur = finish.first + num_elements % get_map_size();
}

template <typename T>
void deque<T>::deallocate_map() {
    deque<T>::map_pointer cur;
    for (cur = start.node; cur <= finish.node; ++cur) deallocate_node(*cur);
    deque<T>::map_alloc::deallocate(map, map_size);
}

template <typename T>
void deque<T>::reallocate_map(deque<T>::size_type added_node_size,
                              bool added_front) {
    deque<T>::size_type required_map_size =
        finish.node - start.node + 1 + added_node_size;

    deque<T>::map_pointer new_start;
    if (map_size > 2 * required_map_size) {
        new_start = map + (map_size - required_map_size) / 2 +
                    (added_front ? added_node_size : 0);
        if (new_start < start.node)
            std::copy(start.node, finish.node + 1, new_start);
        else
            std::copy_backward(start.node, finish.node + 1,
                               new_start + required_map_size);
    } else {
        deque<T>::size_type new_map_size =
            map_size + std::max(map_size, added_node_size) + 2;
        deque<T>::map_pointer new_map =
            deque<T>::map_alloc::allocate(new_map_size);
        new_start = new_map + (new_map_size - required_map_size) / 2 +
                    (added_front ? added_node_size : 0);
        if (new_start < start.node)
            std::copy(start.node, finish.node + 1, new_start);
        else
            std::copy_backward(start.node, finish.node + 1,
                               new_start + required_map_size);
        deque<T>::map_alloc::deallocate(map);
        map = new_map;
        map_size = new_map_size;
    }
    start.set_node(new_start);
    finish.set_node(new_start + required_map_size - 1);
}

template <typename T>
void deque<T>::expand_map_front(deque<T>::size_type n) {
    if (n > start.node - map) {
        reallocate_map(n, true);
    }
}

template <typename T>
void deque<T>::expand_map_back(deque<T>::size_type n) {
    if (n + 1 > map_size - (finish.node - map)) {
        reallocate_map(n, false);
    }
}

template <typename T>
typename deque<T>::iterator deque<T>::expand_front(deque<T>::size_type n) {
    deque<T>::size_type left = start.cur - start.first;
    if (left < n) {
        deque<T>::size_type required_element = n - left;
        deque<T>::size_type required_node =
            (required_element - 1) / get_map_size();
        if (required_element % get_map_size() != 0) ++required_node;

        expand_map_front(required_node);
        deque<T>::size_type i;
        try {
            for (i = 1; i < required_node; ++i) {
                *(start.node - i) = allocate_node();
            }
        } catch (const std::exception &e) {
            while (--i >= 1) {
                deallocate_node(*(start.node - i));
            }
            std::cerr << e.what() << '\n';
        }
    }
    return start - deque<T>::difference_type(n);
}

template <typename T>
typename deque<T>::iterator deque<T>::expand_back(deque<T>::size_type n) {
    deque<T>::size_type required_element = finish.last - finish.cur;
    deque<T>::size_type required_node = (required_element - 1) / get_map_size();
    if (required_element % get_map_size() != 0) ++required_node;

    expand_map_back(required_node);
    deque<T>::size_type i;
    try {
        for (i = 1; i <= required_node; ++i) {
            *(finish.node + i) = allocate_node();
        }
    } catch (const std::exception &e) {
        while (--i >= 1) {
            deallocate_node(*(finish.node + i));
        }
        std::cerr << e.what() << '\n';
    }
    return finish + deque<T>::difference_type(n);
}

template <typename T>
void deque<T>::destroy_map_front(deque<T>::iterator before_start) {
    for (deque<T>::map_pointer i = before_start; i < start.node; ++i) {
        deallocate_node(*i);
    }
}

template <typename T>
void deque<T>::destroy_map_back(deque<T>::iterator after_finish) {
    for (deque<T>::map_pointer i = after_finish; i > finish.node; --i) {
        deallocate_node(*i);
    }
}

template <typename T>
template <typename InputIterator>
void deque<T>::copy_init(InputIterator first, InputIterator last) {
    allocate_map(0);
    for (; first != last; ++first) {
        push_back(*first);
    }
}

template <typename T>
void deque<T>::insert_aux(deque<T>::iterator position, deque<T>::size_type n,
                          const deque<T>::value_type &value) {
    deque<T>::difference_type offset = position - start;
    deque<T>::size_type elems_before = offset;
    if (elems_before < deque<T>::size() / 2) {
        deque<T>::iterator new_start = expand_front(elems_before);
        deque<T>::iterator old_start = start;
        position = start + elems_before;
        try {
            uninitialized_copy(old_start, position, new_start);
            uninitialized_fill(position - n, position, value);
            start = new_start;
        } catch (const std::exception &e) {
            destroy_map_front(new_start);
            std::cerr << e.what() << '\n';
        }
    } else {
        deque<T>::iterator new_finish = expand_back(n);
        deque<T>::iterator old_finish = finish;
        position = start + elems_before;
        deque<T>::size_type elems_after = deque<T>::size() - elems_before;
        try {
            uninitialized_copy(position, position + elems_after, position + n);
            uninitialized_fill(position, position + n, value);
            finish = new_finish;
        } catch (const std::exception &e) {
            destroy_map_back(new_finish);
            std::cerr << e.what() << '\n';
        }
    }
}

template <typename T>
void deque<T>::fill_init(deque<T>::size_type n,
                         const deque<T>::value_type &value) {
    allocate_map(n);
    deque<T>::map_pointer cur;
    try {
        for (cur = start.node; cur < finish.node; ++cur) {
            uninitialized_fill(*cur, *cur + get_map_size(), value);
        }
        uninitialized_fill(finish.first, finish.cur, value);
    } catch (const std::exception &e) {
        while (--cur >= start.node) {
            destroy(*cur, *cur + get_map_size());
        }
        deallocate_map();

        std::cerr << e.what() << '\n';
    }
}

// 公有接口
// 参数构造函数
template <typename T>
deque<T>::deque() {
    allocate_map(0);
}

template <typename T>
template <typename T1>
deque<T>::deque(const T1 n) {
    fill_init(n, deque<T>::value_type());
}

template <typename T>
template <typename T1, typename T2>
deque<T>::deque(const T1 n, const T2 &value) {
    fill_init(n, value);
}

// 多类型参数模板构造函数取代
//  template <typename T>
//  deque<T>::deque(deque<T>::size_type n)
//  {
//      fill_init(n, deque<T>::value_type());
//  }

// template <typename T>
// deque<T>::deque(deque<T>::size_type n, const value_type &value)
// {
//     fill_init(n, value);
// }

template <typename T>
deque<T>::deque(const deque<T> &rhs) {
    copy_init(rhs.begin(), rhs.end());
}

// 初始化列表
template <typename T>
deque<T>::deque(std::initializer_list<value_type> il) {
    copy_init(il.begin(), il.end());
}

// 迭代器构造函数
template <typename T>
template <typename InputIterator>
deque<T>::deque(InputIterator first, InputIterator last) {
    copy_init(first, last);
}

// 析构函数
template <typename T>
deque<T>::~deque() {
    destroy(start, finish);
    deallocate_map();
}

// 赋值函数
template <typename T>
deque<T> &deque<T>::operator=(const deque<T> &rhs) {
    if (this != &rhs) {
        size_t len = size();
        if (len >= rhs.size()) {
            erase(std::copy(rhs.begin(), rhs.end(), begin()), end());
        } else {
            deque<T>::const_iterator mid =
                rhs.begin() + deque<T>::difference_type(len);
            std::copy(rhs.begin(), rhs.end(), begin());
            insert(end(), mid, rhs.end());
        }
    }
    return *this;
}

// 迭代器相关接口
template <typename T>
typename deque<T>::iterator deque<T>::begin() {
    return start;
}

template <typename T>
typename deque<T>::const_iterator deque<T>::begin() const {
    return start;
}

template <typename T>
typename deque<T>::const_iterator deque<T>::cbegin() const {
    return start;
}

template <typename T>
typename deque<T>::iterator deque<T>::end() {
    return finish;
}

template <typename T>
typename deque<T>::const_iterator deque<T>::end() const {
    return finish;
}

template <typename T>
typename deque<T>::const_iterator deque<T>::cend() const {
    return finish;
}

// 反向迭代器
template <typename T>
typename deque<T>::reverse_iterator deque<T>::rbegin() {
    return reverse_iterator(finish);
}

template <typename T>
typename deque<T>::const_reverse_iterator deque<T>::rbegin() const {
    return const_reverse_iterator(finish);
}

template <typename T>
typename deque<T>::const_reverse_iterator deque<T>::crbegin() const {
    return const_reverse_iterator(finish);
}

template <typename T>
typename deque<T>::reverse_iterator deque<T>::rend() {
    return reverse_iterator(start);
}

template <typename T>
typename deque<T>::const_reverse_iterator deque<T>::rend() const {
    return const_reverse_iterator(start);
}

template <typename T>
typename deque<T>::const_reverse_iterator deque<T>::crend() const {
    return const_reverse_iterator(start);
}

// 访问取值接口
template <typename T>
typename deque<T>::reference deque<T>::operator[](deque<T>::size_type n) {
    return start[difference_type(n)];
}

template <typename T>
typename deque<T>::const_reference deque<T>::operator[](
    deque<T>::size_type n) const {
    return start[difference_type(n)];
}

template <typename T>
typename deque<T>::reference deque<T>::at(deque<T>::size_type n) {
    return *(begin() + n);
}

template <typename T>
typename deque<T>::const_reference deque<T>::at(deque<T>::size_type n) const {
    return *(cbegin() + n);
}

// 访问首尾
template <typename T>
typename deque<T>::reference deque<T>::front() {
    return *begin();
}

template <typename T>
typename deque<T>::const_reference deque<T>::front() const {
    return *cbegin();
}

template <typename T>
typename deque<T>::reference deque<T>::back() {
    deque<T>::iterator tmp = finish;
    --tmp;
    return *tmp;
}

template <typename T>
typename deque<T>::const_reference deque<T>::back() const {
    deque<T>::const_iterator tmp = finish;
    --tmp;
    return *tmp;
}

// 容器相关接口
template <typename T>
typename deque<T>::size_type deque<T>::size() const {
    return finish - start;
}

template <typename T>
typename deque<T>::size_type deque<T>::max_size() const {
    return static_cast<size_type>(-1) / sizeof(T);
}

template <typename T>
bool deque<T>::empty() const {
    return start == finish;
}

template <typename T>
void deque<T>::swap(deque<T> &rhs) {
    std::swap(start, rhs.start);
    std::swap(finish, rhs.finish);
    std::swap(map, rhs.map);
    std::swap(map_size, rhs.map_size);
}

template <typename T>
void deque<T>::insert(deque<T>::iterator position) {
    insert(position, 1, T());
}

template <typename T>
void deque<T>::insert(deque<T>::iterator position,
                      const deque<T>::value_type &value) {
    insert(position, 1, value);  // 委托给其他接口，性能不好
}

template <typename T>
void deque<T>::insert(deque<T>::iterator position, deque<T>::size_type n,
                      const deque<T>::value_type &value) {
    if (position.cur == start.cur) {
        deque<T>::iterator new_start = expand_front(n);
        uninitialized_fill(new_start, start, value);
        start = new_start;
    } else if (position.cur == finish.cur) {
        deque<T>::iterator new_finish = expand_back(n);
        uninitialized_fill(finish, new_finish, value);
        finish = new_finish;
    } else {
        insert_aux(position, n, value);
    }
};

template <typename T>
template <typename InputIterator>
void deque<T>::insert(deque<T>::iterator position, InputIterator first,
                      InputIterator last) {
    std::copy(first, last, std::inserter(*this, position));  // std实现
}

template <typename T>
void deque<T>::erase(deque<T>::iterator position) {
    erase(position, position + 1);
}

template <typename T>
void deque<T>::erase(deque<T>::iterator first, deque<T>::iterator last) {
    if (first == start && last == finish) {
        clear();
    } else {
        deque<T>::difference_type n = last - first;
        deque<T>::difference_type elems_before = first - start;
        if (elems_before < (size() - n) / 2) {
            std::copy_backward(start, first, last);
            deque<T>::iterator new_start = start + n;
            destroy(start, new_start);
            for (deque<T>::map_pointer node = start.node; node < new_start.node;
                 ++node) {
                data_alloc::deallocate(*node, get_map_size());
            }
            start = new_start;
        } else {
            std::copy(last, finish, first);
            deque<T>::iterator new_finish = finish - n;
            destroy(new_finish, finish);
            for (deque<T>::map_pointer node = new_finish.node + 1;
                 node < finish.node; ++node) {
                data_alloc::deallocate(*node, get_map_size());
            }
            finish = new_finish;
        }
    }
}

template <typename T>
void deque<T>::clear() {
    for (deque<T>::map_pointer node = start.node + 1; node < finish.node;
         ++node) {
        destroy(*node, *node + get_map_size());
        deallocate(*node,
                   get_map_size());  // 如果destroy已经释放内存，这里就不需要了
    }
    if (start.node != finish.node) {
        destroy(start.cur, start.last);
        destroy(finish.first, finish.cur);
        deallocate(start.first,
                   get_map_size());  // 如果destroy已经释放内存，这里就不需要了
        deallocate(finish.first, get_map_size());
    } else {
        destroy(start.cur, finish.cur);
        deallocae(start.first,
                  get_map_size());  // 如果destroy已经释放内存，这里就不需要了
    }
}

template <typename T>
void deque<T>::push_front(const deque<T>::value_type &value) {
    if (start.first != start.cur) {
        --start.cur;
        mystl::construct(start.cur, value);
    } else {
        expand_front(1);
        *(start.node - 1) = allocate_node();
        try {
            start.set_node(start.node - 1);
            start.cur = start.last - 1;
            mystl::construct(start.cur, value);
        } catch (const std::exception &e) {
            deallocate_node(*(start.node - 1));
            std::cerr << e.what() << '\n';
        }
    }
}

template <typename T>
void deque<T>::push_back(const deque<T>::value_type &value) {
    if (finish.cur != finish.last - 1) {
        mystl::construct(finish.cur, value);
        ++finish.cur;
    } else {
        expand_back(1);
        *(finish.node + 1) = allocate_node();
        try {
            mystl::construct(finish.cur, value);
            finish.set_node(finish.node + 1);
            finish.cur = finish.first;
        } catch (const std::exception &e) {
            deallocate_node(*(finish.node + 1));
            std::cerr << e.what() << '\n';
        }
    }
}

template <typename T>
void deque<T>::pop_front() {
    if (size() <= 0) {
        return;
    }
    destroy(start.cur);
    if (start.cur != start.last - 1) {
        ++start.cur;
    } else {
        deallocate_node(start.first);
        start.set_node(start.node + 1);
        start.cur = start.first;
    }
}

template <typename T>
void deque<T>::pop_back() {
    if (size() <= 0) {
        return;
    }
    if (finish.cur != finish.first) {
        --finish.cur;
        destroy(finish.cur);
    } else {
        deallocate_node(finish.last);
        finish.set_node(finish.node - 1);
        finish.cur = finish.last - 1;
        destroy(finish.cur);
    }
}

template <typename T>
void deque<T>::resize(deque<T>::size_type new_size) {
    resize(new_size, T());
}

template <typename T>
void deque<T>::resize(deque<T>::size_type new_size,
                      const deque<T>::value_type &value) {
    if (new_size < size()) {
        erase(start + new_size, finish);
    } else {
        insert(finish, new_size - size(), value);
    }
}

template <typename T>
bool deque<T>::operator==(const deque &rhs) const {
    return size() == rhs.size() && std::equal(begin(), end(), rhs.begin());
}

template <typename T>
bool deque<T>::operator!=(const deque &rhs) const {
    return !(*this == rhs);
}

template <typename T>
bool deque<T>::operator<(const deque &rhs) const {
    return std::lexicographical_compare(begin(), end(), rhs.begin(), rhs.end());
}

template <typename T>
bool deque<T>::operator<=(const deque &rhs) const {
    return (*this == rhs) || (*this < rhs);
}

template <typename T>
bool deque<T>::operator>(const deque &rhs) const {
    return std::lexicographical_compare(rhs.begin(), rhs.end(), begin(), end());
}

template <typename T>
bool deque<T>::operator>=(const deque &rhs) const {
    return (*this == rhs) || (*this > rhs);
}
}  // namespace mystl
