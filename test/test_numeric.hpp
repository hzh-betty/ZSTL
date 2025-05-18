#pragma once
#include <gtest/gtest.h>
#include "../algorithm/numeric.hpp"
#include "../container/vector.hpp"
#include "../container/list.hpp"
#include "../container/string.hpp"
namespace zstl
{
    // 累加测试
    TEST(NumericTest, Accumulate)
    {
        vector<int> v{1, 2, 3, 4};
        EXPECT_EQ(accumulate(v.begin(), v.end(), 0), 10);
        EXPECT_EQ(accumulate(v.begin(), v.end(), 10), 20);

        // 带二元操作
        EXPECT_EQ(accumulate(v.begin(), v.end(), 1, std::multiplies<>()), 24); // 1*1*2*3*4

        list<int> l{1, 2, 3};
        EXPECT_EQ(accumulate(l.begin(), l.end(), 0), 6);
    }

    // 相邻差分测试
    TEST(NumericTest, AdjacentDifference)
    {
        vector<int> v{1, 3, 6, 10};
        vector<int> out(4);
        adjacent_difference(v.begin(), v.end(), out.begin());
        EXPECT_EQ(out[0], 1);
        EXPECT_EQ(out[1], 2);
        EXPECT_EQ(out[2], 3);
        EXPECT_EQ(out[3], 4);

        // 带二元操作
        vector<int> out2(4);
        adjacent_difference(v.begin(), v.end(), out2.begin(), std::plus<>());
        EXPECT_EQ(out2[0], 1);
        EXPECT_EQ(out2[1], 4);
        EXPECT_EQ(out2[2], 9);
        EXPECT_EQ(out2[3], 16);

        list<int> l{1, 3, 6, 10};
        list<int> lout(4);
        adjacent_difference(l.begin(), l.end(), lout.begin());
        auto it = lout.begin();
        EXPECT_EQ(*it++, 1);
        EXPECT_EQ(*it++, 2);
        EXPECT_EQ(*it++, 3);
        EXPECT_EQ(*it++, 4);
    }

    // 内积测试
    TEST(NumericTest, InnerProduct)
    {
        vector<int> a{1, 2, 3};
        vector<int> b{4, 5, 6};
        EXPECT_EQ(inner_product(a.begin(), a.end(), b.begin(), 0), 32); // 1*4+2*5+3*6

        // 带自定义操作
        auto op1 = std::plus<>();
        auto op2 = std::multiplies<>();
        EXPECT_EQ(inner_product(a.begin(), a.end(), b.begin(), 1, op1, op2), 33); // 1+1*4+2*5+3*6

        list<int> la{1, 2, 3};
        list<int> lb{4, 5, 6};
        EXPECT_EQ(inner_product(la.begin(), la.end(), lb.begin(), 0), 32);
    }

    // 部分和测试
    TEST(NumericTest, PartialSum)
    {
        vector<int> v{1, 2, 3, 4};
        vector<int> out(4);
        partial_sum(v.begin(), v.end(), out.begin());
        EXPECT_EQ(out[0], 1);
        EXPECT_EQ(out[1], 3);
        EXPECT_EQ(out[2], 6);
        EXPECT_EQ(out[3], 10);

        // 带二元操作
        vector<int> out2(4);
        partial_sum(v.begin(), v.end(), out2.begin(), std::multiplies<>());
        EXPECT_EQ(out2[0], 1);
        EXPECT_EQ(out2[1], 2);
        EXPECT_EQ(out2[2], 6);
        EXPECT_EQ(out2[3], 24);

        list<int> l{1, 2, 3, 4};
        list<int> lout(4);
        partial_sum(l.begin(), l.end(), lout.begin());
        auto it = lout.begin();
        EXPECT_EQ(*it++, 1);
        EXPECT_EQ(*it++, 3);
        EXPECT_EQ(*it++, 6);
        EXPECT_EQ(*it++, 10);
    }

    // iota测试
    TEST(NumericTest, Iota)
    {
        vector<int> v(5);
        iota(v.begin(), v.end(), 10);
        EXPECT_EQ(v[0], 10);
        EXPECT_EQ(v[1], 11);
        EXPECT_EQ(v[2], 12);
        EXPECT_EQ(v[3], 13);
        EXPECT_EQ(v[4], 14);

        list<int> l(5);
        iota(l.begin(), l.end(), -2);
        auto it = l.begin();
        EXPECT_EQ(*it++, -2);
        EXPECT_EQ(*it++, -1);
        EXPECT_EQ(*it++, 0);
        EXPECT_EQ(*it++, 1);
        EXPECT_EQ(*it++, 2);
    }

    // 字符串测试
    TEST(NumericTest, StringAccumulate)
    {
        string s = "abc";
        EXPECT_EQ(accumulate(s.begin(), s.end(), 0), int('a') + int('b') + int('c'));
        string s2 = "xyz";
        EXPECT_EQ(inner_product(s.begin(), s.end(), s2.begin(), 0), int('a') * int('x') + int('b') * int('y') + int('c') * int('z'));
    }
}
