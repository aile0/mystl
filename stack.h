#pragma once

#include "vector.h"

namespace mystl
{
    template <typename T, typename Container = mystl::vector<T>>
    class stack
    {
    public:
        using value_type = T;
        using size_type = typename Container::size_type;
        using container_type = Container;
        using reference = value_type &;
        using const_reference = value_type const &;
        using pointer = typename Container::pointer;
        using const_pointer = typename Container::const_pointer;

        // 访问操作
        reference top() { return container.back(); }
        const_reference top() const { return container.back(); }

        // 容器操作
        void push(const value_type &value) { container.push_back(value); }
        void pop() { container.pop_back(); }

        // 容器信息
        bool empty() const { return container.empty(); }
        size_type size() const { return container.size(); }

        friend bool operator==(const stack &lhs, const stack &rhs)
        {
            return lhs.container == rhs.container;
        }

        friend bool operator!=(const stack &lhs, const stack &rhs)
        {
            return !(lhs == rhs);
        }

    protected:
        Container container;
    };

}