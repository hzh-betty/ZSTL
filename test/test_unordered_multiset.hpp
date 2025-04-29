#pragma once
#include "gtest/gtest.h"
#include "../include/unordered_set.hpp"
#include "../include/string.hpp"
#include "../include/vector.hpp"
namespace zstl
{
    // 整数类型测试
    TEST(UnorderedMultisetTest, IntBasicOperations)
    {
        unordered_multiset<int> ms;
        // 初始应该为空
        EXPECT_TRUE(ms.empty());
        EXPECT_EQ(ms.size(), 0);

        // 插入元素
        auto it1 = ms.insert(10);
        EXPECT_NE(it1, ms.end()); // 插入成功，迭代器有效
        EXPECT_FALSE(ms.empty());
        EXPECT_EQ(ms.size(), 1);

        // 插入重复元素
        ms.insert(10);
        ms.insert(20);
        EXPECT_EQ(ms.count(10), 2); // 10 出现两次
        EXPECT_EQ(ms.count(20), 1);
        EXPECT_EQ(ms.size(), 3);

        // 查找存在和不存在的元素
        EXPECT_NE(ms.find(10), ms.end());
        EXPECT_EQ(ms.find(30), ms.end());

        // equal_range 返回正确范围
        auto range = ms.equal_range(10);
        int cnt = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            EXPECT_EQ(*it, 10);
            cnt++;
        }
        EXPECT_EQ(cnt, 2);

        // 删除单个 key
        size_t erased = ms.erase(10);
        EXPECT_EQ(erased, 2);
        EXPECT_EQ(ms.count(10), 0);
        EXPECT_EQ(ms.size(), 1);

        // erase by iterator
        auto it20 = ms.find(20);
        ASSERT_NE(it20, ms.end());
        ms.erase(it20);
        EXPECT_EQ(ms.size(), 0);
        EXPECT_TRUE(ms.empty());
    }

    // 字符串类型测试
    TEST(UnorderedMultisetTest, StringBasicOperations)
    {
        unordered_multiset<string> ms;
        // 插入多个字符串
        const char *inputs[4] = {"a", "bb", "ccc", "bb"};
        for (const auto &str : inputs)
        {
            string s = str;
            ms.insert(s);
        }
        // size 和 count
        EXPECT_EQ(ms.size(), 4);
        EXPECT_EQ(ms.count("bb"), 2);
        EXPECT_EQ(ms.count("zzz"), 0);

        // equal_range 遍历
        auto range_bb = ms.equal_range("bb");
        int cnt_bb = 0;
        for (auto it = range_bb.first; it != range_bb.second; ++it)
        {
            EXPECT_EQ(*it, "bb");
            cnt_bb++;
        }
        EXPECT_EQ(cnt_bb, 2);

        // clear 清空
        ms.clear();
        EXPECT_TRUE(ms.empty());
        EXPECT_EQ(ms.size(), 0);
    }

    // swap 测试
    TEST(UnorderedMultisetTest, SwapTest)
    {
        unordered_multiset<int> a;
        unordered_multiset<int> b;
        a.insert(1);
        a.insert(2);
        b.insert(3);

        a.swap(b);
        // 交换后 a 包含 3，b 包含 1,2
        EXPECT_EQ(a.size(), 1);
        EXPECT_EQ(*a.begin(), 3);
        EXPECT_EQ(b.size(), 2);
        EXPECT_NE(b.find(1), b.end());
        EXPECT_NE(b.find(2), b.end());
    }

    // 迭代器遍历测试
    TEST(UnorderedMultisetTest, IteratorTraversal)
    {
        unordered_multiset<int> ms;
        for (int i = 0; i < 5; ++i)
            ms.insert(i);
        bool seen[5] = {false};
        int cnt = 0;
        for (auto it = ms.begin(); it != ms.end(); ++it)
        {
            int v = *it;
            EXPECT_GE(v, 0);
            EXPECT_LT(v, 5);
            seen[v] = true;
            cnt++;
        }
        EXPECT_EQ(cnt, 5);
        for (int i = 0; i < 5; ++i)
            EXPECT_TRUE(seen[i]);
    }

    // 常量版本接口测试
    TEST(UnorderedMultisetTest, ConstInterface)
    {
        unordered_multiset<int> ms;
        ms.insert(42);
        const auto &cms = ms;
        EXPECT_FALSE(cms.empty());
        EXPECT_EQ(cms.size(), 1);
        EXPECT_NE(cms.find(42), cms.end());
        auto range = cms.equal_range(42);
        EXPECT_EQ(++cms.begin(), cms.end());
    };

    // 测试按键删除所有元素
    TEST(UnorderedMultisetTest, EraseByKey)
    {
        unordered_multiset<int> ms;
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

        EXPECT_EQ(removed, 10);    // 应删除100个2
        EXPECT_EQ(ms.size(), 992); // 剩余元素数为 992
        EXPECT_EQ(*ms.begin(), 0);
        EXPECT_EQ(*ms.find(101), 101);
        EXPECT_EQ(ms.find(2), ms.end()); // 找不到 2
    }

    // 测试移动构造
    TEST(UnorderedMultisetTest, MoveConstructor)
    {
        unordered_multiset<int> ms1;
        ms1.insert(1);
        ms1.insert(2);
        ms1.insert(3);

        unordered_multiset<int> ms2(std::move(ms1));
        EXPECT_EQ(ms2.size(), 3);
        EXPECT_EQ(ms2.count(1), 1);
        EXPECT_EQ(ms2.count(2), 1);
        EXPECT_EQ(ms2.count(3), 1);
        EXPECT_EQ(ms1.size(), 0);
    }

    // 测试移动赋值
    TEST(UnorderedMultisetTest, MoveAssignment)
    {
        unordered_multiset<int> ms1;
        ms1.insert(1);
        ms1.insert(2);
        ms1.insert(3);

        unordered_multiset<int> ms2;
        ms2 = std::move(ms1);
        EXPECT_EQ(ms2.size(), 3);
        EXPECT_EQ(ms2.count(1), 1);
        EXPECT_EQ(ms2.count(2), 1);
        EXPECT_EQ(ms2.count(3), 1);
        EXPECT_EQ(ms1.size(), 0);
    }

    // 测试 initializer_list 构造
    TEST(UnorderedMultisetTest, InitializerListConstructor)
    {
        unordered_multiset<int> ms = {1, 2, 3};
        EXPECT_EQ(ms.size(), 3);
        EXPECT_EQ(ms.count(1), 1);
        EXPECT_EQ(ms.count(2), 1);
        EXPECT_EQ(ms.count(3), 1);
    }

    TEST(UnorderedMultisetTest, Emplace)
    {
        unordered_multiset<int> ums;
        auto it = ums.emplace(42);
        EXPECT_EQ(*it, 42);
        EXPECT_EQ(ums.count(42), 1u);
    }
}; // namespace zstl