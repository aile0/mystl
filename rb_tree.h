#pragma once

#include "allocator.h"
#include "exceptdef.h"
#include "iterator.h"
#include "rb_tree_algorithm.h"
#include "type_traits.h"
#include "util.h"

namespace mystl {
// define rb_tree_color type
using rb_tree_color = bool;
constexpr rb_tree_color rb_tree_red = false;
constexpr rb_tree_color rb_tree_black = true;

// forward declaration
template <class T>
class rb_tree_node_base;

template <class T>
class rb_tree_node;

template <class T>
class rb_tree_iterator;

template <class T>
class rb_tree_const_iterator;

// define value traits for costum type
template <class T, bool>
class rb_tree_value_traits_imp {
   public:
    // value(key,map_value) key == map_value
    using key_type = T;
    using map_value_type = T;
    using value_type = T;

    // return key
    template <class U>
    static const key_type& get_key(const value_type& value) {
        return value;
    }

    // return value
    template <class U>
    static const value_type& get_value(const value_type& value) {
        return value;
    }
};

// define value traits for pair type
template <class T>
class rb_tree_value_traits_imp<T, true> {
   public:
    // value(key,map_value) pair(key,map_value)
    using key_type = typename std::remove_cv<T>::type;
    using map_value_type = typename T::second_type;
    using value_type = T;

    // return key of pair
    template <class U>
    static const key_type& get_key(const value_type& value) {
        return value.first;
    }

    // return pair
    template <class U>
    static const value_type& get_value(const value_type& value) {
        return value;
    }
};

// define rb_tree_value_traits
// encapsulation of value_traits_imp selection in node_traits and tree_traits
template <class T>
class rb_tree_value_traits {
   public:
    // judge whether T is pair
    static constexpr bool is_pair = mystl::is_pair<T>::value;
    // select right value_traits
    using value_traits = rb_tree_value_traits_imp<T, is_pair>;
    // define type
    using key_type = typename value_traits::key_type;
    using map_value_type = typename value_traits::map_value_type;
    using value_type = typename value_traits::value_type;

    // return key
    template <class U>
    static const key_type& get_key(const value_type& value) {
        return value_traits::get_key(value);
    }

    // return value
    template <class U>
    static const value_type& get_value(const value_type& value) {
        return value_traits::get_value(value);
    }
};

// define rb_tree_node_traits
// encapsulation of rb_tree_color, ptr and value_traits
template <class T>
class rb_tree_node_traits {
   public:
    using color_type = rb_tree_color;

    using value_traits = rb_tree_value_traits<T>;

    using key_type = typename value_traits::key_type;
    using map_value_type = typename value_traits::map_value_type;
    using value_type = typename value_traits::value_type;

    using base_ptr = rb_tree_node_base<T>*;
    using node_ptr = rb_tree_node<T>*;
};

// define rb_tree_node_base
// encapsulation of rb_tree_color, ptr
template <class T>
class rb_tree_node_base {
   public:
    using color_type = rb_tree_color;

    using base_ptr = rb_tree_node_base<T>*;
    using node_ptr = rb_tree_node<T>*;

    // member base_ptr for polymorphism
    color_type color;
    base_ptr parent;
    base_ptr left;
    base_ptr right;

    // member function
    base_ptr get_base_ptr() { return this; }
    // reinterpret_cast(we konw the cvt is safe)
    node_ptr get_node_ptr() { return reinterpret_cast<node_ptr>(this); }

    node_ptr get_node_ref() { return reinterpret_cast<node_ptr&>(this); }
};

// define rb_tree_node
// encapsulation of rb_tree_node_base and value
template <class T>
class rb_tree_node : public rb_tree_node_base<T> {
   public:
    using base_ptr = rb_tree_node_base<T>*;
    using node_ptr = rb_tree_node<T>*;

    // member value for polymorphism
    T value;

    // member function
    base_ptr get_base_ptr() { return static_cast<base_ptr>(this); }

    node_ptr get_node_ptr() { return this; }
};

// define rb_tree_traits
template <class T>
class rb_tree_traits {
   public:
    using value_traits = rb_tree_value_traits<T>;

    using key_type = typename value_traits::key_type;
    using map_value_type = typename value_traits::map_value_type;
    using value_type = typename value_traits::value_type;

    using pointer = value_type*;
    using reference = value_type&;
    using const_pointer = const value_type*;
    using const_reference = const value_type&;

    using base_type = rb_tree_node_base<T>;
    using node_type = rb_tree_node<T>;

    using base_ptr = base_type*;
    using node_ptr = node_type*;
    using const_base_ptr = const base_type*;
    using const_node_ptr = const node_type*;
};

// define rb_tree_iterator_base
// for management and auxiliary operation
template <class T>
class rb_tree_iterator_base : public iterator<bidirectional_iterator_tag, T> {
   public:
    using base_ptr = rb_tree_node_base<T>*;
    using node_ptr = rb_tree_node<T>*;
    // member base_ptr for polymorphism and management
    base_ptr node;
    // member function for iterator operation
    rb_tree_iterator_base() : node(nullptr) {}
    void increment() {
    }  /////////////////////////////////////////////////////////////////
    void decrement() {
    }  /////////////////////////////////////////////////////////////////
};

// define rb_tree_iterator
template <class T>
class rb_tree_iterator : public rb_tree_iterator_base<T> {
   public:
    using tree_traits = rb_tree_traits<T>;

    using key_type = typename tree_traits::key_type;
    using map_value_type = typename tree_traits::map_value_type;
    using value_type = typename tree_traits::value_type;

    using pointer = typename tree_traits::pointer;
    using reference = typename tree_traits::reference;

    using base_ptr = typename tree_traits::base_ptr;
    using node_ptr = typename tree_traits::node_ptr;

    using iterator = rb_tree_iterator<T>;
    using const_iterator = rb_tree_const_iterator<T>;

    // member
    using rb_tree_iterator_base<T>::node;

    // constructor
    rb_tree_iterator() {}
    rb_tree_iterator(base_ptr x) { node = x; }
    rb_tree_iterator(node_ptr x) { node = x->get_base_ptr(); }
    rb_tree_iterator(iterator& x) { node = x.node; }
    rb_tree_iterator(const_iterator& x) { node = x.node; }

    // member function
    reference operator*() const { return node->get_node_ptr()->value; }
    pointer operator->() const { return &(operator*()); }
    iterator& operator++() {
        this->increment();
        return *this;
    }
    iterator operator++(int) {
        iterator tmp = *this;
        this->increment();
        return tmp;
    }
    iterator& operator--() {
        this->decrement();
        return *this;
    }
    iterator operator--(int) {
        iterator tmp = *this;
        this->decrement();
        return tmp;
    }
};

// define rb_tree_const_iterator
template <class T>
class rb_tree_const_iterator : public rb_tree_iterator_base<T> {
   public:
    using tree_traits = rb_tree_traits<T>;

    using key_type = typename tree_traits::key_type;
    using map_value_type = typename tree_traits::map_value_type;
    using value_type = typename tree_traits::value_type;

    using pointer = typename tree_traits::const_pointer;
    using reference = typename tree_traits::const_reference;

    using base_ptr = typename tree_traits::const_base_ptr;
    using node_ptr = typename tree_traits::const_node_ptr;

    using iterator = rb_tree_iterator<T>;
    using const_iterator = rb_tree_const_iterator<T>;

    // member
    using rb_tree_iterator_base<T>::node;

    // constructor
    rb_tree_const_iterator() {}
    rb_tree_const_iterator(base_ptr x) { node = x; }
    rb_tree_const_iterator(node_ptr x) { node = x->get_base_ptr(); }
    rb_tree_const_iterator(iterator& x) { node = x.node; }
    rb_tree_const_iterator(const_iterator& x) { node = x.node; }

    // member function
    reference operator*() const { return node->get_node_ptr()->value; }
    pointer operator->() const { return &(operator*()); }
    const_iterator& operator++() {
        this->increment();
        return *this;
    }
    const_iterator operator++(int) {
        const_iterator tmp = *this;
        this->increment();
        return tmp;
    }
    const_iterator& operator--() {
        this->decrement();
        return *this;
    }
    const_iterator operator--(int) {
        const_iterator tmp = *this;
        this->decrement();
        return tmp;
    }
};

// define rb_tree
template <class T, class Compare>
class rb_tree {
   public:
    using tree_traits = rb_tree_traits<T>;
    using value_traits = typename rb_tree_value_traits<T>;

    using base_type = tree_traits::base_type;
    using node_type = tree_traits::node_type;
    using base_ptr = tree_traits::base_ptr;
    using node_ptr = tree_traits::node_ptr;

    using key_type = typename tree_traits::key_type;
    using map_value_type = typename tree_traits::map_value_type;
    using value_type = typename tree_traits::value_type;

    using data_allocator = mystl::allocator<T>;
    using base_allocator = mystl::allocator<base_type>;
    using node_allocator = mystl::allocator<node_type>;

    using pointer = mystl::allocator<T>::pointer;
    using reference = mystl::allocator<T>::reference;
    using const_pointer = mystl::allocator<T>::const_pointer;
    using const_reference = mystl::allocator<T>::const_reference;

    using iterator = rb_tree_iterator<T>;
    using const_iterator = rb_tree_const_iterator<T>;
    using reverse_iterator = mystl::reverse_iterator<iterator>;
    using const_reverse_iterator = mystl::reverse_iterator<const_iterator>;

   private:
    // member
    // header parent is root, left is leftmost, right is rightmost
    base_ptr header;
    size_t node_count;
    Compare key_compare;

    // auxiliary function  //////////////////////////////////////////////////
    base_ptr& root() const { return header->parent; }
    base_ptr& leftmost() const { return header->left; }
    base_ptr& rightmost() const { return header->right; }

    // node auxiliary function
    template <class... Args>
    node_ptr create_node(Args&&... args);

    node_ptr clone_node(base_ptr x);
    void destroy_node(base_ptr x);

    // initial auxiliary function
    void rb_tree_init();

    // reset rb_tree (for move)
    void reset();

    // get insert multi position
    mystl::pair<base_ptr, bool> get_insert_multi_pos(const key_type& key);
    // get insert unique position
    mystl::pair<mystl::pair<base_ptr, bool>, bool> get_insert_unique_pos(
        const key_type& key);

    // inert value at position
    iterator insert_value_at(base_ptr x, const value_type& value,
                             bool add_to_left);
    // insert node at position
    iterator insert_node_at(base_ptr x, node_ptr node, bool add_to_left);

    // insert multi use hint
    iterator insert_multi_use_hint(base_ptr hint, key_type key, node_ptr node);
    // insert unique use hint
    iterator insert_unique_use_hint(base_ptr hint, key_type key, node_ptr node);

    // copy_tree org to child of ohr_parent
    base_ptr copy_tree(base_ptr org, base_ptr ohr_parent);
    // erase subtree
    void erase_subtree(base_ptr x);

    // member function  /////////////////////////////////////////////////////
    // Construct, Copy, Destroy
    rb_tree() { rb_tree_init(); }

    rb_tree(const rb_tree& rhs);
    rb_tree(rb_tree&& rhs);

    rb_tree& operator=(const rb_tree& rhs);
    rb_tree& operator=(rb_tree&& rhs);

    ~rb_tree() { clear(); }

    // Iterators
    iterator begin() { return leftmost(); }
    const_iterator begin() const { return leftmost(); }
    iterator end() { return header; }  // use header as end for iterator compare
    const_iterator end() const { return header; }
    // reverse iterator
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(end());
    }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const {
        return const_reverse_iterator(begin());
    }
    // const iterator
    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
    const_iterator crbegin() const { return rbegin(); }
    const_iterator crend() const { return rend(); }

    // Capacity
    bool empty() const { return node_count == 0; }
    size_t size() const { return node_count; }
    size_t max_size() const { return static_case<size_t>(-1); }

    // insert

    // erase
    void clear();

    // rb_tree operation
    // find

    // swap
    void swap(rb_tree& rhs);

    // global overload
    // ...
};

// auxiliary function
// create one node from param
template <class T, class Compare>
template <class... Args>
typename rb_tree<T, Compare>::node_ptr rb_tree<T, Compare>::create_node(
    Args&&... args) {
    node_ptr tmp = node_allocator::allocate(1);
    try {
        data_allocator::construct(&tmp->value, std::forward<Args>(args)...);
        tmp->parent = nullptr;
        tmp->left = nullptr;
        tmp->right = nullptr;
    } catch (const std::exception& e) {
        node_allocator::deallocate(tmp);
        throw;
    }
    return tmp;
}

// copy one node
template <class T, class Compare>
typename rb_tree<T, Compare>::node_ptr rb_tree<T, Compare>::clone_node(
    rb_tree<T, Compare>::base_ptr x) {
    auto clone = create_node(x.get_node_ptr()->value);
    clone->color = x->color;
    clone->left = nullptr;
    clone->right = nullptr;
    return clone;
}

// destroy one node
template <class T, class Compare>
void rb_tree<T, Compare>::destroy_node(rb_tree<T, Compare>::base_ptr x) {
    data_allocator::destroy(&x.get_node_ptr()->value);
    node_allocator::deallocate(x.get_node_ptr());
}

// initial auxiliary function
template <class T, class Compare>
void rb_tree<T, Compare>::rb_tree_init() {
    header = base_allocator::allocate(1);
    header->color = rb_tree_red;
    header->parent = nullptr;
    header->left = header;
    header->right = header;
    node_count = 0;
}

// reset rb_tree (for move)
template <class T, class Compare>
void reset() {
    header = nullptr;
    node_count = 0;
}

// get insert multi position
template <class T, class Compare>
mystl::pair<typename rb_tree<T, Compare>::base_ptr, bool>
rb_tree<T, Compare>::get_insert_multi_pos(const key_type& key) {
    auto parent = header;
    auto cur = root();
    bool add_to_left = true;
    while (cur != nullptr) {
        parent = cur;
        add_to_left = key_compare(key, cur->get_node_ptr()->value);
        // left < , right >=
        cur = add_to_left ? cur->left : cur->right;
    }
    return mystl::pair<parent, add_to_left>;
}

// get insert unique position
template <class T, class Compare>
mystl::pair<mystl::pair<typename rb_tree<T, Compare>::base_ptr, bool>, bool>
rb_tree<T, Compare>::get_insert_unique_pos(const key_type& key) {
    auto parent = header;
    auto cur = root();
    bool add_to_left = true;
    while (cur != nullptr) {
        parent = cur;
        add_to_left = key_compare(key, cur->get_node_ptr()->value);
        cur = add_to_left ? cur->left : cur->right;
    }
    iterator itor = iterator(parent);
    if (add_to_left) {
        if (parent == header || itor == begin()) {
            return mystl::pair(mystl::pair(parent, true), true);
        } else {
            --itor;
        }
    }
    if (key_compare(value_traits::get_key(*itor), key)) {
        return mystl::pair(mystl::pair(parent, add_to_left), true);
    }
    return mystl::pair(mystl::pair(parent, add_to_left), false);
}

// insert value at position
template <class T, class Compare>
rb_tree<T, Compare>::iterator rb_tree<T, Compare>::insert_value_at(
    base_ptr x, const value_type& value, bool add_to_left) {
    auto node = create_node(value);
    auto parent = x;
    node.parent = parent;
    auto base_ptr = node.get_base_ptr();
    if (parent == header) {
        root() = base_ptr;
        leftmost() = base_ptr;
        rightmost() = base_ptr;
    } else if (add_to_left) {
        parent->left = base_ptr;
        if (parent == leftmost()) leftmost() = base_ptr;
    } else {
        parent->right = base_ptr;
        if (parent == rightmost()) rightmost() = base_ptr;
    }
    ++node_count;
    return iterator(node);
}

// insert node at position
template <class T, class Compare>
rb_tree<T, Compare>::iterator rb_tree<T, Compare>::insert_node_at(
    base_ptr x, node_ptr node, bool add_to_left) {
    auto parent = x;
    node->parent = parent;
    auto base_ptr = node->get_base_ptr();
    if (parent == header) {
        root() = base_ptr;
        leftmost() = base_ptr;
        rightmost() = base_ptr;
    } else if (add_to_left) {
        parent->left = base_ptr;
        if (parent == leftmost()) leftmost() = base_ptr;
    } else {
        parent->right = base_ptr;
        if (parent == rightmost()) rightmost() = base_ptr;
    }
    ++node_count;
    return iterator(node);
}

// insert multi use hint
template <class T, class Compare>
rb_tree<T, Compare>::iterator rb_tree<T, Compare>::insert_multi_use_hint(
    base_ptr hint, key_type key, node_ptr node) {
    auto np = hint.node;
    auto before = iterator(hint);
    --before;
    auto bnp = before.node;
    if (!key_compare(key, value_traits::get_key(*before)) &&
        !key_compare(value_traits::get_key(*hint), key)) {
        if (bnp->right == nullptr) {
            return insert_node_at(bnp, node, false);
        }
        if (np->left == nullptr) {
            return insert_node_at(np, node, true);)
        }
    }  // before <= key <=hint
    auto pos = get_insert_multi_pos(key);
    return insert_node_at(pos.first, node, pos.second);
}

// insert unique use hint
template <class T, class Compare>
rb_tree<T, Compare>::iterator rb_tree<T, Compare>::insert_unique_use_hint(
    base_ptr hint, key_type key, node_ptr node) {
    auto np = hint.node;
    auto before = iterator(hint);
    --before;
    auto bnp = before.node;
    if (key_compare(value_traits::get_key(*before), key) &&
        key_compare(key, value_traits::get_key(*hint))) {
        if (bnp->right == nullptr) {
            return insert_node_at(bnp, node, false);
        }
        if (np->left == nullptr) {
            return insert_node_at(np, node, true);
        }
    }  // before < key < hint
    auto pos = get_insert_unique_pos(key);
    if (!pos.second) {
        destroy_node(node);
        return pos.first.first;
    }
    return insert_node_at(pos.first.first, node, pos.first.second);
}

// copy tree org to child of ohr_parent
template <class T, class Compare>
rb_tree<T, Compare>::base_ptr rb_tree<T, Compare>::copy_tree(
    base_ptr org, base_ptr ohr_parent) {
    if (org == nullptr) return nullptr;
    auto top = clone_node(org);
    top->parent = ohr_parent;
    if (org->right != nullptr) {
        top->right = copy_tree(org->right, top);
    }
    if (org->left != nullptr) {
        top->left = copy_tree(org->left, top);
    }
    return top;
}

// erase subtree
template <class T, class Compare>
void rb_tree<T, Compare>::erase_subtree(base_ptr x) {
    if (x == nullptr) return;
    erase_subtree(x->left);
    erase_subtree(x->right);
    destroy_node(x);
}

// member function
// copy constructor
template <class T, class Compare>
rb_tree<T, Compare>::rb_tree(const rb_tree& rhs) {
    rb_tree_init();
    if (rhs.node_count != 0) {
        root() = copy_tree(rhs.root(), header);
        leftmost() = rb_tree_minimum(root());
        rightmost() = rb_tree_maximum(root());
    }
    node_count = rhs.node_count;
    key_compare = rhs.key_compare;
}

// move constructor
template <class T, class Compare>
rb_tree<T, Compare>::rb_tree(rb_tree&& rhs) {
    header = rhs.header;
    node_count = rhs.node_count;
    key_compare = rhs.key_compare;
    rhs.reset();
}

// copy assignment
template <class T, class Compare>
rb_tree<T, Compare>& rb_tree<T, Compare>::operator=(const rb_tree& rhs) {
    if (this != &rhs) {
        clear();
        if (rhs.node_count != 0) {
            root() = copy_tree(rhs.root(), header);
            leftmost() = rb_tree_minimum(root());
            rightmost() = rb_tree_maximum(root());
        }
        node_count = rhs.node_count;
        key_compare = rhs.key_compare;
    }
    return *this;
}

// move assignment
template <class T, class Compare>
rb_tree<T, Compare>& rb_tree<T, Compare>::operator=(rb_tree&& rhs) {
    if (this != &rhs) {
        clear();
        header = rhs.header;
        node_count = rhs.node_count;
        key_compare = rhs.key_compare;
        rhs.reset();
    }
    return *this;
}

// erase
// clear all node
template <class T, class Compare>
void clear() {
    if (node_count != 0) {
        erase_subtree(root());
        root() = nullptr;
        leftmost() = header;
        rightmost() = header;
        node_count = 0;
    }
}

// rb_tree operation
// swap
template <class T, class Compare>
void rb_tree<T, Compare>::swap(rb_tree& rhs) {
    if (this != &rhs) {
        mystl::swap(header, rhs.header);
        mystl::swap(node_count, rhs.node_count);
        mystl::swap(key_compare, rhs.key_compare);
    }
}

// global overload ...
// overload swap
template <class T, class Compare>
void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) {
    lhs.swap(rhs);
}

// overload compare operator
template <class T, class Compare>
bool operator==(const rb_tree<T, Compare>& lhs,
                const rb_tree<T, Compare>& rhs) {
    return lhs.size() == rhs.size() &&
           mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class Compare>
bool operator!=(const rb_tree<T, Compare>& lhs,
                const rb_tree<T, Compare>& rhs) {
    return !(lhs == rhs);
}

template <class T, class Compare>
bool operator<(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return stl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                        rhs.end());
}

template <class T, class Compare>
bool operator<=(const rb_tree<T, Compare>& lhs,
                const rb_tree<T, Compare>& rhs) {
    return !(rhs < lhs);
}

template <class T, class Compare>
bool operator>(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs) {
    return rhs < lhs;
}

template <class T, class Compare>
bool operator>=(const rb_tree<T, Compare>& lhs,
                const rb_tree<T, Compare>& rhs) {
    return !(lhs < rhs);
}

}  // namespace mystl