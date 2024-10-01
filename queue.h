#pragma once

#include "list.h"

namespace mystl {
template <typename T, typename Container = mystl::list<T>>
class queue {
   public:
    using value_type = T;
    using size_type = size_t;
    using reference = value_type &;
    using const_reference = const value_type &;

   protected:
    Container con;

   public:
    bool empty() const { return con.empty(); }
    size_type size() const { return con.size(); }
    reference front() { return con.front(); }
    reference back() { return con.back(); }
    void push(const value_type &value) { con.push_back(value); }
    void pop() { con.pop_front(); }

    template <typename T1, typename C1>
    friend bool operator==(const queue &lhs, const queue &rhs);

    template <typename T1, typename C1>
    friend bool operator<(const queue &lhs, const queue &rhs);
};

template <typename T, typename Container>
bool operator==(const queue<T, Container> &lhs,
                const queue<T, Container> &rhs) {
    return lhs.con == rhs.con;
}

template <typename T, typename Container>
bool operator<(const queue<T, Container> &lhs, const queue<T, Container> &rhs) {
    return lhs.con < rhs.con;
}
}  // namespace mystl