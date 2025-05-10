#pragma once
#include <gtest/gtest.h>
#include "../container/string.hpp"
#include "../container/vector.hpp"
#include "../container/list.hpp"
#include "../container/deque.hpp"
#include "../container/forward_list.hpp"
#include "../container/unordered_map.hpp"
#include "../container/unordered_set.hpp"
#include "../container/map.hpp"
#include "../container/set.hpp"
#include "../container/array.hpp"
#include "../iterator/iterator.hpp"
namespace zstl
{
    // vector 反向迭代器基本功能
    TEST(ReverseIterator, VectorBasicTraversal)
    {
        vector<int> v = {1, 2, 3, 4, 5};
        int expected[] = {5, 4, 3, 2, 1};
        size_t idx = 0;
        for (auto it = v.rbegin(); it != v.rend(); ++it)
        {
            EXPECT_EQ(*it, expected[idx]);
            ++idx;
        }
        EXPECT_EQ(idx, v.size());
    }

    // list 反向迭代器增减操作
    TEST(ReverseIterator, IncrementAndDereference)
    {
        list<zstl::string> l;
        l.push_back("a");
        l.push_back("b");
        l.push_back("c");
        auto rit = l.rbegin();
        EXPECT_EQ(*rit, "c");
        ++rit;
        EXPECT_EQ(*rit, "b");
        ++rit;
        EXPECT_EQ(*rit, "a");
        ++rit;
        EXPECT_EQ(rit, l.rend());
    }

    // deque 反向迭代器随机访问
    TEST(ReverseIterator, RandomAccess)
    {
        deque<double> d = {1.1, 2.2, 3.3, 4.4};
        auto rit = d.rbegin();
        EXPECT_DOUBLE_EQ(rit[0], 4.4);
        EXPECT_DOUBLE_EQ(rit[1], 3.3);
        EXPECT_DOUBLE_EQ(rit[2], 2.2);
        EXPECT_DOUBLE_EQ(rit[3], 1.1);
        EXPECT_DOUBLE_EQ(*(rit + 2), 2.2);
        EXPECT_DOUBLE_EQ(*(rit + 3), 1.1);
    }

    // set 反向迭代器顺序
    TEST(ReverseIterator, OrderCheck)
    {
        set<int> s;
        for (int i = 1; i <= 5; ++i)
            s.insert(i);
        int prev = 100;
        for (auto it = s.rbegin(); it != s.rend(); ++it)
        {
            EXPECT_LT(*it, prev);
            prev = *it;
        }
    }

    // map 反向迭代器键值对访问
    TEST(ReverseIterator, KeyValuePair)
    {
        map<int, char> m;
        m[1] = 'a';
        m[2] = 'b';
        m[3] = 'c';
        std::pair<int, char> expected[] = {{3, 'c'}, {2, 'b'}, {1, 'a'}};
        size_t idx = 0;
        for (auto it = m.rbegin(); it != m.rend(); ++it)
        {
            EXPECT_EQ(it->first, expected[idx].first);
            EXPECT_EQ(it->second, expected[idx].second);
            ++idx;
        }
        EXPECT_EQ(idx, m.size());
    }

    // 迭代器与基迭代器转换
    TEST(ReverseIterator, BaseConversion)
    {
        vector<int> v = {10, 20, 30};
        auto rit = v.rbegin();
        auto it = rit.base();
        EXPECT_EQ(it, v.end());
    }

    // multiset 反向迭代器，含重复元素顺序检查
    TEST(ReverseIterator, DuplicatesOrder)
    {
        multiset<int> ms = {1, 2, 2, 3, 3, 3};
        int expected[] = {3, 3, 3, 2, 2, 1};
        size_t idx = 0;
        for (auto it = ms.rbegin(); it != ms.rend(); ++it)
        {
            EXPECT_EQ(*it, expected[idx]);
            ++idx;
        }
        EXPECT_EQ(idx, ms.size());
    }

    // multimap 反向迭代器，键值对访问
    TEST(ReverseIterator, KeyValuePairs)
    {
        multimap<int, char> mm;
        mm.insert({1, 'a'});
        mm.insert({2, 'b'});
        mm.insert({2, 'b'});
        mm.insert({3, 'c'});
        zstl::vector<std::pair<int, char>> forward;
        for (auto &p : mm)
            forward.push_back(p);
        zstl::vector<std::pair<int, char>> expected(forward.rbegin(), forward.rend());
        size_t idx = 0;
        for (auto it = mm.rbegin(); it != mm.rend(); ++it)
        {
            EXPECT_EQ(it->first, expected[idx].first);
            EXPECT_EQ(it->second, expected[idx].second);
            ++idx;
        }
        EXPECT_EQ(idx, mm.size());
    }

    // array 反向迭代器，基本遍历
    TEST(ReverseIterator, ArrayBasicTraversal)
    {
        array<int, 5> a = {{5, 4, 3, 2, 1}};
        int expected[] = {1, 2, 3, 4, 5};
        size_t idx = 0;
        for (auto it = a.rbegin(); it != a.rend(); ++it)
        {
            EXPECT_EQ(*it, expected[idx]);
            ++idx;
        }
        EXPECT_EQ(idx, a.max_size());
    }

};