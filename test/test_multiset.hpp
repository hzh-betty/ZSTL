#pragma once
#include <gtest/gtest.h>
#include "../include/set.hpp"
#include "../include/vector.hpp"
#include "../include/string.hpp"

namespace zstl
{
    // 测试基本插入和大小
    TEST(MultisetTest, InsertAndSize)
    {
        multiset<int> ms;
        EXPECT_TRUE(ms.empty()); // 刚创建应为空容器
        EXPECT_EQ(ms.size(), 0); // 大小为 0

        ms.insert(5);
        ms.insert(3);
        ms.insert(8);
        EXPECT_FALSE(ms.empty()); // 插入后不为空
        EXPECT_EQ(ms.size(), 3);  // 大小应为 3
    }

    // 测试支持重复元素插入
    TEST(MultisetTest, InsertDuplicates)
    {
        multiset<int> ms;
        ms.insert(1);
        for (int i = 0; i < 10; i++)
        {
            ms.insert(4);
        }
        ms.insert(10);
        EXPECT_EQ(ms.size(), 12); // 重复插入12次，大小为 12

        EXPECT_EQ(*ms.begin(), 1);
        EXPECT_EQ(*(--ms.end()), 10);
        auto begin1 = ++ms.begin();
        auto end1 = --ms.end();

        // 所有值都应该等于 4
        int count = 0;
        for (auto it = begin1; it != end1; ++it)
        {
            EXPECT_EQ(*it, 4);
            ++count;
        }
        EXPECT_EQ(count, 10);
    }

    // 测试按键删除所有元素
    TEST(MultisetTest, EraseByKey)
    {
        multiset<int> ms;
        ms.insert(0);
        for (int i = 0; i < 10; i++)
        {
            for (int j = 1; j < 10; j++)
                ms.insert(j);
        }
        ms.insert(10);

        // 删除所有值为 2 的元素
        size_t removed = ms.erase(2);
        EXPECT_EQ(removed, 1000); // 应删除两个 2
        EXPECT_EQ(ms.size(), 82);  // 剩余元素数为 2
        EXPECT_EQ(*ms.find(1), 0);
        EXPECT_EQ(*ms.find(3), 10);
        EXPECT_EQ(ms.find(2), ms.end()); // 找不到 2
    }

    // 测试通过迭代器删除单个元素
    TEST(MultisetTest, EraseByIterator)
    {
        multiset<int> ms;
        ms.insert(10);
        ms.insert(20);
        ms.insert(30);

        auto it = ms.find(20);
        ASSERT_NE(it, ms.end()); // 确保找到了 20
        ms.erase(it);            // 删除该迭代器指向的元素
        EXPECT_EQ(ms.size(), 2);
        EXPECT_EQ(ms.find(20), ms.end()); // 20 已被删除
    }

    // 测试查找操作
    TEST(MultisetTest, FindExistingAndNonExisting)
    {
        multiset<std::string> ms;
        ms.insert("apple");
        ms.insert("banana");

        auto it1 = ms.find("apple");
        EXPECT_NE(it1, ms.end()); // 能找到 apple
        EXPECT_EQ(*it1, "apple");

        auto it2 = ms.find("cherry");
        EXPECT_EQ(it2, ms.end()); // 找不到 cherry
    }

    // 测试插入大量元素的稳定性
    TEST(MultisetTest, BulkInsertErase)
    {
        multiset<int> ss;
        const int N = 1000;
        for (int i = 0; i < N; ++i)
        {
            ss.insert(i);
        }
        EXPECT_EQ(ss.size(), 1000u);

        // 验证所有元素都能找到
        for (int i = 0; i < N; ++i)
        {
            EXPECT_NE(ss.find(i), ss.end());
        }
        // 删除偶数元素
        for (int i = 0; i < N; i += 2)
        {
            EXPECT_EQ(ss.erase(i), 1);
        }
        EXPECT_EQ(ss.size(), 500u);

        // 验证偶数已删除，奇数仍存在
        for (int i = 0; i < N; ++i)
        {
            if (i % 2 == 0)
            {
                EXPECT_EQ(ss.find(i), ss.end());
            }
            else
            {
                EXPECT_NE(ss.find(i), ss.end());
            }
        }
    }

    // 测试拷贝构造与赋值
    TEST(MultisetTest, CopyAndAssignment)
    {
        multiset<int> ms1;
        ms1.insert(7);
        ms1.insert(9);

        // 拷贝构造
        multiset<int> ms2(ms1);
        EXPECT_EQ(ms2.size(), ms1.size());
        EXPECT_EQ(*ms2.find(7), 7);

        // 赋值
        multiset<int> ms3;
        ms3 = ms1;
        EXPECT_EQ(ms3.size(), ms1.size());
        EXPECT_EQ(*ms3.find(9), 9);
    }

    // 测试 const_iterator 和 const 对象访问
    TEST(MultisetTest, ConstBeginEnd)
    {
        multiset<int> ms;
        ms.insert(2);
        ms.insert(1);

        const multiset<int> cms = ms;
        auto it = cms.begin(); // const_iterator
        EXPECT_EQ(*it, 1);     // 排序后最小为 1
        ++it;
        EXPECT_EQ(*it, 2);
    }

    // 测试迭代器遍历顺序（应为有序）
    TEST(MultisetTest, IteratorOrder)
    {
        multiset<int> ms;
        ms.insert(50);
        ms.insert(20);
        ms.insert(40);
        ms.insert(30);

        int num = 20;
        vector<int> values;
        for (auto x : ms)
        {
            EXPECT_EQ(x, num);
            num += 10;
        }
    }
};