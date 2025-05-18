#pragma once
#include "algo.hpp"
#include "../iterator/iterator.hpp"

namespace zstl
{
    // ======================== push_heap =========================

    // 上浮调整（最大堆）
    template <class RandomIt, class Compare>
    void adjust_up(RandomIt first, RandomIt last, Compare comp)
    {
        if (last - first < 2)
            return;
        auto child = last - first - 1;
        auto parent = (child - 1) / 2;
        while (child > 0 && comp(*(first + parent), *(first + child)))
        {
            zstl::swap(*(first + child), *(first + parent));
            child = parent;
            parent = (child - 1) / 2;
        }
    }

    template <class RandomIt>
    void push_heap(RandomIt first, RandomIt last)
    {
        zstl::adjust_up(first, last, std::less<typename zstl::iterator_traits<RandomIt>::value_type>());
    }

    template <class RandomIt, class Compare>
    void push_heap(RandomIt first, RandomIt last, Compare comp)
    {
        static_assert(is_random_access_iterator_v<RandomIt>,
                      "push_heap requires random access iterators");
        zstl::adjust_up(first, last, comp);
    }

    // ======================== pop_heap =========================

    // 下沉调整（最大堆）
    template <class RandomIt, class Compare>
    void adjust_down(RandomIt first, RandomIt last, size_t parent, Compare comp)
    {
        auto n = last - first;
        auto child = 2 * parent + 1;
        while (child < n)
        {
            // 选择较大的子节点
            if (child + 1 < n && comp(*(first + child), *(first + child + 1)))
                ++child;
            if (comp(*(first + parent), *(first + child)))
            {
                zstl::swap(*(first + parent), *(first + child));
                parent = child;
                child = 2 * parent + 1;
            }
            else
            {
                break;
            }
        }
    }

    template <class RandomIt>
    void pop_heap(RandomIt first, RandomIt last)
    {
        using T = typename zstl::iterator_traits<RandomIt>::value_type;
        if (last - first < 2)
            return;
        zstl::swap(*first, *(last - 1));
        zstl::adjust_down(first, last - 1, 0, std::less<T>());
    }

    template <class RandomIt, class Compare>
    void pop_heap(RandomIt first, RandomIt last, Compare comp)
    {
        static_assert(is_random_access_iterator_v<RandomIt>,
                      "pop_heap requires random access iterators");
        if (last - first < 2)
            return;
        zstl::swap(*first, *(last - 1));
        zstl::adjust_down(first, last - 1, 0, comp);
    }

    // ======================== make_heap =========================
    // 向下调整建堆O(N)，效率更
    template <class RandomIt, class Compare>
    void make_heap(RandomIt first, RandomIt last, Compare comp)
    {
        auto n = last - first;
        if (n < 2)
            return;
        for (auto parent = (n - 2) / 2 + 1; parent > 0; --parent)
        {
            zstl::adjust_down(first, last, parent - 1, comp);
        }
    }

    template <class RandomIt>
    void make_heap(RandomIt first, RandomIt last)
    {
        static_assert(is_random_access_iterator_v<RandomIt>,
                      "make_heap requires random access iterators");
        zstl::make_heap(first, last, std::less<typename zstl::iterator_traits<RandomIt>::value_type>());
    }

    // ======================== sort_heap =========================

    template <class RandomIt, class Compare>
    void sort_heap(RandomIt first, RandomIt last, Compare comp)
    {
        while (last - first > 1)
        {
            static_assert(is_random_access_iterator_v<RandomIt>,
                          "sort_heap requires random access iterators");
            // 交换堆顶元素和最后一个元素
            zstl::pop_heap(first, last--, comp);
        }
    }

    template <class RandomIt>
    void sort_heap(RandomIt first, RandomIt last)
    {
        zstl::sort_heap(first, last, std::less<typename zstl::iterator_traits<RandomIt>::value_type>());
    }

    // 判断[first, last)是否为最大堆
    template <class RandomIt, class Compare>
    bool is_heap(RandomIt first, RandomIt last, Compare comp)
    {
        auto n = last - first;
        for (auto parent = 0; parent < n; ++parent)
        {
            auto left = 2 * parent + 1;
            auto right = 2 * parent + 2;
            if (left < n && comp(*(first + parent), *(first + left)))
                return false;
            if (right < n && comp(*(first + parent), *(first + right)))
                return false;
        }
        return true;
    }

    template <class RandomIt>
    bool is_heap(RandomIt first, RandomIt last)
    {
        using T = typename zstl::iterator_traits<RandomIt>::value_type;
        return zstl::is_heap(first, last, std::less<T>());
    }
} // namespace zstl