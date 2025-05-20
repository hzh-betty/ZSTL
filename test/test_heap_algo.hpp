#pragma once
#include <gtest/gtest.h>
#include "../algorithm/heap_algo.hpp"
#include "../container/vector.hpp"
#include"../functor/functional.hpp"
namespace zstl
{
    // push_heap 测试
    TEST(HeapAlgoTest, PushHeap)
    {
        vector<int> v{10, 8, 6, 4, 2};
        v.push_back(12);
        zstl::push_heap(v.begin(), v.end());
        // 12 应为堆顶
        EXPECT_EQ(v.front(), 12);

        // 自定义比较器（最小堆）
        vector<int> v2{2, 4, 6, 8, 10};
        v2.push_back(1);
        zstl::push_heap(v2.begin(), v2.end(), zstl::greater<int>());
        EXPECT_EQ(v2.front(), 1);
    }

    // pop_heap 测试
    TEST(HeapAlgoTest, PopHeap)
    {
        vector<int> v{12, 10, 6, 4, 2, 8};
        make_heap(v.begin(), v.end());
        pop_heap(v.begin(), v.end());
        // 堆顶元素被移到最后
        EXPECT_EQ(v.back(), 12);
        // 剩余部分仍为堆
        EXPECT_TRUE(zstl::is_heap(v.begin(), v.end() - 1));

        // 自定义比较器（最小堆）
        vector<int> v2{1, 4, 6, 8, 10, 2};
        zstl::make_heap(v2.begin(), v2.end(), zstl::greater<int>());
        zstl::pop_heap(v2.begin(), v2.end(), zstl::greater<int>());
        EXPECT_EQ(v2.back(), 1);
        EXPECT_TRUE(zstl::is_heap(v2.begin(), v2.end() - 1, zstl::greater<int>()));
    }

    // make_heap 测试
    TEST(HeapAlgoTest, MakeHeap)
    {
        vector<int> v{3, 1, 4, 1, 5, 9, 2};
        zstl::make_heap(v.begin(), v.end());
        EXPECT_TRUE(zstl::is_heap(v.begin(), v.end()));

        // 自定义比较器（最小堆）
        vector<int> v2{3, 1, 4, 1, 5, 9, 2};
        zstl::make_heap(v2.begin(), v2.end(), zstl::greater<int>());
        EXPECT_TRUE(zstl::is_heap(v2.begin(), v2.end(), zstl::greater<int>()));
    }

    // sort_heap 测试
    TEST(HeapAlgoTest, SortHeap)
    {
        vector<int> v{3, 1, 4, 1, 5, 9, 2};
        zstl::make_heap(v.begin(), v.end());
        zstl::sort_heap(v.begin(), v.end());
        // 应为降序排列
        for (size_t i = 1; i < v.size(); ++i)
            EXPECT_GE(v[i], v[i - 1]);

        // 自定义比较器（最小堆，升序排列）
        vector<int> v2{3, 1, 4, 1, 5, 9, 2};
        zstl::make_heap(v2.begin(), v2.end(), zstl::greater<int>());
        zstl::sort_heap(v2.begin(), v2.end(), zstl::greater<int>());
        for (size_t i = 1; i < v2.size(); ++i)
            EXPECT_LE(v2[i], v2[i - 1]);
    }
}
