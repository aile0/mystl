#pragma once

#include "functional.h"  // for mystl::less<Key>
#include "rb_tree.h"

namespace mystl {

template <class Key, class T, class Compare = mystl::less<Key>>
class map {
   public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = mystl::pair<const Key, T>;
    using key_compare = Compare;

   private:
    using tree_type = rb_tree<value_type, key_compare>;
    tree_type tree_;

   public:
    using size_type = typename tree_type::size_type;
    using difference_type = typename tree_type::difference_type;

    using base_type = typename tree_type::base_type;
    using node_type = typename tree_type::node_type;
    using base_ptr = typename tree_type::base_ptr;
    using node_ptr = typename tree_type::node_ptr;

    using pointer = typename tree_type::pointer;
    using const_pointer = typename tree_type::const_pointer;
    using reference = typename tree_type::reference;
    using const_reference = typename tree_type::const_reference;

    using iterator = typename tree_type::iterator;
    using const_iterator = typename tree_type::const_iterator;
    using reverse_iterator = typename tree_type::reverse_iterator;
    using const_reverse_iterator = typename tree_type::const_reverse_iterator;

    // Constructor
    map() = default;

    template <class InputIterator>
    map(InputIterator first, InputIterator last) : tree_() {
        tree_.insert_unique(first, last);
    }

    map(std::initializer_list<value_type> il) : tree_() {
        tree_.insert_unique(il.begin(), il.end());
    }

    // Copy constructor
    map(map& rhs) : tree_(rhs.tree_) {}

    map& operator=(map& rhs) {
        tree_ = rhs.tree_;
        return *this;
    }

    map& operator=(std::initializer_list<value_type> il) {
        tree_.clear();
        tree_.insert_unique(il.begin(), il.end());
        return *this;
    }

    // Move constructor
    map(map&& rhs) : tree_(mystl::move(rhs.tree_)) {}

    map& operator=(map&& rhs) {
        tree_ = mystl::move(rhs.tree_);
        return *this;
    }

    // Destructor
    ~map() = default;

    // Auxiliary member functions
    key_compare key_comp() const { return tree_.get_key_compare(); }

    // Iterators
    iterator begin() { return tree_.begin(); }
    const_iterator begin() const { return tree_.begin(); }
    iterator end() { return tree_.end(); }
    const_iterator end() const { return tree_.end(); }

    reverse_iterator rbegin() { return tree_.rbegin(); }
    const_reverse_iterator rbegin() const { return tree_.rbegin(); }
    reverse_iterator rend() { return tree_.rend(); }
    const_reverse_iterator rend() const { return tree_.rend(); }

    const_iterator cbegin() const { return tree_.cbegin(); }
    const_iterator cend() const { return tree_.cend(); }
    const_reverse_iterator crbegin() const { return tree_.crbegin(); }
    const_reverse_iterator crend() const { return tree_.crend(); }

    // Capacity
    bool empty() const { return tree_.empty(); }
    size_type size() const { return tree_.size(); }
    size_type max_size() const { return tree_.max_size(); }

    // Element access
    mapped_type& operator[](const key_type& key);
    mapped_type& operator[](key_type&& key);

    mapped_type& at(const key_type& key);
    const mapped_type& at(const key_type& key) const;

    // Emplace
    template <class... Args>
    mystl::pair<iterator, bool> emplace(Args&&... args);

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args&&... args);

    // Insert
    mystl::pair<iterator, bool> insert(const value_type& value);
    mystl::pair<iterator, bool> insert(value_type&& value);
    mystl::pair<iterator, bool> insert(const_iterator hint,
                                       const value_type& value);
    mystl::pair<iterator, bool> insert(const_iterator hint, value_type&& value);

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last);

    // Erase
    void erase(iterator position);
    size_type erase(const key_type& key);
    void erase(iterator first, iterator last);

    // Clear
    void clear() { tree_.clear(); }

    // Map operations
    // find
    iterator find(const key_type& key) { return tree_.find(key); }
    const_iterator find(const key_type& key) const { return tree_.find(key); }
    // count
    size_type count(const key_type& key) const { return tree_.count(key); }
    // lower_bound
    iterator lower_bound(const key_type& key) { return tree_.lower_bound(key); }
    const_iterator lower_bound(const key_type& key) const {
        return tree_.lower_bound(key);
    }
    // upper_bound
    iterator upper_bound(const key_type& key) { return tree_.upper_bound(key); }
    const_iterator upper_bound(const key_type& key) const {
        return tree_.upper_bound(key);
    }
    // equal_range
    mystl::pair<iterator, iterator> equal_range(const key_type& key) {
        return tree_.equal_range_unique(key);
    }
    mystl::pair<const_iterator, const_iterator> equal_range(
        const key_type& key) const {
        return tree_.equal_range_unique(key);
    }

    // overload compare operators
    bool operator==(const map& rhs) const { return tree_ == rhs.tree_; }
    bool operator!=(const map& rhs) const { return tree_ != rhs.tree_; }
    bool operator<(const map& rhs) const { return tree_ < rhs.tree_; }
    bool operator<=(const map& rhs) const { return tree_ <= rhs.tree_; }
    bool operator>(const map& rhs) const { return tree_ > rhs.tree_; }
    bool operator>=(const map& rhs) const { return tree_ >= rhs.tree_; }

    // swap
    void swap(map& rhs) { tree_.swap(rhs.tree_); }
};

// operator[]
template <class Key, class T, class Compare>
typename map<Key, T, Compare>::mapped_type& map<Key, T, Compare>::operator[](
    const key_type& key) {
    auto itor = tree_.lower_bound(key);
    if (itor == tree_.end() || key_comp()(key, itor->first)) {
        itor = emplace_hint(itor, key, mapped_type());
    }
    return itor->second;
}

template <class Key, class T, class Compare>
typename map<Key, T, Compare>::mapped_type& map<Key, T, Compare>::operator[](
    key_type&& key) {
    auto itor = tree_.lower_bound(key);
    if (itor == tree_.end() || key_comp()(key, itor->first)) {
        itor = emplace_hint(itor, mystl::move(key), mapped_type());
    }
    return itor->second;
}

// at
template <class Key, class T, class Compare>
typename map<Key, T, Compare>::mapped_type& map<Key, T, Compare>::at(
    const key_type& key) {
    auto itor = tree_.lower_bound(key);
    if (itor == tree_.end() || key_comp()(key, itor->first)) {
        throw std::out_of_range("map<Key, T> at()");
    }
    return itor->second;
}

template <class Key, class T, class Compare>
const typename map<Key, T, Compare>::mapped_type& map<Key, T, Compare>::at(
    const key_type& key) const {
    auto itor = tree_.lower_bound(key);
    if (itor == tree_.end() || key_comp()(key, itor->first)) {
        throw std::out_of_range("map<Key, T> at()");
    }
    return itor->second;
}

// emplace
template <class Key, class T, class Compare>
template <class... Args>
mystl::pair<typename map<Key, T, Compare>::iterator, bool>
map<Key, T, Compare>::emplace(Args&&... args) {
    return tree_.emplace_unique(mystl::forward<Args>(args)...);
}

template <class Key, class T, class Compare>
template <class... Args>
typename map<Key, T, Compare>::iterator map<Key, T, Compare>::emplace_hint(
    const_iterator hint, Args&&... args) {
    return tree_.emplace_unique_use_hint(hint, mystl::forward<Args>(args)...);
}

// insert
template <class Key, class T, class Compare>
mystl::pair<typename map<Key, T, Compare>::iterator, bool>
map<Key, T, Compare>::insert(const value_type& value) {
    return tree_.insert_unique(value);
}

template <class Key, class T, class Compare>
mystl::pair<typename map<Key, T, Compare>::iterator, bool>
map<Key, T, Compare>::insert(value_type&& value) {
    return tree_.insert_unique(mystl::move(value));
}

template <class Key, class T, class Compare>
mystl::pair<typename map<Key, T, Compare>::iterator, bool>
map<Key, T, Compare>::insert(const_iterator hint, const value_type& value) {
    return tree_.insert_unique(hint, value);
}

template <class Key, class T, class Compare>
mystl::pair<typename map<Key, T, Compare>::iterator, bool>
map<Key, T, Compare>::insert(const_iterator hint, value_type&& value) {
    return tree_.insert_unique(hint, mystl::move(value));
}

template <class Key, class T, class Compare>
template <class InputIterator>
void map<Key, T, Compare>::insert(InputIterator first, InputIterator last) {
    tree_.insert_unique(first, last);
}

// erase
template <class Key, class T, class Compare>
void map<Key, T, Compare>::erase(iterator position) {
    tree_.erase(position);
}

template <class Key, class T, class Compare>
typename map<Key, T, Compare>::size_type map<Key, T, Compare>::erase(
    const key_type& key) {
    return tree_.erase_unique(key);
}

template <class Key, class T, class Compare>
void map<Key, T, Compare>::erase(iterator first, iterator last) {
    tree_.erase(first, last);
}

// global swap
template <class Key, class T, class Compare>
void swap(map<Key, T, Compare>& lhs, map<Key, T, Compare>& rhs) {
    lhs.swap(rhs);
}

}  // namespace mystl