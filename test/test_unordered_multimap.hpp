#pragma once
#include "../container/unordered_map.hpp"
#include "../container/string.hpp"
#include <gtest/gtest.h>
namespace zstl
{
    // 测试 Fixture（可选）
    class UnorderedMultimapTest : public ::testing::Test
    {
    protected:
        // 每个 TEST 之前会执行
        void SetUp() override
        {
            // 可在此初始化共享资源
        }
        // 每个 TEST 之后会执行
        void TearDown() override
        {
            // 可在此清理
        }
    };

    // 1. 默认构造应为空
    TEST_F(UnorderedMultimapTest, DefaultConstructorIsEmpty)
    {
        unordered_multimap<int, int> m;
        EXPECT_TRUE(m.empty()); // empty() 应为 true
        EXPECT_EQ(m.size(), 0u);
    }

    // 2. initializer_list 构造
    TEST_F(UnorderedMultimapTest, InitializerListConstructor)
    {
        unordered_multimap<int, char> m{{1, 'a'}, {2, 'b'}, {1, 'c'}};
        // 应包含 3 个元素，允许键重复
        EXPECT_EQ(m.size(), 3u);
        EXPECT_EQ(m.count(1), 2u); // count() 应返回 2
        EXPECT_EQ(m.count(2), 1u);
    }


    // 4. insert_duplicate（insert）允许重复键
    TEST_F(UnorderedMultimapTest, InsertAllowsDuplicates)
    {
        unordered_multimap<int, int> m;
        m.insert({5, 5});
        m.insert({5, 5});
        EXPECT_EQ(m.count(5), 2u);
    }

    // 5. find 应能找到元素
    TEST_F(UnorderedMultimapTest, FindReturnsValidIterator)
    {
        unordered_multimap<int, char> m{{1, 'x'}, {2, 'y'}};
        auto it = m.find(2);
        ASSERT_NE(it, m.end());
        EXPECT_EQ(it->first, 2);
        EXPECT_EQ(it->second, 'y');
    }

    // 6. equal_range 应遍历所有同键元素
    TEST_F(UnorderedMultimapTest, EqualRangeIteratesAll)
    {
        unordered_multimap<int, char> m{{1, 'a'}, {1, 'b'}, {1, 'd'}, {2, 'z'}};
        auto range = m.equal_range(1); // equal_range
        size_t cnt = 0;
        for (auto iter = range.first; iter != range.second; ++iter)
        {
            ++cnt;
            EXPECT_EQ(iter->first, 1);
        }
        EXPECT_EQ(cnt, 3u);
    }

    // 7. erase(key) 删除所有该键元素，返回删除数量
    TEST_F(UnorderedMultimapTest, EraseByKey)
    {
        unordered_multimap<int, int> m;
        m.emplace(10, 100);
        m.emplace(10, 200);
        size_t removed = m.erase(10); // erase(key)
        EXPECT_EQ(removed, 2u);
        EXPECT_EQ(m.count(10), 0u);
    }

    // 8. erase(iterator) 与 erase(range)
    TEST_F(UnorderedMultimapTest, EraseByIterator)
    {
        unordered_multimap<int, char> m{{1, 'a'}, {2, 'b'}, {3, 'c'}};
        auto it2 = m.find(2);
        ASSERT_NE(it2, m.end());
        m.erase(it2);
        EXPECT_EQ(m.count(2), 0u);

        // 区间删除
        auto r = m.equal_range(1);
        m.erase(r.first, r.second);
        EXPECT_EQ(m.count(1), 0u);
    }

    // 9. clear 后应为空
    TEST_F(UnorderedMultimapTest, ClearEmptiesContainer)
    {
        unordered_multimap<int, int> m;
        m.emplace(1, 1);
        m.emplace(2, 2);
        m.clear(); // clear 方法
        EXPECT_TRUE(m.empty());
    }

    // 10. swap 交换内容
    TEST_F(UnorderedMultimapTest, SwapExchangesContents)
    {
        unordered_multimap<int, char> a{{1, 'a'}};
        unordered_multimap<int, char> b{{2, 'b'}, {2, 'c'}};
        a.swap(b); // swap 方法
        EXPECT_EQ(a.count(2), 2u);
        EXPECT_EQ(b.count(1), 1u);
    }

    // 11. 移动构造与移动赋值
    TEST_F(UnorderedMultimapTest, MoveConstructorAndAssignment)
    {
        unordered_multimap<int, int> src;
        src.emplace(7, 70);
        // 移动构造
        unordered_multimap<int, int> moved_ctor(std::move(src));
        EXPECT_EQ(moved_ctor.count(7), 1u);
        EXPECT_TRUE(src.empty()); // 源应变为空

        // 移动赋值
        unordered_multimap<int, int> src2;
        src2.emplace(8, 80);
        unordered_multimap<int, int> moved_assign;
        moved_assign = std::move(src2);
        EXPECT_EQ(moved_assign.count(8), 1u);
        EXPECT_TRUE(src2.empty());
    }

    // 12. emplace 原位构造并返回迭代器
    TEST_F(UnorderedMultimapTest, EmplaceInsertsElement)
    {
        unordered_multimap<string, double> m;
        auto p = m.emplace("pi", 3.14); // emplace 返回 iterator
        EXPECT_EQ(p->first, "pi");
        EXPECT_DOUBLE_EQ(p->second, 3.14);
        EXPECT_EQ(m.count("pi"), 1u);
    }
}