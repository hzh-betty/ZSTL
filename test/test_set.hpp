#pragma once
#include "../container/set.hpp"
#include <gtest/gtest.h>
namespace zstl
{
    // 测试空集合的begin和end
    TEST(SetTest, EmptySet)
    {
        set<int> s;
        // 新创建的集合应为空，大小为0
        EXPECT_TRUE(s.empty());
        EXPECT_EQ(s.size(), 0u);
        // begin() 应等于 end()
        EXPECT_EQ(s.begin(), s.end());
    }

    // 测试插入单个元素并查找
    TEST(SetTest, InsertAndFind)
    {
        set<int> s;
        auto result = s.insert(42);
        // 插入应成功
        EXPECT_TRUE(result.second);
        // 迭代器指向的值应为插入值
        EXPECT_EQ(*result.first, 42);
        // find应能找到该元素
        EXPECT_NE(s.find(42), s.end());
        EXPECT_EQ(s.size(), 1u);
    }

    // 测试重复插入
    TEST(SetTest, DuplicateInsert)
    {
        set<int> s;
        s.insert(7);
        auto result = s.insert(7);
        // 第二次插入同一元素应返回false
        EXPECT_FALSE(result.second);
        // 迭代器应指向已有元素的位置
        EXPECT_EQ(*result.first, 7);
        EXPECT_EQ(s.size(), 1u);
    }

    // 测试删除元素
    TEST(SetTest, EraseElement)
    {
        set<int> s;
        s.insert(1);
        s.insert(2);
        EXPECT_EQ(s.size(), 2u);
        // 删除存在的元素应返回并移除
        EXPECT_EQ(s.erase(1), 1);
        EXPECT_EQ(s.find(1), s.end());
        // 删除不存在的元素应返回false
        EXPECT_EQ(s.erase(99), 0);
        EXPECT_EQ(s.size(), 1);
    }

    // 范围删除测试
    TEST(SetTest, EraseRange)
    {
        set<int> s;
        for (int i = 0; i < 100; ++i)
            s.insert(i);
        auto it1 = s.lower_bound(1);
        EXPECT_EQ(*it1, 1);
        auto it2 = s.upper_bound(98);
        EXPECT_EQ(*it2, 99);
        auto ret = s.erase(it1, it2);
        EXPECT_EQ(*ret, 99); // 返回 first not erased

        auto iter = s.begin();
        EXPECT_EQ(*iter, 0);
        ++iter;
        EXPECT_EQ(*iter, 99);
    }

    // 边界操作
    TEST(SetTest, BoundsAndEqualRange)
    {
        set<int> s;
        for (int x : {10, 20, 30, 40})
            s.insert(x);
        EXPECT_EQ(*s.lower_bound(25), 30);
        EXPECT_EQ(s.lower_bound(100), s.end());
        EXPECT_EQ(*s.upper_bound(20), 30);
        EXPECT_EQ(s.upper_bound(40), s.end());
        auto range = s.equal_range(20);
        EXPECT_EQ(*range.first, 20);
        EXPECT_EQ(*range.second, 30);
    }
    // 测试有序遍历
    TEST(SetTest, OrderedTraversal)
    {
        set<int> s;
        int values[] = {3, 1, 4, 2};
        for (int v : values)
        {
            s.insert(v);
        }
        EXPECT_EQ(s.size(), 4u);
        int expected = 1;
        for (int it : s)
        {
            // 遍历顺序应为升序
            EXPECT_EQ(it, expected++);
        }
        EXPECT_EQ(expected, 5);

        for (auto it = --s.end(); it != s.begin(); --it)
        {
            // 遍历顺序应为升序
            EXPECT_EQ(*it, --expected);
        }
    }

    // 测试const_iterator
    TEST(SetTest, ConstIterator)
    {
        set<int> s;
        s.insert(10);
        s.insert(20);
        const set<int> &cs = s;
        auto it = cs.begin();
        // const_iterator解引并访问值
        EXPECT_EQ(*it, 10);
        ++it;
        EXPECT_EQ(*it, 20);
    }

    // 测试插入大量元素的稳定性
    TEST(SetTest, BulkInsertErase)
    {
        set<int> s;
        const int N = 1000;
        for (int i = 0; i < N; ++i)
        {
            s.insert(i);
        }
        EXPECT_EQ(s.size(), 1000u);

        // 验证所有元素都能找到
        for (int i = 0; i < N; ++i)
        {
            EXPECT_NE(s.find(i), s.end());
        }
        // 删除偶数元素
        for (int i = 0; i < N; i += 2)
        {
            EXPECT_EQ(s.erase(i), 1);
        }
        EXPECT_EQ(s.size(), 500u);
        
        // 验证偶数已删除，奇数仍存在
        for (int i = 0; i < N; ++i)
        {
            auto iter = s.find(i);
            if (i % 2 == 0)
            {
                EXPECT_EQ(iter, s.end());
            }
            else
            {
                EXPECT_NE(iter, s.end());
            }
        }

        EXPECT_EQ(*s.begin(), 1);
        EXPECT_EQ(*(--s.end()), 999);
    }

    // 测试拷贝构造函数
    TEST(SetTest, CopyConstructor)
    {
        set<int> s1;
        int values[] = {5, 3, 8, 1};
        for (int v : values)
        {
            s1.insert(v);
        }
        // 使用拷贝构造生成新集合
        set<int> s2(s1);
        // 两个集合大小相同
        auto it1 = s1.begin();
        auto it2 = s2.begin();
        while (it1 != s1.end() && it2 != s2.end())
        {
            // 对应元素相同
            EXPECT_EQ(*it1, *it2);
            ++it1;
            ++it2;
        }
        EXPECT_EQ(it1, s1.end());
        EXPECT_EQ(it2, s2.end());
        // 修改原集合，不影响拷贝
        s1.insert(10);
        EXPECT_EQ(s1.find(10) != s1.end(), true);
        EXPECT_EQ(s2.find(10), s2.end());
    }

    // 测试赋值运算符重载
    TEST(SetTest, AssignmentOperator)
    {
        set<int> s1;
        s1.insert(2);
        s1.insert(4);
        set<int> s2;
        s2.insert(100);
        // 使用赋值运算符
        s2 = s1;
        // s2应包含与s1相同的元素
        auto it1 = s1.begin();
        auto it2 = s2.begin();
        while (it1 != s1.end() && it2 != s2.end())
        {
            EXPECT_EQ(*it1, *it2);
            ++it1;
            ++it2;
        }
        EXPECT_EQ(it1, s1.end());
        EXPECT_EQ(it2, s2.end());
        // 修改已赋值集合，不影响原集合
        s2.erase(2);
        EXPECT_EQ(s2.find(2), s2.end());
        EXPECT_NE(s1.find(2), s1.end());
    }

    // 测试移动构造
    TEST(SetTest, MoveConstructor)
    {
        set<int> s1;
        s1.insert(1);
        s1.insert(2);
        s1.insert(3);

        set<int> s2(std::move(s1));
        EXPECT_EQ(s2.size(), 3);
        auto it = s2.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
        EXPECT_EQ(s1.size(), 0);
    }

    // 测试移动赋值
    TEST(SetTest, MoveAssignment)
    {
        set<int> s1;
        s1.insert(1);
        s1.insert(2);
        s1.insert(3);

        set<int> s2;
        s2 = std::move(s1);
        EXPECT_EQ(s2.size(), 3);
        auto it = s2.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
        EXPECT_EQ(s1.size(), 0);
    }

    // 测试 initializer_list 构造
    TEST(SetTest, InitializerListConstructor)
    {
        set<int> s = {1, 2, 3};
        EXPECT_EQ(s.size(), 3);
        auto it = s.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
    }

    TEST(SetTest, EmplaceUnique)
    {
        set<int> s;
        auto result = s.emplace(42);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(*result.first, 42);
        EXPECT_EQ(s.find(42), result.first);
    }
} // namespace zstl
