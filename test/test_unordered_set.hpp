#pragma once
#include "../container/unordered_set.hpp"
namespace zstl
{
    // fixture 用于不同类型的 unordered_set 测试
    class UnorderedSetTest : public ::testing::Test
    {
    protected:
        void SetUp() override {}
        void TearDown() override {}
    };

    // 测试初始状态
    TEST_F(UnorderedSetTest, EmptyOnConstruction)
    {
        unordered_set<int> us;
        // 新建后应为空
        EXPECT_TRUE(us.empty());
        EXPECT_EQ(us.size(), 0);
    }

    // 测试插入元素及重复插入
    TEST_F(UnorderedSetTest, InsertAndDuplicate)
    {
        unordered_set<int> us;
        auto result1 = us.insert(42);
        // 第一次插入应成功
        EXPECT_TRUE(result1.second);
        EXPECT_EQ(*result1.first, 42);
        EXPECT_EQ(us.size(), 1);

        auto result2 = us.insert(42);
        // 重复插入应失败
        EXPECT_FALSE(result2.second);
        EXPECT_EQ(us.size(), 1);
    }

    // 测试查找元素
    TEST_F(UnorderedSetTest, FindExistingAndNonExisting)
    {
        unordered_set<int> us;
        us.insert(5);
        us.insert(10);

        // 查找已插入的元素
        auto it5 = us.find(5);
        EXPECT_NE(it5, us.end());
        EXPECT_EQ(*it5, 5);

        // 查找不存在的元素
        auto it7 = us.find(7);
        EXPECT_EQ(it7, us.end());
    }

    // 测试删除操作
    TEST_F(UnorderedSetTest, EraseElements)
    {
        unordered_set<int> us;
        us.insert(1);
        us.insert(2);
        us.insert(3);
        EXPECT_EQ(us.size(), 3);

        // 删除存在的元素
        EXPECT_EQ(us.erase(2), 1u);
        EXPECT_EQ(us.size(), 2);
        EXPECT_EQ(us.find(2), us.end());

        // 删除不存在的元素
        EXPECT_EQ(us.erase(42), 0u);
        EXPECT_EQ(us.size(), 2);
    }

    // 测试 clear 和 empty
    TEST_F(UnorderedSetTest, ClearAndEmpty)
    {
        unordered_set<int> us;
        us.insert(100);
        us.insert(200);
        EXPECT_FALSE(us.empty());
        EXPECT_EQ(us.size(), 2);

        us.clear();
        // 清空后应为空
        EXPECT_TRUE(us.empty());
        EXPECT_EQ(us.size(), 0);
    }

    // 测试迭代器遍历
    TEST_F(UnorderedSetTest, IteratorTraverse)
    {
        unordered_set<int> us;
        for (int i = 0; i < 5; ++i)
            us.insert(i * 2);

        unordered_set<int> collected;
        for (int u : us)
        {
            collected.insert(u);
        }

        int cnt = 0;
        for (int it : collected)
        {
            EXPECT_EQ(it, 2 * (cnt++));
        }

        // 验证所有元素都被遍历
        for (int i = 0; i < 5; ++i)
        {
            EXPECT_EQ(collected.count(i * 2), 1u);
        }
    }

    // 测试 swap 功能
    TEST_F(UnorderedSetTest, SwapSets)
    {
        unordered_set<int> a;
        unordered_set<int> b;
        a.insert(7);
        a.insert(8);
        b.insert(100);

        a.swap(b);
        EXPECT_EQ(a.size(), 1);
        EXPECT_EQ(b.size(), 2);
        EXPECT_EQ(*a.begin(), 100);
        EXPECT_TRUE(b.find(7) != b.end());
        EXPECT_TRUE(b.find(8) != b.end());
    }

    // 测试自定义 string 类型
    TEST_F(UnorderedSetTest, ZstlStringType)
    {
        unordered_set<string> us;
        string s1("hello");
        string s2("world");

        EXPECT_TRUE(us.insert(s1).second);
        EXPECT_TRUE(us.insert(s2).second);
        EXPECT_EQ(us.size(), 2);

        // 查找和删除
        EXPECT_NE(us.find(s1), us.end());
        EXPECT_EQ(us.erase(s1), 1u);
        EXPECT_EQ(us.find(s1), us.end());
    }

    // 测试 size() 成员函数
    TEST_F(UnorderedSetTest, SizeMemberFunction)
    {
        unordered_set<int> us;
        // 初始 size 应为 0
        EXPECT_EQ(us.size(), 0);

        // 插入多个元素后 size 应反映元素数量
        for (int i = 1; i <= 3; ++i)
        {
            us.insert(i);
        }
        EXPECT_EQ(us.size(), 3);

        // 删除一个元素后 size 应更新
        us.erase(2);
        EXPECT_EQ(us.size(), 2);
    }

    // 测试 empty() 成员函数
    TEST_F(UnorderedSetTest, EmptyMemberFunction)
    {
        unordered_set<int> us;
        // 初始应为空
        EXPECT_TRUE(us.empty());

        // 插入元素后不应为空
        us.insert(100);
        EXPECT_FALSE(us.empty());

        // clear 后应为空
        us.clear();
        EXPECT_TRUE(us.empty());
    }

    // 测试移动构造
    TEST_F(UnorderedSetTest, MoveConstructor)
    {
        unordered_set<int> us1;
        us1.insert(1);
        us1.insert(2);
        us1.insert(3);

        unordered_set<int> us2(std::move(us1));
        EXPECT_EQ(us2.size(), 3);
        EXPECT_EQ(us2.count(1), 1);
        EXPECT_EQ(us2.count(2), 1);
        EXPECT_EQ(us2.count(3), 1);
        EXPECT_EQ(us1.size(), 0);
    }

    // 测试移动赋值
    TEST_F(UnorderedSetTest, MoveAssignment)
    {
        unordered_set<int> us1;
        us1.insert(1);
        us1.insert(2);
        us1.insert(3);

        unordered_set<int> us2;
        us2 = std::move(us1);
        EXPECT_EQ(us2.size(), 3);
        EXPECT_EQ(us2.count(1), 1);
        EXPECT_EQ(us2.count(2), 1);
        EXPECT_EQ(us2.count(3), 1);
        EXPECT_EQ(us1.size(), 0);
    }

    // 测试 initializer_list 构造
    TEST_F(UnorderedSetTest, InitializerListConstructor)
    {
        unordered_set<int> us = {1, 2, 3};
        EXPECT_EQ(us.size(), 3);
        EXPECT_EQ(us.count(1), 1);
        EXPECT_EQ(us.count(2), 1);
        EXPECT_EQ(us.count(3), 1);
    }

    TEST_F(UnorderedSetTest, EmplaceUnique)
    {
        unordered_set<int> us;
        auto result = us.emplace(42);
        EXPECT_TRUE(result.second);
        EXPECT_EQ(*result.first, 42);
        EXPECT_EQ(us.find(42), result.first);
    }
}