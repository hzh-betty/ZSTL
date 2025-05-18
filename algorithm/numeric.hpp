#pragma once
#include "../iterator/iterator.hpp"
namespace zstl
{

    // 累加区间 [first, last) 的值，初始值为init
    template <typename InputIt, typename T>
    T accumulate(InputIt first, InputIt last, T init)
    {
        for (; first != last; ++first)
            init += *first;
        return init;
    }

    // 带二元操作的累加
    template <typename InputIt, typename T, typename BinaryOp>
    T accumulate(InputIt first, InputIt last, T init, BinaryOp op)
    {
        for (; first != last; ++first)
            init = op(init, *first);
        return init;
    }

    // 计算相邻元素的差，结果写入 result
    template <typename InputIt, typename OutputIt>
    OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt result)
    {
        if (first == last)
            return result;
        using value_type = typename iterator_traits<InputIt>::value_type;
        value_type prev = *first;
        *result++ = prev;
        while (++first != last)
        {
            value_type curr = *first;
            *result++ = curr - prev;
            prev = curr;
        }
        return result;
    }

    // 带二元操作的 adjacent_difference
    template <typename InputIt, typename OutputIt, typename BinaryOp>
    OutputIt adjacent_difference(InputIt first, InputIt last, OutputIt result, BinaryOp op)
    {
        if (first == last)
            return result;
        using value_type = typename iterator_traits<InputIt>::value_type;
        value_type prev = *first;
        *result++ = prev;
        while (++first != last)
        {
            value_type curr = *first;
            *result++ = op(curr, prev);
            prev = curr;
        }
        return result;
    }

    // 计算区间的内积
    template <typename InputIt1, typename InputIt2, typename T>
    T inner_product(InputIt1 first1, InputIt1 last1, InputIt2 first2, T init)
    {
        for (; first1 != last1; ++first1, ++first2)
            init += (*first1) * (*first2);
        return init;
    }

    // 带自定义操作的内积
    template <typename InputIt1, typename InputIt2, typename T, typename BinaryOp1, typename BinaryOp2>
    T inner_product(InputIt1 first1, InputIt1 last1, InputIt2 first2, T init, BinaryOp1 op1, BinaryOp2 op2)
    {
        for (; first1 != last1; ++first1, ++first2)
            init = op1(init, op2(*first1, *first2));
        return init;
    }

    // 计算部分和，结果写入 result
    template <typename InputIt, typename OutputIt>
    OutputIt partial_sum(InputIt first, InputIt last, OutputIt result)
    {
        if (first == last)
            return result;
        using value_type = typename iterator_traits<InputIt>::value_type;
        value_type sum = *first;
        *result++ = sum;
        while (++first != last)
        {
            sum += *first;
            *result++ = sum;
        }
        return result;
    }

    // 带二元操作的 partial_sum
    template <typename InputIt, typename OutputIt, typename BinaryOp>
    OutputIt partial_sum(InputIt first, InputIt last, OutputIt result, BinaryOp op)
    {
        if (first == last)
            return result;
        using value_type = typename iterator_traits<InputIt>::value_type;
        value_type sum = *first;
        *result++ = sum;
        while (++first != last)
        {
            sum = op(sum, *first);
            *result++ = sum;
        }
        return result;
    }

    // 生成等差数列 [first, last)，初值为value，每次递增1
    template <typename ForwardIt, typename T>
    void iota(ForwardIt first, ForwardIt last, T value)
    {
        while (first != last)
        {
            *first = value;
            ++first;
            ++value;
        }
    }
}