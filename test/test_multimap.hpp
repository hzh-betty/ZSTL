#pragma once
#include <gtest/gtest.h>
#include "../container/map.hpp"
namespace zstl
{
    // 空容器基本属性测试
    TEST(MultiMapTest, EmptyContainer)
    {
        multimap<int, int> m;          // 新建空 multimap
        EXPECT_TRUE(m.empty());        // 应为空 :contentReference[oaicite:7]{index=7}
        EXPECT_EQ(m.size(), 0u);       // 大小为 0
        EXPECT_EQ(m.begin(), m.end()); // begin 应等于 end
    }

    // 单元素插入与查询测试
    TEST(MultiMapTest, SingleElement)
    {
        multimap<int, int> m;
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
    TEST(MultiMapTest, MultipleKeys)
    {
        multimap<int, char> m;
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
    TEST(MultiMapTest, EraseByKey)
    {
        multimap<int, int> m;
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
    TEST(MultiMapTest, EraseIteratorAndRange)
    {
        multimap<int, int> m;
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
    TEST(MultiMapTest, ClearAll)
    {
        multimap<int, int> m;
        m.insert({1, 10});
        m.insert({2, 20});
        EXPECT_FALSE(m.empty());
        m.clear();
        EXPECT_TRUE(m.empty());
        EXPECT_EQ(m.size(), 0u);
        EXPECT_EQ(m.begin(), m.end());
    }

    // 测试移动构造
    TEST(MultiMapTest, MoveConstructor)
    {
        multimap<int, int> m1;
        m1.insert({1, 10});
        m1.insert({2, 20});
        m1.insert({3, 30});

        multimap<int, int> m2(std::move(m1));
        EXPECT_EQ(m2.size(), 3);
        auto it = m2.begin();
        EXPECT_EQ(it->first, 1);
        EXPECT_EQ(it->second, 10);
        ++it;
        EXPECT_EQ(it->first, 2);
        EXPECT_EQ(it->second, 20);
        ++it;
        EXPECT_EQ(it->first, 3);
        EXPECT_EQ(it->second, 30);
        EXPECT_EQ(m1.size(), 0);
    }

    // 测试移动赋值
    TEST(MultiMapTest, MoveAssignment)
    {
        multimap<int, int> m1;
        m1.insert({1, 10});
        m1.insert({2, 20});
        m1.insert({3, 30});

        multimap<int, int> m2;
        m2 = std::move(m1);
        EXPECT_EQ(m2.size(), 3);
        auto it = m2.begin();
        EXPECT_EQ(it->first, 1);
        EXPECT_EQ(it->second, 10);
        ++it;
        EXPECT_EQ(it->first, 2);
        EXPECT_EQ(it->second, 20);
        ++it;
        EXPECT_EQ(it->first, 3);
        EXPECT_EQ(it->second, 30);
        EXPECT_EQ(m1.size(), 0);
    }

    // 测试 initializer_list 构造
    TEST(MultiMapTest, InitializerListConstructor)
    {
        multimap<int, int> m = {{1, 10}, {2, 20}, {3, 30}};
        EXPECT_EQ(m.size(), 3);
        auto it = m.begin();
        EXPECT_EQ(it->first, 1);
        EXPECT_EQ(it->second, 10);
        ++it;
        EXPECT_EQ(it->first, 2);
        EXPECT_EQ(it->second, 20);
        ++it;
        EXPECT_EQ(it->first, 3);
        EXPECT_EQ(it->second, 30);
    }

    TEST(MultiMapTest, Emplace)
    {
        multimap<int, int> mm;
        auto it = mm.emplace(1, 100);
        EXPECT_EQ(it->first, 1);
        EXPECT_EQ(it->second, 100);
        EXPECT_EQ(mm.count(1), 1u);
    }
}; // namespace zstl
