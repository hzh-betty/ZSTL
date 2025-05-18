#pragma once
#include "../iterator/iterator.hpp"
#include <type_traits>

namespace zstl
{
    // 通用 max 函数
    template <typename T>
    const T &max(const T &a, const T &b)
    {
        return a > b ? a : b;
    }

    // 通用 min 函数
    template <typename T>
    const T &min(const T &a, const T &b)
    {
        return a < b ? a : b;
    }

    // 交换函数
    template <typename T>
    void swap(T &a, T &b)
    {
        T tmp = std::move(a);
        a = std::move(b);
        b = std::move(tmp);
    }

    // 填充函数
    template <typename ForwardIt, typename T>
    void fill(ForwardIt first, ForwardIt last, const T &value)
    {
        for (; first != last; ++first)
        {
            *first = value;
        }
    }

    // 填充函数
    template <typename OutputIt, typename Size, typename T>
    OutputIt fill_n(OutputIt first, Size n, const T &value)
    {
        for (size_t i = 0; i < n; ++i)
        {
            *first++ = value;
        }
        return first;
    }

    // 优化：对 trivial 类型使用 memset
    template <typename T, typename Size, typename U>
    std::enable_if_t<
        std::is_same_v<std::remove_const_t<T>, U> &&
            std::is_trivially_copy_assignable_v<U> &&
            sizeof(U) == 1,
        T *>
    fill_n(T *first, Size n, U value)
    {
        if (n > 0)
            std::memset(first, static_cast<unsigned char>(value), n);
        return first + n;
    }

    // 功能：拷贝范围内的元素到目标位置
    template <typename InputIt, typename OutputIt>
    OutputIt copy(InputIt first, InputIt last, OutputIt dest)
    {
        if constexpr (is_random_access_iterator_v<InputIt>)
        {
            auto n = last - first;
            for (size_t i = 0; i < n; ++i)
            {
                *dest++ = *first++;
            }
            return dest;
        }
        else
        {
            for (; first != last; ++first, ++dest)
            {
                *dest = *first;
            }
            return dest;
        }
    }

    // 特化优化：可平凡复制的类型
    template <typename T>
    std::enable_if_t<
        std::is_trivially_copyable_v<T>,
        T *>
    copy(const T *first, const T *last, T *result)
    {
        const auto n = static_cast<size_t>(last - first);
        if (n)
            std::memmove(result, first, n * sizeof(T));
        return result + n;
    }

    // 功能：比较两个范围的元素是否相等
    // 迭代器版本
    template <typename InputIter1, typename InputIter2>
    bool equal(InputIter1 first1, InputIter1 last1, InputIter2 first2)
    {
        while (first1 != last1)
        {
            if (!(*first1 == *first2))
                return false;
            ++first1;
            ++first2;
        }
        return true;
    }

    // 带比较函数的版本
    template <typename InputIter1, typename InputIter2, typename Compare>
    bool equal(InputIter1 first1, InputIter1 last1,
               InputIter2 first2, Compare comp)
    {
        while (first1 != last1)
        {
            if (!comp(*first1, *first2))
                return false;
            ++first1;
            ++first2;
        }
        return true;
    }

    // 优化：内存直接比较
    template <typename T>
    std::enable_if_t<
        std::is_trivially_copyable_v<T>,
        bool>
    equal(const T *first1, const T *last1, const T *first2)
    {
        const auto n = last1 - first1;
        return std::memcmp(first1, first2, n * sizeof(T)) == 0;
    }

    // 字典序比较
    template <typename InputIter1, typename InputIter2>
    bool lexicographical_compare(InputIter1 first1, InputIter1 last1,
                                 InputIter2 first2, InputIter2 last2)
    {
        while (first1 != last1)
        {
            // 迭代器到达末尾或者first1大于first2
            if (first2 == last2 || *first2 < *first1)
                return false;
            // 如果first2大于first1
            else if (*first1 < *first2)
                return true;
            ++first1;
            ++first2;
        }
        // 迭代器到达末尾，判断first2是否到达末尾
        return (first2 != last2);
    }

    // 优化：内存块比较
    template <typename T, typename U>
    std::enable_if_t<
        std::is_same_v<std::remove_const_t<T>, U> &&
            std::is_trivially_copyable_v<U>,
        bool>
    lexicographical_compare(const T *first1, const T *last1,
                            const U *first2, const U *last2)
    {
        const auto len1 = last1 - first1;
        const auto len2 = last2 - first2;
        const int result = std::memcmp(first1, first2, std::min(len1, len2));
        return result < 0 || (result == 0 && len1 < len2);
    }

    // 计数函数
    template <typename InputIter, typename T>
    typename iterator_traits<InputIter>::difference_type count(InputIter first, InputIter last, const T &value)
    {
        typename iterator_traits<InputIter>::difference_type n = 0;
        for (; first != last; ++first)
            if (*first == value)
                ++n;
        return n;
    }

    // 逆置，通用双向迭代器版本
    template <class BidirIt>
    void reverse(BidirIt first, BidirIt last)
    {
        static_assert(is_bidirectional_iterator_v<BidirIt>,
                      "reverse requires bidirectional iterators");
        while ((first != last) && (first != --last))
        {
            swap(*first, *last);
            ++first;
        }
    }

} // namespace zstl
