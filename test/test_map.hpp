#pragma once
#include "../include/map.hpp"
#include "../include/string.hpp"
#include <gtest/gtest.h>
namespace zstl
{
    // 测试空 map 的行为
    TEST(MapTest, EmptyMap)
    {
        map<int, int> m;
        // 刚创建的 map 应该为空
        EXPECT_TRUE(m.empty());
        // 大小为 0
        EXPECT_EQ(m.size(), 0u);
        // begin() 应等于 end()
        EXPECT_EQ(m.begin(), m.end());
    }

    // 测试 zstl::map 的基本功能
    TEST(MapTest, InsertAndFind)
    {
        map<int, string> m;
        // 插入元素并检查返回结果
        auto res1 = m.insert(std::make_pair(1, string("one")));
        EXPECT_TRUE(res1.second) << "首次插入 key=1 应成功";
        EXPECT_EQ(res1.first->first, 1);
        EXPECT_EQ(res1.first->second, "one");

        // 插入相同 key 应失败，返回原 iterator
        auto res2 = m.insert(std::make_pair(1, string("uno")));
        EXPECT_FALSE(res2.second) << "重复插入 key=1 应失败";
        EXPECT_EQ(res2.first->second, "one") << "值应保持原有的 'one'";

        // 使用 operator[] 插入或访问
        m[2] = "two";
        EXPECT_EQ(m.find(2)->second, "two");
        EXPECT_EQ(m[2], "two");

        // 查找不存在的 key
        auto it = m.find(3);
        EXPECT_EQ(it, m.end()) << "查找不存在的 key=3 应返回 end()";
        EXPECT_EQ(m.size(), 2u);
    }

    // 测试迭代器顺序和 begin/end
    TEST(MapTest, IteratorOrder)
    {
        map<int, int> m;
        // 插入多组乱序数据
        m.insert(std::make_pair(5, 50));
        m.insert(std::make_pair(1, 10));
        m.insert(std::make_pair(3, 30));
        m.insert(std::make_pair(4, 40));
        m.insert(std::make_pair(2, 20));

        // 检查中序遍历结果为有序
        int expect_key = 1;
        for (auto it = m.begin(); it != m.end(); ++it)
        {
            EXPECT_EQ(it->first, expect_key) << "迭代器键值应从 1 到 5 顺序遍历";
            expect_key++;
        }
        EXPECT_EQ(expect_key, 6);

        for (auto it = --m.end(); it != m.begin(); --it)
        {
            // 遍历顺序应为降序
            EXPECT_EQ(it->first, --expect_key);
        }
    }

    // 测试 erase 功能及返回值
    TEST(MapTest, EraseExistingAndNonExisting)
    {
        map<int, int> m;
        for (int i = 0; i < 5; ++i)
        {
            m.insert(std::make_pair(i, i * 10));
        }
        EXPECT_EQ(m.size(), 5u);

        // 删除存在的 key
        EXPECT_EQ(m.erase(2), 1);
        EXPECT_EQ(m.find(2), m.end());

        // 删除不存在的 key
        EXPECT_EQ(m.erase(999), 0) << "删除不存在的 key=999 应返回 false";
        EXPECT_EQ(m.size(), 4u);
    }

    // 测试拷贝构造和赋值操作
    TEST(MapTest, CopyAndAssignment)
    {
        map<int, string> m1;
        m1.insert(std::make_pair(1, "a"));
        m1.insert(std::make_pair(2, "b"));

        // 拷贝构造
        map<int, string> m2(m1);
        EXPECT_EQ(m2.find(1)->second, "a");
        EXPECT_EQ(m2.find(2)->second, "b");

        // 修改原 map，不影响拷贝
        m1[1] = "alpha";
        EXPECT_EQ(m1.find(1)->second, "alpha");
        EXPECT_EQ(m2.find(1)->second, "a");

        // 赋值操作
        map<int, string> m3;
        m3 = m1;
        EXPECT_EQ(m3.find(1)->second, "alpha");
        EXPECT_EQ(m3.find(2)->second, "b");
    }

    // 测试空 map 的 begin/end 和操作
    TEST(MapTest, EmptyMapBehaviors)
    {
        map<int, int> m;
        EXPECT_EQ(m.begin(), m.end()) << "空 map 的 begin() 应等于 end()";
        EXPECT_FALSE(m.erase(1)) << "对空 map 删除任何元素应返回 false";
    }

    // 测试复杂类型作为 value
    TEST(MapTest, ComplexValueType)
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
};