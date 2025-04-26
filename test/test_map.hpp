#pragma once
#include "../include/map.hpp"
#include "../include/string.hpp"
#include <gtest/gtest.h>
namespace zstl
{
    // 测试夹具：为每个测试提供独立的 map 实例
    class MapTest : public ::testing::Test
    {
    protected:
        map<int, string> m;
    };

    // 测试默认构造后容器应为空
    TEST_F(MapTest, InitiallyEmpty)
    {
        // 空时 size 为 0，empty 返回 true
        EXPECT_TRUE(m.empty());
        EXPECT_EQ(m.size(), 0);
    }

    // 测试 insert 和 find
    TEST_F(MapTest, InsertAndFind)
    {
        auto p = m.insert({1, "one"});
        // insert 返回 pair<iterator,bool>，第一次插入应成功
        EXPECT_TRUE(p.second);
        EXPECT_EQ(p.first->first, 1);
        EXPECT_EQ(p.first->second, "one");

        // find 应能找到键 1
        auto it = m.find(1);
        EXPECT_NE(it, m.end());
        EXPECT_EQ(it->second, "one");

        // 对于不存在的键 find 返回 end
        EXPECT_EQ(m.find(42), m.end());
    }

    // 测试 operator[] 行为：不存在时插入，存在时访问
    TEST_F(MapTest, SubscriptOperator)
    {
        // operator[] 不存在则插入，返回引用，可修改
        m[5] = "five";
        EXPECT_EQ(m.size(), 1);
        EXPECT_EQ(m[5], "five");

        // 已存在则不改变 size，仅返回引用
        m[5] = "FIVE";
        EXPECT_EQ(m.size(), 1);
        EXPECT_EQ(m[5], "FIVE");
    }

    // 测试重复插入相同键
    TEST_F(MapTest, DuplicateInsert)
    {
        m.insert({2, "two"});
        auto p2 = m.insert({2, "TWO"});
        // 第二次插入相同键应失败，返回 false
        EXPECT_FALSE(p2.second);
        // 值应保持第一次插入的 "two"
        EXPECT_EQ(m.find(2)->second, "two");
    }

    // 测试 erase by key 和 erase(iterator)
    TEST_F(MapTest, EraseKeyAndIterator)
    {
        m[1] = "one";
        m[2] = "two";
        m[3] = "three";
        EXPECT_EQ(m.size(), 3);

        // erase(key) 返回被删除元素个数
        size_t ec = m.erase(2);
        EXPECT_EQ(ec, 1);
        EXPECT_EQ(m.size(), 2);
        EXPECT_EQ(m.find(2), m.end());

        // erase(iterator) 删除指定位置，返回下一个 iterator
        auto it = m.find(1);
        auto next = m.erase(it);
        EXPECT_EQ(next->first, 3);
        EXPECT_EQ(m.size(), 1);
    }

    // 测试 clear 功能（如果有）
    TEST_F(MapTest, Clear)
    {
        m[1] = "one";
        m[2] = "two";
        m.clear();
        EXPECT_TRUE(m.empty());
        EXPECT_EQ(m.size(), 0);
    }

    // 测试复杂类型作为 value
    TEST_F(MapTest, ComplexValueType)
    {
        struct Point
        {
            int x, y;
        };
        map<int, Point> m;
        m[10] = Point{3, 4};
        auto it = m.find(10);
        ASSERT_NE(it, m.end());
        EXPECT_EQ(it->second.x, 3);
        EXPECT_EQ(it->second.y, 4);
    }

    // 测试迭代顺序与 Compare
    TEST_F(MapTest, IterationOrder)
    {
        m.insert({3, "three"});
        m.insert({1, "one"});
        m.insert({2, "two"});
        // map 应按键升序排列

        int cnt = 1;
        for (auto iter = m.begin(); iter != m.end(); ++iter)
        {
            EXPECT_EQ(iter->first, cnt++);
        }
    }

};