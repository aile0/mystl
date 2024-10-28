#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <memory>
#include <new>
#include <numeric>
#include <unordered_map>
#include <vector>

#include "construct.h"
#include "iterator.h"
#include "type_traits.h"

namespace mystl {

// 定义第一个分配器
class malloc_alloc {
   public:
    static void *allocate(size_t n);
    static void deallocate(void *p, size_t n);
    static void *reallocate(void *p, size_t old_sz, size_t new_sz);

   private:
    using FunPtr = void (*)();
    static FunPtr malloc_alloc_oom_handler;

    static void *oom_malloc(size_t);
    static void *oom_realloc(void *, size_t, size_t);

    static FunPtr set_malloc_handler(FunPtr);
};

void *malloc_alloc::oom_malloc(size_t n) {
    if (malloc_alloc_oom_handler == nullptr) {
        std::cout << "oom_malloc failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    void *result = nullptr;
    while (true) {
        malloc_alloc_oom_handler();
        result = malloc_alloc::allocate(n);
        if (result) {
            return result;
        }
    }
}

void *malloc_alloc::oom_realloc(void *p, size_t old_sz, size_t new_sz) {
    if (malloc_alloc_oom_handler == nullptr) {
        std::cout << "oom_realloc failed" << std::endl;
        exit(EXIT_FAILURE);
    }
    void *result = nullptr;
    while (true) {
        malloc_alloc_oom_handler();
        result = malloc_alloc::reallocate(p, old_sz, new_sz);
        if (result) {
            return result;
        }
    }
}

malloc_alloc::FunPtr malloc_alloc::set_malloc_handler(FunPtr f) {
    FunPtr old = malloc_alloc_oom_handler;
    malloc_alloc_oom_handler = f;
    return old;
}

void *malloc_alloc::allocate(size_t n) {
    void *result = ::malloc(n);
    if (result == nullptr) {
        result = oom_malloc(n);
    }
    return result;
}

void *malloc_alloc::reallocate(void *p, size_t old_sz, size_t new_sz) {
    void *result = ::realloc(p, new_sz);
    if (result == nullptr) {
        result = oom_realloc(p, old_sz, new_sz);
    }
    return result;
}

void malloc_alloc::deallocate(void *p, size_t n) { ::free(p); }
// 初始化静态变量
void (*malloc_alloc::malloc_alloc_oom_handler)(void) = nullptr;

// 定义单一大小内存分配器
class MemoryPool {
   public:
    class FreeNode {
       public:
        FreeNode *next;
    };

    size_t elem_size;
    size_t elem_count;
    FreeNode *free_list;
    void **all_blocks;
    size_t all_blocks_count;
    size_t max_blocks_count;

    MemoryPool(size_t elem_size, size_t elem_count, size_t max_blocks_count)
        : elem_size(elem_size),
          elem_count(elem_count),
          free_list(nullptr),
          all_blocks(nullptr),
          all_blocks_count(0),
          max_blocks_count(max_blocks_count) {
        all_blocks = new void *[max_blocks_count];
        expand();
    }

    void *allocate() {
        if (free_list == nullptr) {
            expand();
        }
        FreeNode *node = free_list;
        free_list = free_list->next;
        return static_cast<void *>(node);
    }

    void deallocate(void *p) {
        FreeNode *node = static_cast<FreeNode *>(p);
        node->next = free_list;
        free_list = node;

        for (size_t i = 0; i < all_blocks_count; i++) {
            if (is_in_block(p, all_blocks[i])) {
                size_t free_count = 0;
                for (FreeNode *cur = free_list; cur != nullptr;
                     cur = cur->next) {
                    if (is_in_block(cur, all_blocks[i])) {
                        free_count++;
                    }
                }
                if (free_count == elem_count) {
                    FreeNode *cur = free_list;
                    while (is_in_block(cur, all_blocks[i])) {
                        cur = cur->next;
                    }
                    free_list = cur;
                    FreeNode *next = cur->next;
                    while (next != nullptr) {
                        if (is_in_block(next, all_blocks[i])) {
                            cur->next = next->next;
                            next = cur->next;
                        } else {
                            cur = next;
                            next = next->next;
                        }
                    }

                    ::free(all_blocks[i]);
                    all_blocks[i] = all_blocks[--all_blocks_count];
                    break;
                }
            }
        }
    }

   private:
    void expand() {
        if (all_blocks_count == max_blocks_count) {
            max_blocks_count *= 2;
            void **new_blocks = new void *[max_blocks_count];
            std::copy(all_blocks, all_blocks + all_blocks_count, new_blocks);
            delete[] all_blocks;
            all_blocks = new_blocks;
        }
        void *block = ::malloc(elem_size * elem_count);
        all_blocks[all_blocks_count++] = block;
        for (size_t i = 0; i < elem_count; ++i) {
            deallocate(static_cast<char *>(block) + i * elem_size);
        }
    }

    bool is_in_block(void *p, void *block) {
        return p >= block &&
               p < static_cast<char *>(block) + elem_size * elem_count;
    }
};

// 定义多大小分配器
class MemoryPoolManager {
   public:
    static void *allocate(size_t n) {
        if (n > MAX_BYTES) {
            return malloc_alloc::allocate(n);
        }
        size_t index = round_up(n) / ALIGN;
        if (pool_map[index] == nullptr) {
            pool_map[index] =
                new MemoryPool(index * ALIGN, ELEM_COUNT, MAX_BLOCKS_COUNT);
        }
        return pool_map[index]->allocate();
    }

    static void deallocate(void *p, size_t n) {
        if (n > MAX_BYTES) {
            malloc_alloc::deallocate(p, n);
        } else {
            size_t index = round_up(n) / ALIGN;
            pool_map[index]->deallocate(p);
        }
    }

    static void *reallocate(void *p, size_t old_sz, size_t new_sz) {
        void *result = allocate(new_sz);
        size_t copy_sz = old_sz < new_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz);
        return result;
    }

   private:
    static constexpr size_t ALIGN = 8;
    static constexpr size_t MAX_BYTES = 128;
    static constexpr size_t ELEM_COUNT = 20;
    static constexpr size_t MAX_BLOCKS_COUNT = 20;
    static MemoryPool *pool_map[MAX_BYTES / ALIGN + 1];

    static size_t round_up(size_t n) {
        return (n + ALIGN - 1) & ~size_t(ALIGN - 1);
    }
};
MemoryPool *MemoryPoolManager::pool_map[MAX_BYTES / ALIGN + 1] = {nullptr};

// 分配器模板
template <typename T>
class simple_alloc {
   public:
    using value_type = T;
    using pointer = T *;
    using const_pointer = const T *;
    using reference = T &;
    using const_reference = const T &;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    static T *allocate();
    static T *allocate(size_type n);

    static void deallocate(T *p);
    static void deallocate(T *p, size_type n);

    static void construct(T *p);
    static void construct(T *p, const T &value);
    static void construct(T *p, T &&value);

    static void destroy(T *p);
    static void destroy(T *first, T *last);

    static T *address(reference x) { return &x; }
    static size_t max_size() { return size_t(-1) / sizeof(T); }

    template <typename U>
    struct rebind {
        using other = simple_alloc<U>;
    };
};
// 分配内存
template <typename T>
T *simple_alloc<T>::allocate() {
    return static_cast<T *>(MemoryPoolManager::allocate(sizeof(T)));
}

template <typename T>
T *simple_alloc<T>::allocate(size_type n) {
    return static_cast<T *>(MemoryPoolManager::allocate(sizeof(T) * n));
}

template <typename T>
void simple_alloc<T>::deallocate(T *p) {
    if (p) {
        MemoryPoolManager::deallocate(p, sizeof(T));
    }
}

template <typename T>
void simple_alloc<T>::deallocate(T *p, size_type n) {
    if (p) {
        MemoryPoolManager::deallocate(p, sizeof(T) * n);
    }
}
// 构造和析构对象
template <typename T>
void simple_alloc<T>::construct(T *p) {
    mystl::construct(p);
}

template <typename T>
void simple_alloc<T>::construct(T *p, const T &value) {
    mystl::construct(p, value);
}

template <typename T>
void simple_alloc<T>::construct(T *p, T &&value) {
    mystl::construct(p, std::move(value));
}

template <typename T>
void simple_alloc<T>::destroy(T *p) {
    mystl::destroy(p);
}

template <typename T>
void simple_alloc<T>::destroy(T *first, T *last) {
    mystl::destroy(first, last);
}
}  // namespace mystl
