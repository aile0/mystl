#pragma once

#include "rb_tree.h"
#include "rb_tree_color.h"
#include "util.h"

// find the minimum node under x
template <class Node_ptr>
Node_ptr rb_tree_minimum(Node_ptr ptr) {
    while (ptr->left != nullptr) ptr = ptr->left;
    return ptr;
}
// find the maximum node under x
template <class Node_ptr>
Node_ptr rb_tree_maximum(Node_ptr ptr) {
    while (ptr->right != nullptr) ptr = ptr->right;
    return ptr;
}
/********************************************************** */
// judge whether x is the root
template <class Node_ptr>
bool rb_tree_is_root(Node_ptr x) {
    return x->parent->parent == x;
}
// judge whether x is the left child of its parent
template <class Node_ptr>
bool rb_tree_is_left_child(Node_ptr x) {
    return x->parent->left == x;
}

// judge whether x is the right child of its parent
template <class Node_ptr>
bool rb_tree_is_right_child(Node_ptr x) {
    return x->parent->right == x;
}
/********************************************************** */
// judge whether x is red
template <class Node_ptr>
bool rb_tree_is_red(Node_ptr x) {
    return x != nullptr && x->color == rb_tree_color_red;
}
// judge whether x is black
template <class Node_ptr>
bool rb_tree_is_black(Node_ptr x) {
    return x == nullptr || x->color == rb_tree_color_black;
}
/*********************************************************** */
// set color val to x
template <class Node_ptr, class rb_tree_color>
void rb_tree_set_color(Node_ptr ptr, rb_tree_color color) {
    ptr->color = color;
}
// set x to red
template <class Node_ptr>
void rb_tree_set_red(Node_ptr x) {
    x->color = rb_tree_color_red;
}
// set x to black
template <class Node_ptr>
void rb_tree_set_black(Node_ptr x) {
    x->color = rb_tree_color_black;
}
/************************************************************ */
// previous node
template <class Node_ptr>
Node_ptr rb_tree_previous(Node_ptr x) {
    if (x->left != nullptr) return rb_tree_maximum(x->left);
    while (!rb_tree_is_right_child(x)) {
        if (rb_tree_is_root(x)) return nullptr;
        x = x->parent;
    }
    return x->parent;
}
// next node
template <class Node_ptr>
Node_ptr rb_tree_next(Node_ptr x) {
    if (x->right != nullptr) return rb_tree_minimum(x->right);
    while (rb_tree_is_right_child(x)) {
        if (rb_tree_is_root(x)) return nullptr;
        x = x->parent;
    }
    return x->parent;
}
/************************************************************ */
// rotate left
template <class Node_ptr>
void rb_tree_rotate_left(Node_ptr x, Node_ptr& root) {
    Node_ptr y = x->right;
    x->right = y->left;
    if (y->left != nullptr) y->left->parent = x;
    y->parent = x->parent;
    if (x == root)
        root = y;
    else if (rb_tree_is_left_child(x))
        x->parent->left = y;
    else
        x->parent->right = y;
    y->left = x;
    x->parent = y;
}
// rotate right
template <class Node_ptr>
void rb_tree_rotate_right(Node_ptr x, Node_ptr& root) {
    Node_ptr y = x->left;
    x->left = y->right;
    if (y->right != nullptr) y->right->parent = x;
    y->parent = x->parent;
    if (x == root)
        root = y;
    else if (rb_tree_is_left_child(x))
        x->parent->left = y;
    else
        x->parent->right = y;
    y->right = x;
    x->parent = y;
}
/************************************************************ */
// rebalance after insertion
// x already be inserted, now rebalance
template <class Node_ptr>
void rb_tree_insert_rebalance(Node_ptr x, Node_ptr& root) {
    rb_tree_set_red(x);  // red color
    while (
        x != root &&
        rb_tree_is_red(x->parent)) {  // current node is red and parent is red
        if (rb_tree_is_left_child(x->parent)) {  // parent is left child
            auto y = x->parent->parent->right;
            if (y != nullptr && rb_tree_is_red(y)) {  // uncle is red
                rb_tree_set_black(x->parent);
                rb_tree_set_black(y);
                x = x->parent->parent;
                rb_tree_set_red(x);
            } else {  // uncle is black
                if (rb_tree_is_right_child(x)) {
                    x = x->parent;
                    rb_tree_rotate_left(x, root);
                }
                rb_tree_set_black(x->parent);
                rb_tree_set_red(x->parent->parent);
                rb_tree_rotate_right(x->parent->parent, root);
                break;
            }
        } else {  // parent is right child
            auto y = x->parent->parent->left;
            if (y != nullptr && rb_tree_is_red(y)) {  // uncle is red
                rb_tree_set_black(x->parent);
                rb_tree_set_black(y);
                x = x->parent->parent;
                rb_tree_set_red(x);
            } else {  // uncle is black
                if (rb_tree_is_left_child(x)) {
                    x = x->parent;
                    rb_tree_rotate_right(x, root);
                }
                rb_tree_set_black(x->parent);
                rb_tree_set_red(x->parent->parent);
                rb_tree_rotate_left(x->parent->parent, root);
                break;
            }
        }
    }
    rb_tree_set_black(root);  // root is black
}

// rebalance before deletion
// x is the node to be deleted
// return ptr to the node to be deleted
template <class Node_ptr>
Node_ptr rb_tree_erase_rebalance(Node_ptr x, Node_ptr& root) {
    Node_ptr res = x;  // res(x) is the node to be deleted
    // delete x before rebalance
    if (rb_tree_is_black(x)) {  // x is black(red node can be deleted directly)
        while (x != root && x != nullptr) {
            if (rb_tree_is_left_child(x)) {  // x is left child
                auto w = x->parent->right;   // w is the sibling of x
                if (rb_tree_is_red(w)) {     // sibling is red
                    rb_tree_set_black(w);
                    rb_tree_set_red(x->parent);
                    rb_tree_rotate_left(x->parent, root);
                    w = x->parent->right;
                }
                if (rb_tree_is_black(w->left) &&
                    rb_tree_is_black(
                        w->right)) {  // sibling's children are black
                    rb_tree_set_red(w);
                    x = x->parent;
                } else {
                    if (rb_tree_is_black(w->right)) {
                        rb_tree_set_black(w->left);
                        rb_tree_set_red(w);
                        rb_tree_rotate_right(w, root);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    rb_tree_set_black(x->parent);
                    rb_tree_set_black(w->right);
                    rb_tree_rotate_left(x->parent, root);
                    break;
                }
            } else {  // x is right child
                auto w = x->parent->left;
                if (rb_tree_is_red(w)) {
                    rb_tree_set_black(w);
                    rb_tree_set_red(x->parent);
                    rb_tree_rotate_right(x->parent, root);
                    w = x->parent->left;
                }
                if (rb_tree_is_black(w->left) && rb_tree_is_black(w->right)) {
                    rb_tree_set_red(w);
                    x = x->parent;
                } else {
                    if (rb_tree_is_black(w->left)) {
                        rb_tree_set_black(w->right);
                        rb_tree_set_red(w);
                        rb_tree_rotate_left(w, root);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    rb_tree_set_black(x->parent);
                    rb_tree_set_black(w->left);
                    rb_tree_rotate_right(x->parent, root);
                    break;
                }
            }
        }
    }
    auto x_child = res->left != nullptr ? res->left : res->right;
    if (x_child == nullptr) {
        if (rb_tree_is_left_child(res)) {
            res->parent->left = nullptr;
        } else {
            res->parent->right = nullptr;
        }
    } else {
        x_child->parent = res->parent;
        if (rb_tree_is_left_child(res)) {
            res->parent->left = x_child;
        } else {
            res->parent->right = x_child;
        }
    }
    if (res == root) root = x_child;
    return res;
}
