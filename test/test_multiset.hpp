#pragma once
#include <gtest/gtest.h>
#include "../include/set.hpp"
#include "../include/vector.hpp"
#include "../include/string.hpp"

namespace zstl
{
    // 测试基本插入和大小
    TEST(MultisetBasicTest, InsertAndSize)
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
    TEST(MultisetDuplicateTest, InsertDuplicates)
    {
        multiset<int> ms;
        ms.insert(4);
        ms.insert(4);
        ms.insert(4);
        EXPECT_EQ(ms.size(), 3); // 重复插入三次，大小为 3

        // 所有值都应该等于 4
        int count = 0;
        for (auto it = ms.begin(); it != ms.end(); ++it)
        {
            EXPECT_EQ(*it, 4);
            ++count;
        }
        EXPECT_EQ(count, 3);
    }

    // 测试按键删除所有元素
    TEST(MultisetEraseKeyTest, EraseByKey)
    {
        multiset<int> ms;
        ms.insert(1);
        ms.insert(2);
        ms.insert(2);
        ms.insert(3);


        // 删除所有值为 2 的元素
        size_t removed = ms.erase(2);
        EXPECT_EQ(removed, 2);   // 应删除两个 2
        EXPECT_EQ(ms.size(), 2); // 剩余元素数为 2
        EXPECT_EQ(*ms.find(1), 1);
        EXPECT_EQ(ms.find(2), ms.end()); // 找不到 2
    }

    // 测试通过迭代器删除单个元素
    TEST(MultisetEraseIteratorTest, EraseByIterator)
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
    TEST(MultisetFindTest, FindExistingAndNonExisting)
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

    // 测试拷贝构造与赋值
    TEST(MultisetCopyTest, CopyAndAssignment)
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
    TEST(MultisetConstTest, ConstBeginEnd)
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
    TEST(MultisetOrderTest, IteratorOrder)
    {
        multiset<int> ms;
        ms.insert(50);
        ms.insert(20);
        ms.insert(40);
        ms.insert(30);

        std::vector<int> values;
        for (auto x : ms)
        {
            values.push_back(x);
        }
        // 应按升序排列
        std::vector<int> expected = {20, 30, 40, 50};
        EXPECT_EQ(values, expected);
    }
};