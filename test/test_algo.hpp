#pragma once
#include <gtest/gtest.h>
#include "../algorithm/algo.hpp"
#include "../container/vector.hpp"
#include "../container/list.hpp"
#include "../container/array.hpp"
namespace zstl
{
    // 测试 max/min/swap
    TEST(AlgoTest, MaxMinSwap)
    {
        // max/min
        EXPECT_EQ(max(3, 5), 5);
        EXPECT_EQ(min(3, 5), 3);

        // swap
        int a = 1, b = 2;
        swap(a, b);
        EXPECT_EQ(a, 2);
        EXPECT_EQ(b, 1);
    }

    // 测试 fill 和 fill_n
    TEST(AlgoTest, FillAndFillN)
    {
        zstl::vector<int> v(5);
        fill(v.begin(), v.end(), 7);
        for (auto x : v)
            EXPECT_EQ(x, 7);

        zstl::array<char, 4> arr;
        fill_n(arr.begin(), arr.max_size(), 'a');
        for (auto c : arr)
            EXPECT_EQ(c, 'a');

        // 测试trivial类型的fill_n优化
        char buf[6];
        fill_n(buf, 6, 'x');
        for (char c : buf)
            EXPECT_EQ(c, 'x');
    }

    // 测试 copy
    TEST(AlgoTest, Copy)
    {
        zstl::vector<int> src{1, 2, 3, 4};
        zstl::vector<int> dst(4);
        copy(src.begin(), src.end(), dst.begin());
        for (int i = 0; i < 4; ++i)
            EXPECT_EQ(dst[i], src[i]);

        // 随机访问迭代器优化
        zstl::vector<int> dst2(4);
        copy(src.begin(), src.begin() + 4, dst2.begin());
        for (int i = 0; i < 4; ++i)
            EXPECT_EQ(dst2[i], src[i]);

        // trivial类型memmove优化
        char s1[] = "abcd";
        char s2[5] = {};
        copy(s1, s1 + 4, s2);
        EXPECT_STREQ(s1, s2);
    }

    // 测试 equal
    TEST(AlgoTest, Equal)
    {
        zstl::vector<int> a{1, 2, 3};
        zstl::vector<int> b{1, 2, 3};
        zstl::vector<int> c{1, 2, 4};
        EXPECT_TRUE(equal(a.begin(), a.end(), b.begin()));
        EXPECT_FALSE(equal(a.begin(), a.end(), c.begin()));

        // 带比较器
        EXPECT_TRUE(equal(a.begin(), a.end(), b.begin(), [](int x, int y)
                          { return x == y; }));
        EXPECT_FALSE(equal(a.begin(), a.end(), c.begin(), [](int x, int y)
                           { return x == y; }));

        // trivial类型memcmp优化
        char s1[] = "hello";
        char s2[] = "hello";
        char s3[] = "hellx";
        EXPECT_TRUE(equal(s1, s1 + 5, s2));
        EXPECT_FALSE(equal(s1, s1 + 5, s3));
    }

    // 测试 lexicographical_compare
    TEST(AlgoTest, LexicographicalCompare)
    {
        zstl::vector<int> a{1, 2, 3};
        zstl::vector<int> b{1, 2, 4};
        zstl::vector<int> c{1, 2, 3, 0};

        EXPECT_TRUE(lexicographical_compare(a.begin(), a.end(), b.begin(), b.end()));  // 3<4
        EXPECT_FALSE(lexicographical_compare(b.begin(), b.end(), a.begin(), a.end())); // 4>3
        EXPECT_TRUE(lexicographical_compare(a.begin(), a.end(), c.begin(), c.end()));  // 长度短

        // trivial类型memcmp优化
        char s1[] = "abc";
        char s2[] = "abd";
        char s3[] = "abc";
        char s4[] = "ab";
        EXPECT_TRUE(lexicographical_compare(s1, s1 + 3, s2, s2 + 3));
        EXPECT_FALSE(lexicographical_compare(s2, s2 + 3, s1, s1 + 3));
        EXPECT_FALSE(lexicographical_compare(s1, s1 + 3, s3, s3 + 3));
        EXPECT_FALSE(lexicographical_compare(s1, s1 + 3, s4, s4 + 2));
        EXPECT_TRUE(lexicographical_compare(s4, s4 + 2, s1, s1 + 3));
    }

    // 测试 count
    TEST(AlgoTest, Count)
    {
        zstl::vector<int> v{1, 2, 2, 3, 2};
        EXPECT_EQ(count(v.begin(), v.end(), 2), 3);
        EXPECT_EQ(count(v.begin(), v.end(), 4), 0);
    }

    // 测试 reverse
    TEST(AlgoTest, Reverse)
    {
        zstl::vector<int> v{1, 2, 3, 4, 5};
        zstl::vector<int> vv(v);
        reverse(v.begin(), v.end());
        for (int i = 0; i < 5; ++i)
            EXPECT_EQ(v[i], 5 - i); // 5,4,3,2,1

        // 双向迭代器自定义类型
        zstl::list<int> l{1, 2, 3};
        zstl::vector<int> lv(l.begin(), l.end());
        reverse(l.begin(), l.end());
        zstl::vector<int> ll(l.begin(), l.end());
        for (int i = 0; i < 3; ++i)
            EXPECT_EQ(ll[i], 3 - i); // 3,2,1
    }
}