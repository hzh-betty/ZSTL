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
// 测试工具宏：验证迭代器特性
#define TEST_ITERATOR_TRAITS(ContainerType, ExpectedCategory, ValueType)                  \
    using Iter1 = typename ContainerType::iterator;                                       \
    using Traits1 = zstl::iterator_traits<Iter1>;                                         \
    EXPECT_TRUE((std::is_same_v<typename Traits1::iterator_category, ExpectedCategory>)); \
    EXPECT_TRUE((std::is_same_v<typename Traits1::value_type, ValueType>));               \
    EXPECT_TRUE((std::is_same_v<typename Traits1::difference_type, ptrdiff_t>));          \
    EXPECT_TRUE((std::is_same_v<typename Traits1::pointer, ValueType *>));                \
    EXPECT_TRUE((std::is_same_v<typename Traits1::reference, ValueType &>))

// 测试 const 迭代器特性
#define TEST_CONST_ITERATOR_TRAITS(ContainerType, ExpectedCategory, ValueType)            \
    using Iter2 = typename ContainerType::const_iterator;                                 \
    using Traits2 = zstl::iterator_traits<Iter2>;                                         \
    EXPECT_TRUE((std::is_same_v<typename Traits2::iterator_category, ExpectedCategory>)); \
    EXPECT_TRUE((std::is_same_v<typename Traits2::value_type, ValueType>));               \
    EXPECT_TRUE((std::is_same_v<typename Traits2::difference_type, ptrdiff_t>));          \
    EXPECT_TRUE((std::is_same_v<typename Traits2::pointer, const ValueType *>));          \
    EXPECT_TRUE((std::is_same_v<typename Traits2::reference, const ValueType &>))

    TEST(IteratorTraitsTest, Vector)
    {
        using Container = zstl::vector<int>;
        TEST_ITERATOR_TRAITS(Container, zstl::random_access_iterator_tag, int);
        TEST_CONST_ITERATOR_TRAITS(Container, zstl::random_access_iterator_tag, int);
    }
    TEST(IteratorTraitsTest, List)
    {
        using Container = zstl::list<int>;
        TEST_ITERATOR_TRAITS(Container, zstl::bidirectional_iterator_tag, int);
        TEST_CONST_ITERATOR_TRAITS(Container, zstl::bidirectional_iterator_tag, int);
    }
    TEST(IteratorTraitsTest, Deque)
    {
        using Container = zstl::deque<int>;
        TEST_ITERATOR_TRAITS(Container, zstl::random_access_iterator_tag, int);
        TEST_CONST_ITERATOR_TRAITS(Container, zstl::random_access_iterator_tag, int);
    }
    TEST(IteratorTraitsTest, ForwardList)
    {
        using Container = zstl::forward_list<int>;
        TEST_ITERATOR_TRAITS(Container, zstl::forward_iterator_tag, int);
        TEST_CONST_ITERATOR_TRAITS(Container, zstl::forward_iterator_tag, int);
    }

    TEST(IteratorTraitsTest, UnorderedSet)
    {
        using Container = zstl::unordered_set<string>;
        TEST_CONST_ITERATOR_TRAITS(Container, zstl::forward_iterator_tag, string);
    }

    TEST(IteratorTraitsTest, UnorderedMap)
    {
        using Container = zstl::unordered_map<int, string>;
        using PairType = std::pair<const int, string>;
        TEST_ITERATOR_TRAITS(Container, zstl::forward_iterator_tag, PairType);
        TEST_CONST_ITERATOR_TRAITS(Container, zstl::forward_iterator_tag, PairType);
    }
    TEST(IteratorTraitsTest, Map)
    {
        using Container = zstl::map<int, string>;
        using PairType = std::pair<const int, string>;
        TEST_ITERATOR_TRAITS(Container, zstl::bidirectional_iterator_tag, PairType);
        TEST_CONST_ITERATOR_TRAITS(Container, zstl::bidirectional_iterator_tag, PairType);
    }

    TEST(IteratorTraitsTest, Set)
    {
        using Container = zstl::set<string>;
        TEST_CONST_ITERATOR_TRAITS(Container, zstl::bidirectional_iterator_tag, string);
    }

    TEST(IteratorTraitsTest, Array)
    {
        using Container = zstl::array<int, 10>;
        TEST_ITERATOR_TRAITS(Container, zstl::random_access_iterator_tag, int);
        TEST_CONST_ITERATOR_TRAITS(Container, zstl::random_access_iterator_tag, int);
    }

    // 随机访问迭代器：vector<int>::iterator
    TEST(IteratorTraitsTest, Distance_RandomAccess)
    {
        zstl::vector<int> v{10, 20, 30, 40, 50};
        auto d1 = zstl::distance(v.begin(), v.begin() + 3);
        EXPECT_EQ(d1, 3);

        // 反向距离也应正确（random access 支持 last-first）
        auto d2 = zstl::distance(v.begin() + 4, v.begin() + 1);
        EXPECT_EQ(d2, -3);
    }

    // 双向迭代器：list<int>::iterator
    TEST(IteratorTraitsTest, Distance_Bidirectional)
    {
        zstl::list<int> lst{1, 2, 3, 4, 5};
        // forward distance
        auto d1 = zstl::distance(lst.begin(), zstl::next(lst.begin(), 4));
        EXPECT_EQ(d1, 4);
        // backward distance
        auto it1 = zstl::next(lst.begin(), 1);
        auto it2 = zstl::next(lst.begin(), 4);
        auto d2 = zstl::distance(it1, it2);
        EXPECT_EQ(d2, 3);
    }

    // 前向迭代器：forward_list<int>::iterator
    TEST(IteratorTraitsTest, Distance_Forward)
    {
        zstl::forward_list<int> fl{5, 6, 7, 8};
        auto first = fl.begin();
        auto last = fl.end();
        auto d = zstl::distance(first, last);
        EXPECT_EQ(d, 4);
    }

    // 原生指针
    TEST(IteratorTraitsTest, Distance_Pointer)
    {
        int a[] = {0, 1, 2, 3, 4};
        int *b = a;
        int *e = a + 5;
        auto d = zstl::distance(b, e);
        EXPECT_EQ(d, 5);
    }

    // —— 测试 advance ——

    // 随机访问迭代器
    TEST(IteratorTraitsTest, Advance_RandomAccess)
    {
        zstl::vector<char> v{'a', 'b', 'c', 'd', 'e'};
        auto it = v.begin();
        zstl::advance(it, 2);
        EXPECT_EQ(*it, 'c');
        zstl::advance(it, -1);
        EXPECT_EQ(*it, 'b');
    }

    // 双向迭代器
    TEST(IteratorTraitsTest, Advance_Bidirectional)
    {
        zstl::list<int> lst{10, 20, 30, 40};
        auto it = lst.begin();
        zstl::advance(it, 3);
        EXPECT_EQ(*it, 40);
        zstl::advance(it, -2);
        EXPECT_EQ(*it, 20);
    }

    // 前向迭代器（只能正向移动）
    TEST(IteratorTraitsTest, Advance_Forward)
    {
        zstl::forward_list<int> fl{100, 200, 300, 400};
        auto it = fl.begin();
        zstl::advance(it, 2);
        EXPECT_EQ(*it, 300);
        // 对于负数步进，forward 只能向前，因此行为等同于 advance(it, 0)
        zstl::advance(it, -1);
        EXPECT_EQ(*it, 300);
    }

    // 原生指针
    TEST(IteratorTraitsTest, Advance_Pointer)
    {
        int arr[] = {7, 8, 9, 10};
        int *p = arr;
        zstl::advance(p, 3);
        EXPECT_EQ(*p, 10);
        zstl::advance(p, -2);
        EXPECT_EQ(*p, 8);
    }
};