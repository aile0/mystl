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
    size_t elem_size;
    size_t elem_count;
    std::vector<void *> free_vector;
    std::vector<void *> all_blocks;
    // free-all
    std::unordered_map<void *, void *> used_map;
    std::unordered_map<void *, size_t> available_elem;
    std::unordered_map<void *, size_t> used_elem;

    MemoryPool() = default;
    MemoryPool(size_t size, size_t count) {
        elem_count = count;
        elem_size = size;
        expand();
    }

    ~MemoryPool() {
        for (size_t i = 0; i < all_blocks.size(); ++i) {
            ::free(all_blocks[i]);
        }
    }

    void *allocate() {
        if (free_vector.empty()) {
            expand();
        }
        void *result = free_vector.back();
        free_vector.pop_back();
        available_elem[used_map[result]]--;
        used_elem[used_map[result]]++;
        return result;
    }

    void deallocate(void *p) {
        free_vector.push_back(p);
        available_elem[used_map[p]]++;
        used_elem[used_map[p]]--;

        void *bptr = used_map[p];
        if (used_elem[bptr] == 0 && all_blocks.size() > 1) {
            free_vector.erase(
                std::remove_if(free_vector.begin(), free_vector.end(),
                               [&](void *p) { return used_map[p] == bptr; }),
                free_vector.end());
            for (auto it = used_map.begin(); it != used_map.end(); ++it) {
                if (it->second == bptr) {
                    it = used_map.erase(it);
                } else {
                    ++it;
                }
            }
            all_blocks.erase(
                std::remove(all_blocks.begin(), all_blocks.end(), bptr),
                all_blocks.end());
            available_elem.erase(bptr);
            used_elem.erase(bptr);
            ::free(bptr);
        }
    }

   private:
    void expand() {
        void *block = malloc_alloc::allocate(elem_size * elem_count);
        for (int i = 0; i < elem_count; ++i) {
            free_vector.push_back((char *)block + i * elem_size);
            used_map[free_vector.back()] = block;
        }
        all_blocks.push_back(block);
        available_elem[block] = elem_count;
        used_elem[block] = 0;
    }
};

// 定义多大小分配器
class MemoryPoolManager {
   public:
    MemoryPoolManager() = default;
    ~MemoryPoolManager() = default;
    static void *allocate(size_t size) {
        if (size > MAX_BYTES) {
            return malloc_alloc::allocate(size);
        }
        size_t n = round_up(size);
        if (pool_map.find(n) == pool_map.end()) {
            pool_map[n] = MemoryPool(n, ELEM_COUNT);
        }
        return pool_map[n].allocate();
    }

    static void deallocate(void *p, size_t size) {
        if (size > MAX_BYTES) {
            malloc_alloc::deallocate(p, size);
            return;
        }
        pool_map[round_up(size)].deallocate(p);
    }

    static void reallocate(void *p, size_t old_sz, size_t new_sz) {
        void *result = nullptr;
        if (new_sz > MAX_BYTES) {
            result = malloc_alloc::allocate(new_sz);
        } else {
            result = allocate(new_sz);
        }
        size_t copy_sz = old_sz < new_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        if (old_sz > MAX_BYTES) {
            malloc_alloc::deallocate(p, old_sz);
        } else {
            deallocate(p, old_sz);
        }
    }

   private:
    static constexpr size_t ALIGN = 8;
    static constexpr size_t MAX_BYTES = 128;
    static constexpr size_t ELEM_COUNT = 20;
    static std::unordered_map<size_t, MemoryPool> pool_map;

    static size_t round_up(size_t n) {
        return (n + ALIGN - 1) & ~size_t(ALIGN - 1);
    }
};

// // 定义default_alloc
// class default_alloc {
//    public:
//     static void *allocate(size_t n);
//     static void deallocate(void *p, size_t n);
//     static void *reallocate(void *p, size_t old_sz, size_t new_sz);

//    private:
//     static size_t round_up(size_t n);
//     static size_t freelist_index(size_t bytes);

//     static void *refill(size_t n);
//     static void *chunk_alloc(size_t n, size_t &nobjs);

//     static constexpr size_t ALIGN = 8;
//     static constexpr size_t MAX_BYTES = 128;
//     static constexpr size_t NFREELIST = 17;
//     static constexpr size_t MAX_COUNT = 100;

//     union Obj {
//         Obj *next;
//         char data[1];
//     };

//     static char *start_free;
//     static char *end_free;
//     static size_t heap_size;

//     static Obj *free_list[NFREELIST];
//     static int free_node_count[NFREELIST];
// };
// // 静态变量初始化
// char *default_alloc::start_free = nullptr;
// char *default_alloc::end_free = nullptr;
// size_t default_alloc::heap_size = 0;
// default_alloc::Obj *default_alloc::free_list[default_alloc::NFREELIST] = {
//     nullptr};
// int default_alloc::free_node_count[default_alloc::NFREELIST] = {0};

// size_t default_alloc::round_up(size_t n) {
//     return (n + ALIGN - 1) & ~size_t(ALIGN - 1);
// }

// // bytes为8的倍数
// size_t default_alloc::freelist_index(size_t bytes) { return bytes / ALIGN; }

// // 尝试从内存池中分配
// void *default_alloc::chunk_alloc(size_t n, size_t &nobjs) {
//     size_t total = n * nobjs;
//     size_t bytes_left = static_cast<size_t>(end_free - start_free);

//     if (bytes_left >= total) {
//         void *result = start_free;
//         start_free += total;
//         return result;
//     } else if (bytes_left >= n) {
//         nobjs = bytes_left / n;
//         total = n * nobjs;
//         void *result = start_free;
//         start_free += total;
//         return result;
//     } else {
//         size_t bytes_to_get = 2 * total;
//         if (bytes_left > 0) {
//             Obj *next_obj = (Obj *)start_free;
//             size_t index = freelist_index(round_up(bytes_left));
//             next_obj->next = free_list[index];
//             free_list[index] = next_obj;

//             free_node_count[index]++;
//             if (free_node_count[index] >= MAX_COUNT) {
//                 Obj *obj = free_list[index];
//                 Obj *next_obj = obj->next;
//                 while (next_obj != nullptr) {
//                     obj = next_obj;
//                     next_obj = obj->next;
//                     free_node_count[index]--;
//                     ::free(obj);
//                 }
//             }
//         }
//         start_free = (char *)malloc(bytes_to_get);
//         if (start_free == nullptr) {
//             size_t index = freelist_index(round_up(bytes_left));
//             while (free_list[index] == nullptr) {
//                 index = (index + 1) % NFREELIST;
//             }
//             Obj *next_obj = free_list[index];
//             start_free = (char *)next_obj;
//             end_free = start_free + (index + 1) * 8;
//             free_list[index] = next_obj->next;
//             return chunk_alloc(n, nobjs);
//         } else {
//             heap_size += bytes_to_get;
//             end_free = start_free + bytes_to_get;
//             return chunk_alloc(n, nobjs);
//         }
//     }
// }

// // 填充freelist
// void *default_alloc::refill(size_t n) {
//     size_t nobjs = 20;
//     void *chunk = chunk_alloc(n, nobjs);
//     free_node_count[freelist_index(n)] += nobjs;
//     Obj *cur_obj = (Obj *)chunk;
//     Obj *next_obj = nullptr;
//     for (size_t i = 1; i < nobjs; ++i) {
//         next_obj = (Obj *)((char *)cur_obj + n);
//         cur_obj->next = next_obj;
//         cur_obj = next_obj;
//     }
//     cur_obj->next = nullptr;
//     return chunk;
// }

// /**
//  * @brief Allocate a block of memory of size n.
//  * @details If n is larger than MAX_BYTES, call malloc_alloc::allocate.
//  * Otherwise, try to find a free block in the free list.
//  * If no free block is found, call refill to fill the free list.
//  * @return A pointer to the allocated memory block.
//  */
// void *default_alloc::allocate(size_t n) {
//     if (n > MAX_BYTES) {
//         return malloc_alloc::allocate(n);
//     }
//     size_t index = freelist_index(round_up(n));
//     Obj *obj = free_list[index];
//     if (obj == nullptr) {
//         obj = (Obj *)refill(round_up(n));
//     }
//     free_list[index] = obj->next;
//     free_node_count[index]--;
//     return static_cast<void *>(obj);
// }

// void default_alloc::deallocate(void *p, size_t n) {
//     if (n > MAX_BYTES) {
//         malloc_alloc::deallocate(p, n);
//     } else {
//         size_t index = freelist_index(round_up(n));
//         Obj *obj = static_cast<Obj *>(p);
//         obj->next = free_list[index];
//         free_list[index] = obj;

//         free_node_count[index]++;
//         if (free_node_count[index] >= MAX_COUNT) {
//             Obj *obj = free_list[index];
//             Obj *next_obj = obj->next;
//             while (next_obj != nullptr) {
//                 obj = next_obj;
//                 next_obj = obj->next;
//                 free_node_count[index]--;
//                 ::free(obj);
//             }
//         }
//     }
// }

// void *default_alloc::reallocate(void *p, size_t old_sz, size_t new_sz) {
//     void *result = allocate(new_sz);
//     size_t copy_sz = old_sz < new_sz ? old_sz : new_sz;
//     memcpy(result, p, copy_sz);
//     deallocate(p, old_sz);
//     return result;
// }

// 第一个分配器模板
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
