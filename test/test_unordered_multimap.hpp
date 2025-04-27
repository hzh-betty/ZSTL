#pragma once
#include "../include/unordered_map.hpp"
#include <gtest/gtest.h>
namespace zstl
{
    // 测试夹具：初始化一个空的 unordered_multimap<int>
    class UnorderedMultiMapTest : public ::testing::Test
    {
    protected:
        unordered_multimap<int> umm;
    };

    // 测试插入操作
    TEST_F(UnorderedMultiMapTest, InsertAndSize)
    {
        // 插入单个元素后，size 应该为 1
        auto it1 = umm.insert(42);
        EXPECT_EQ(*it1, 42) << "插入元素 42 后，迭代器应指向 42";
        EXPECT_EQ(umm.size(), 1u) << "插入一个元素后，size 应为 1";

        // 再次插入相同键，multimap 应允许重复，size 应增加
        auto it2 = umm.insert(42);
        EXPECT_EQ(*it2, 42) << "再次插入 42，应返回指向新元素的迭代器";
        EXPECT_EQ(umm.size(), 2u) << "插入重复键后，size 应为 2";

        // 插入不同键，size 继续增加
        auto it3 = umm.insert(7);
        EXPECT_EQ(*it3, 7) << "插入元素 7，应返回指向 7 的迭代器";
        EXPECT_EQ(umm.size(), 3u) << "插入不同键后，size 应为 3";
    }

    // 测试 find 和 count
    TEST_F(UnorderedMultiMapTest, FindAndCount)
    {
        umm.insert(1);
        umm.insert(1);
        umm.insert(2);

        // find 应返回首个匹配元素的迭代器
        auto it = umm.find(1);
        ASSERT_NE(it, umm.end()) << "find 应找到键 1";
        EXPECT_EQ(*it, 1) << "find 返回的元素应等于查询键";

        // count 应返回键 1 的出现次数
        EXPECT_EQ(umm.count(1), 2u) << "键 1 应出现 2 次";
        EXPECT_EQ(umm.count(2), 1u) << "键 2 应出现 1 次";
        EXPECT_EQ(umm.count(3), 0u) << "键 3 未插入，应返回 0";
    }

    // 测试 equal_range
    TEST_F(UnorderedMultiMapTest, EqualRangeIteration)
    {
        umm.insert(5);
        umm.insert(5);
        umm.insert(5);
        umm.insert(6);

        auto range = umm.equal_range(5);
        size_t cnt = 0;
        for (auto it = range.first; it != range.second; ++it)
        {
            EXPECT_EQ(*it, 5) << "equal_range 范围内元素应等于 5";
            ++cnt;
        }
        EXPECT_EQ(cnt, 3u) << "equal_range 应返回 3 个元素";
    }

    // 测试 erase by key, iterator, and range
    TEST_F(UnorderedMultiMapTest, EraseOperations)
    {
        umm.insert(10);
        umm.insert(10);
        umm.insert(20);
        umm.insert(20);
        EXPECT_EQ(umm.size(), 4u);

        // erase(key) 应删除所有该键元素，返回删除数量
        size_t erased = umm.erase(10);
        EXPECT_EQ(erased, 2u) << "erase(10) 应删除 2 个元素";
        EXPECT_EQ(umm.count(10), 0u) << "删除后 count(10) 应为 0";
        EXPECT_EQ(umm.size(), 2u) << "删除后总 size 应为 2";

        // erase(iterator) 删除单个元素
        auto it = umm.find(20);
        ASSERT_NE(it, umm.end());
        auto next_it = umm.erase(it);
        // 下一个迭代器要么指向第二个 20，要么等于 end
        if (next_it != umm.end())
        {
            EXPECT_EQ(*next_it, 20) << "erase 返回的迭代器应指向下一个 20";
        }
        EXPECT_EQ(umm.count(20), 1u) << "删除一个后，count(20) 应为 1";

        // erase(range) 删除剩余所有 20
        auto range = umm.equal_range(20);
        umm.erase(range.first, range.second);
        EXPECT_EQ(umm.count(20), 0u) << "range erase 后，count(20) 应为 0";
        EXPECT_TRUE(umm.empty()) << "全部删除后，应为空";
    }

    // 测试 clear 和 empty
    TEST_F(UnorderedMultiMapTest, ClearAndEmpty)
    {
        umm.insert(100);
        umm.insert(200);
        EXPECT_FALSE(umm.empty()) << "插入后不应为空";
        umm.clear();
        EXPECT_TRUE(umm.empty()) << "clear 后应为空";
        EXPECT_EQ(umm.size(), 0u) << "clear 后 size 应为 0";
    }

    // 测试 swap
    TEST_F(UnorderedMultiMapTest, SwapContents)
    {
        unordered_multimap<int> other;
        umm.insert(1);
        umm.insert(2);
        other.insert(3);

        umm.swap(other);

        // 交换后 umm 应包含原 other 的元素
        EXPECT_EQ(umm.size(), 1u);
        EXPECT_EQ(umm.count(3), 1u);
        EXPECT_EQ(other.size(), 2u);
        EXPECT_EQ(other.count(1), 1u);
        EXPECT_EQ(other.count(2), 1u);
    }

    // 测试 const 版本接口
    TEST_F(UnorderedMultiMapTest, ConstInterface)
    {
        umm.insert(7);
        umm.insert(7);
        const auto &cumm = umm;

        // const find
        auto cit = cumm.find(7);
        ASSERT_NE(cit, cumm.end());
        EXPECT_EQ(*cit, 7);

        // const equal_range
        auto crange = cumm.equal_range(7);
        size_t cnt = 0;
        for (auto it = crange.first; it != crange.second; ++it)
        {
            EXPECT_EQ(*it, 7);
            ++cnt;
        }
        EXPECT_EQ(cnt, 2u);
    }
};