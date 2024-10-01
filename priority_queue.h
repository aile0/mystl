#pragma once

#include "heap.h"
#include "vector.h"

namespace mystl {
template <typename T, typename SequenceContainer = mystl::vector<T>>
class priority_queue {
   public:
    // 数据类型
    using value_type = typename SequenceContainer::value_type;
    using size_type = typename SequenceContainer::size_type;
    using difference_type = typename SequenceContainer::difference_type;

    // 指针，引用类型
    using pointer = typename SequenceContainer::pointer;
    using const_pointer = typename SequenceContainer::const_pointer;
    using reference = typename SequenceContainer::reference;
    using const_reference = typename SequenceContainer::const_reference;

    // 构造函数
    priority_queue();
    template <typename InputIterator>
    priority_queue(InputIterator first, InputIterator last);

    // 容器访问
    bool empty();
    size_type size();
    reference top();

    // 元素操作
    void push(const value_type &value);
    void pop();

   protected:
    SequenceContainer container;
};

template <typename T, typename SequenceContainer>
priority_queue<T, SequenceContainer>::priority_queue() {
    container = SequenceContainer();
}

template <typename T, typename SequenceContainer>
template <typename InputIterator>
priority_queue<T, SequenceContainer>::priority_queue(InputIterator first,
                                                     InputIterator last) {
    container = SequenceContainer(first, last);
    mystl::make_heap(container.begin(), container.end());
}

template <typename T, typename SequenceContainer>
bool priority_queue<T, SequenceContainer>::empty() {
    return container.empty();
}

template <typename T, typename SequenceContainer>
typename priority_queue<T, SequenceContainer>::size_type
priority_queue<T, SequenceContainer>::size() {
    return container.size();
}

template <typename T, typename SequenceContainer>
typename priority_queue<T, SequenceContainer>::reference
priority_queue<T, SequenceContainer>::top() {
    return container.front();
}

template <typename T, typename SequenceContainer>
void priority_queue<T, SequenceContainer>::push(const value_type &value) {
    container.push_back(value);
    mystl::swim(container.begin(), container.end());
}

template <typename T, typename SequenceContainer>
void priority_queue<T, SequenceContainer>::pop() {
    mystl::sink(container.begin(), container.end());
    container.pop_back();
}
}  // namespace mystl