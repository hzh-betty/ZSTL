#pragma once
#include <gtest/gtest.h>
#include "../include/set.hpp"
#include "../include/vector.hpp"
#include "../include/string.hpp"

namespace zstl
{
    // 测试 Fixture: 每个测试都能使用独立的 multiset 实例
    class MultiSetTest : public ::testing::Test
    {
    protected:
        multiset<int> ms;
    };

    // 测试默认构造后容器应为空
    TEST_F(MultiSetTest, InitiallyEmpty)
    {
        // 空容器应为空，size 为 0
        EXPECT_TRUE(ms.empty());
        EXPECT_EQ(ms.size(), 0);
    }

    // 测试单个元素插入
    TEST_F(MultiSetTest, SingleInsert)
    {
        auto it = ms.insert(42);
        // 插入后，容器不应为空，size 增为 1
        EXPECT_FALSE(ms.empty());
        EXPECT_EQ(ms.size(), 1);
        // begin() 应指向插入的元素
        EXPECT_EQ(*ms.begin(), 42);
        // insert 返回的迭代器解引用应为 42
        EXPECT_EQ(*it, 42);
    }

    // 测试重复元素插入
    TEST_F(MultiSetTest, DuplicateInsert)
    {
        ms.insert(5);
        ms.insert(5);
        ms.insert(5);
        // 重复插入三次，size 应为 3
        EXPECT_EQ(ms.size(), 3);
        // lower_bound 应指向第一个 5，upper_bound 指向末尾
        auto lb = ms.lower_bound(5);
        auto ub = ms.upper_bound(5);
        int count = 0;
        for (auto it = lb; it != ub; ++it)
        {
            EXPECT_EQ(*it, 5);
            ++count;
        }
        EXPECT_EQ(count, 3);
    }

    // 测试 find 与 equal_range
    TEST_F(MultiSetTest, FindAndEqualRange)
    {
        ms.insert(1);
        ms.insert(2);
        ms.insert(2);
        ms.insert(3);

        // find 应能找到已插入的元素
        auto f2 = ms.find(2);
        EXPECT_NE(f2, ms.end());
        EXPECT_EQ(*f2, 2);

        // 对于不存在的元素 find 应返回 end
        EXPECT_EQ(ms.find(99), ms.end());

        // equal_range 返回的区间内元素值都应等于 key
        auto range = ms.equal_range(2);
        int count = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            EXPECT_EQ(*it, 2);
            ++count;
        }
        EXPECT_EQ(count, 2);
    }

    // 测试 erase(iterator) 和 erase(key)
    TEST_F(MultiSetTest, EraseByIteratorAndKey)
    {
        // 插入若干元素
        ms.insert(10);
        ms.insert(20);
        ms.insert(20);
        ms.insert(30);
        EXPECT_EQ(ms.size(), 4);

        // erase(iterator)：删除第一个元素
        auto it = ms.begin();
        int val = *it;
        auto nextIt = ms.erase(it);
        EXPECT_EQ(*nextIt, 20);
        EXPECT_EQ(ms.size(), 3);

        // erase(key)：删除所有等于 20 的元素
        size_t erased = ms.erase(20);
        // 原有两个 20，erase 返回删除个数
        EXPECT_EQ(erased, 2);
        EXPECT_EQ(ms.size(), 1);
        EXPECT_EQ(*ms.begin(), 30);
    }

    // 测试按键删除所有元素
    TEST_F(MultiSetTest, EraseByKey)
    {
        ms.insert(0);
        for (int i = 1; i <= 10; i++)
        {
            for (int j = 1; j <= 100; j++)
            {
                ms.insert(j);
            }
        }

        ms.insert(101);

        // 删除所有值为 2 的元素
        size_t removed = ms.erase(2);

        EXPECT_EQ(removed, 10);   // 应删除100个2
        EXPECT_EQ(ms.size(), 992); // 剩余元素数为 902
        EXPECT_EQ(*ms.begin(), 0);
        EXPECT_EQ(*(--ms.end()), 101);
        EXPECT_EQ(ms.find(2), ms.end()); // 找不到 2
    }

    // 测试 erase(range)
    TEST_F(MultiSetTest, EraseRange)
    {
        for (int i = 1; i <= 5; ++i)
        {
            ms.insert(i);
            ms.insert(i);
        }
        // 此时每个 1..5 都有两个，size=10
        EXPECT_EQ(ms.size(), 10);

        // 删除 [lower_bound(2), upper_bound(4)) 区间内的所有元素
        auto first = ms.lower_bound(2);
        auto last = ms.upper_bound(4);
        ms.erase(first, last);

        // 删除后，应剩下 1、5 各两个，共 4 个元素
        EXPECT_EQ(ms.size(), 4);
        for (auto v : {1, 1, 5, 5})
        {
            auto fit = ms.find(v);
            EXPECT_NE(fit, ms.end());
        }
    }

    // 测试 const 版本的 lower_bound/upper_bound
    TEST_F(MultiSetTest, ConstBounds)
    {
        ms.insert(7);
        ms.insert(7);
        const multiset<int> &cms = ms;

        auto lb = cms.lower_bound(7);
        auto ub = cms.upper_bound(7);
        EXPECT_NE(lb, ub);
        EXPECT_EQ(lb, ms.begin());
        EXPECT_EQ(ub, ms.end());
    }

    // 测试在空容器上 erase(key)
    TEST_F(MultiSetTest, EraseNonexistentKey)
    {
        size_t erased = ms.erase(100);
        EXPECT_EQ(erased, 0);
        EXPECT_TRUE(ms.empty());
    }

    // 边界测试：begin == end 时不能解引用
    TEST_F(MultiSetTest, BeginEndSafety)
    {
        // 空容器 begin==end
        EXPECT_EQ(ms.begin(), ms.end());
        // 不要解引用空容器迭代器，以下行为未定义，仅测试相等性
    }

};