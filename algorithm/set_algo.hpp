#pragma once
#include "algo.hpp"
#include "../iterator/iterator.hpp"

namespace zstl
{

    // 并集：将两个有序区间的并集写入 result，返回输出尾迭代器
    template <class InputIt1, class InputIt2, class OutputIt>
    OutputIt set_union(InputIt1 first1, InputIt1 last1,
                       InputIt2 first2, InputIt2 last2,
                       OutputIt result)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (*first1 < *first2)
            {
                *result++ = *first1++;
            }
            else if (*first2 < *first1)
            {
                *result++ = *first2++;
            }
            else
            {
                *result++ = *first1++;
                ++first2;
            }
        }
        // 拷贝剩余元素
        result = zstl::copy(first1, last1, result);
        result = zstl::copy(first2, last2, result);
        return result;
    }

    // 并集：带自定义比较器
    template <class InputIt1, class InputIt2, class OutputIt, class Compare>
    OutputIt set_union(InputIt1 first1, InputIt1 last1,
                       InputIt2 first2, InputIt2 last2,
                       OutputIt result, Compare comp)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first1, *first2))
            {
                *result++ = *first1++;
            }
            else if (comp(*first2, *first1))
            {
                *result++ = *first2++;
            }
            else
            {
                *result++ = *first1++;
                ++first2;
            }
        }
        result = zstl::copy(first1, last1, result);
        result = zstl::copy(first2, last2, result);
        return result;
    }

    // 交集：将两个有序区间的交集写入 result，返回输出尾迭代器
    template <class InputIt1, class InputIt2, class OutputIt>
    OutputIt set_intersection(InputIt1 first1, InputIt1 last1,
                              InputIt2 first2, InputIt2 last2,
                              OutputIt result)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (*first1 < *first2)
            {
                ++first1;
            }
            else if (*first2 < *first1)
            {
                ++first2;
            }
            else
            {
                *result++ = *first1++;
                ++first2;
            }
        }
        return result;
    }

    // 交集：带自定义比较器
    template <class InputIt1, class InputIt2, class OutputIt, class Compare>
    OutputIt set_intersection(InputIt1 first1, InputIt1 last1,
                              InputIt2 first2, InputIt2 last2,
                              OutputIt result, Compare comp)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first1, *first2))
            {
                ++first1;
            }
            else if (comp(*first2, *first1))
            {
                ++first2;
            }
            else
            {
                *result++ = *first1++;
                ++first2;
            }
        }
        return result;
    }

    // 差集：将 first1 区间中不在 first2 区间的元素写入 result
    template <class InputIt1, class InputIt2, class OutputIt>
    OutputIt set_difference(InputIt1 first1, InputIt1 last1,
                            InputIt2 first2, InputIt2 last2,
                            OutputIt result)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (*first1 < *first2)
            {
                *result++ = *first1++;
            }
            else if (*first2 < *first1)
            {
                ++first2;
            }
            else
            {
                ++first1;
                ++first2;
            }
        }
        return zstl::copy(first1, last1, result);
    }

    // 差集：带自定义比较器
    template <class InputIt1, class InputIt2, class OutputIt, class Compare>
    OutputIt set_difference(InputIt1 first1, InputIt1 last1,
                            InputIt2 first2, InputIt2 last2,
                            OutputIt result, Compare comp)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first1, *first2))
            {
                *result++ = *first1++;
            }
            else if (comp(*first2, *first1))
            {
                ++first2;
            }
            else
            {
                ++first1;
                ++first2;
            }
        }
        return zstl::copy(first1, last1, result);
    }

    // 对称差集：将属于 S1 或 S2 但不同时属于两者的元素写入 result
    template <class InputIt1, class InputIt2, class OutputIt>
    OutputIt set_symmetric_difference(InputIt1 first1, InputIt1 last1,
                                      InputIt2 first2, InputIt2 last2,
                                      OutputIt result)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (*first1 < *first2)
            {
                *result++ = *first1++;
            }
            else if (*first2 < *first1)
            {
                *result++ = *first2++;
            }
            else
            {
                ++first1;
                ++first2;
            }
        }
        result = zstl::copy(first1, last1, result);
        result = zstl::copy(first2, last2, result);
        return result;
    }

    // 对称差集：带自定义比较器
    template <class InputIt1, class InputIt2, class OutputIt, class Compare>
    OutputIt set_symmetric_difference(InputIt1 first1, InputIt1 last1,
                                      InputIt2 first2, InputIt2 last2,
                                      OutputIt result, Compare comp)
    {
        while (first1 != last1 && first2 != last2)
        {
            if (comp(*first1, *first2))
            {
                *result++ = *first1++;
            }
            else if (comp(*first2, *first1))
            {
                *result++ = *first2++;
            }
            else
            {
                ++first1;
                ++first2;
            }
        }
        result = zstl::copy(first1, last1, result);
        result = zstl::copy(first2, last2, result);
        return result;
    }

} // namespace zstl