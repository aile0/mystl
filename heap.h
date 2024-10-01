#pragma once

// 定义算法，实现可排序容器的heap排序
// 默认是大顶堆，即堆顶是最大值

#include "iterator.h"

namespace mystl
{

    // heap的swim实现
    template <typename T, typename RandomAccessIterator, typename Distance>
    void swim_aux(RandomAccessIterator first, Distance top, Distance index, const T &value)
    {
        Distance parent = (index - 1) / 2;
        while (index > top && *(first + parent) < value)
        {
            *(first + index) = *(first + parent);
            index = parent;
            parent = (index - 1) / 2;
        }
        *(first + index) = value;
    }

    // 保证*(last-1)是已经插入的值，此时需要swim
    template <typename RandomAccessIterator>
    void swim(RandomAccessIterator first, RandomAccessIterator last)
    {
        auto value = *(last - 1);
        auto index = last - first - 1;
        decltype(index) top = 0;
        swim_aux(first, top, index, value);
    }

    // heap的sink实现，end之前的元素进行有序化
    template <typename T, typename RandomAccessIterator, typename Distance>
    void sink_aux(RandomAccessIterator first, Distance end, Distance index, const T &value)
    {
        Distance top = index;
        Distance left_child = 2 * index + 1;
        Distance right_child = 2 * index + 2;
        while (right_child < end)
        {
            if (*(first + left_child) < *(first + right_child))
            {
                *(first + index) = *(first + right_child);
                index = right_child;
                left_child = 2 * index + 1;
                right_child = 2 * index + 2;
            }
            else
            {
                *(first + index) = *(first + left_child);
                index = left_child;
                left_child = 2 * index + 1;
                right_child = 2 * index + 2;
            }
        }
        if (right_child == end)
        {
            *(first + index) = *(first + left_child);
            index = left_child;
        }
        swim_aux(first, top, index, value);
    }

    // 将最大值放置在尾部，最小值放置在首部sink，保证*(last-1)是需要pop的值
    template <typename RandomAccessIterator>
    void sink(RandomAccessIterator first, RandomAccessIterator last)
    {
        auto value = *(last - 1);
        *(last - 1) = *first;
        auto end = last - first - 1;
        decltype(end) index = 0;
        sink_aux(first, end, index, value);
    }

    // 将可排序容器中元素递增排序
    template <typename RandomAccessIterator>
    void sort_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        for (; last - first > 1; --last)
        {
            sink(first, last);
        }
    }

    // 将容器构建大顶堆
    template <typename RandomAccessIterator>
    void make_heap(RandomAccessIterator first, RandomAccessIterator last)
    {
        auto end = last - first;
        decltype(end) index = (last - first - 1) / 2;
        for (; index >= 0; --index)
        {
            sink_aux(first, end, index, *(first + index));
        }
    }
}