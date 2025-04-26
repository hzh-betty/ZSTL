#pragma once
#include <gtest/gtest.h>
#include "../include/map.hpp"
namespace zstl
{
    // 空容器基本属性测试
    TEST(MutiMapTest, EmptyContainer)
    {
        mutimap<int, int> m;           // 新建空 multimap
        EXPECT_TRUE(m.empty());        // 应为空 :contentReference[oaicite:7]{index=7}
        EXPECT_EQ(m.size(), 0u);       // 大小为 0
        EXPECT_EQ(m.begin(), m.end()); // begin 应等于 end
    }

    // 单元素插入与查询测试
    TEST(MutiMapTest, SingleElement)
    {
        mutimap<int, int> m;
        auto it = m.insert({5, 100}); // 插入键值对 (5,100)
        ASSERT_NE(it, m.end());       // 插入应成功，迭代器不等于 end
        EXPECT_EQ(it->first, 5);      // 键为 5
        EXPECT_EQ(it->second, 100);   // 值为 100

        // find 应能找到该键
        auto it2 = m.find(5);
        ASSERT_NE(it2, m.end());
        EXPECT_EQ(it2->second, 100);

        // lower_bound 与 upper_bound
        EXPECT_EQ(m.lower_bound(5), it);      // lower_bound 返回该元素
        EXPECT_EQ(m.upper_bound(5), m.end()); // upper_bound 在单元素情况下为 end

        // equal_range 范围仅包含该元素
        auto range = m.equal_range(5);
        EXPECT_EQ(range.first, it);
        EXPECT_EQ(range.second, m.end());
    }

    // 多重键插入与迭代顺序测试
    TEST(MutiMapTest, MultipleKeys)
    {
        mutimap<int, char> m;
        // 乱序插入
        m.insert({3, 'c'});
        m.insert({1, 'a'});
        m.insert({2, 'b'});
        m.insert({2, 'B'}); // 重复键

        // 遍历并检查键按升序排列
        int expected_keys[] = {1, 2, 2, 3};
        int idx = 0;
        for (auto it = m.begin(); it != m.end(); ++it)
        {
            EXPECT_EQ(it->first, expected_keys[idx++]);
        }
        EXPECT_EQ(idx, 4); // 共 4 个元素
    }

    // erase(key) 返回删除元素个数测试
    TEST(MutiMapTest, EraseByKey)
    {
        mutimap<int, int> m;
        m.insert({0, 0});
        for (int i = 1; i <= 10; i++)
        {
            for (int j = 1; j <= 100; j++)
            {
                m.insert({j, j});
            }
        }

        m.insert({101, 101});

        // 删除所有值为 2 的元素
        size_t removed = m.erase(2);

        EXPECT_EQ(removed, 10);   // 应删除10个2
        EXPECT_EQ(m.size(), 992); // 剩余元素数为 992
        EXPECT_EQ(m.begin()->first, 0);
        EXPECT_EQ((--m.end())->first, 101);
        EXPECT_EQ(m.find(2), m.end()); // 找不到 2
    }
    // 按迭代器删除单个与区间测试
    TEST(MutiMapTest, EraseIteratorAndRange)
    {
        mutimap<int, int> m;
        m.insert({1, 1});
        m.insert({2, 2});
        m.insert({3, 3});
        // erase 单个
        auto it2 = m.find(2);
        ASSERT_NE(it2, m.end());
        m.erase(it2);
        EXPECT_EQ(m.find(2), m.end());
        EXPECT_EQ(m.size(), 2u);

        // erase 区间: 清空剩余
        m.erase(m.begin(), m.end());
        EXPECT_TRUE(m.empty());
    }

    // clear() 恢复空容器测试
    TEST(MutiMapTest, ClearAll)
    {
        mutimap<int, int> m;
        m.insert({1, 10});
        m.insert({2, 20});
        EXPECT_FALSE(m.empty());
        m.clear();
        EXPECT_TRUE(m.empty());
        EXPECT_EQ(m.size(), 0u);
        EXPECT_EQ(m.begin(), m.end());
    }
}; // namespace zstl
