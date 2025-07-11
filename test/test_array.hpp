#pragma once
#include "../container/array.hpp"
#include <gtest/gtest.h>
namespace zstl
{
    // 测试 size(), max_size() 和 empty() 接口
    TEST(ArrayTest, Capacity)
    {
        array<int, 5> a;
        EXPECT_EQ(a.max_size(), 5u); // max_size 同 size
        EXPECT_FALSE(a.empty());     // N!=0 时非空

    }

    // 测试 fill() 和 front(), back()
    TEST(ArrayTest, FillFrontBack)
    {
        array<int, 4> a;
        a.fill(7); // 全部填充为 7
        // 检查 front 和 back
        EXPECT_EQ(a.front(), 7);
        EXPECT_EQ(a.back(), 7);
        // 检查每个元素
        for (size_t i = 0; i < a.max_size(); ++i)
        {
            EXPECT_EQ(a[i], 7);
        }
    }

    // 测试 operator[]（断言版）和 data()
    TEST(ArrayTest, SubscriptAndData)
    {
        array<int, 3> a;
        int *p = a.data(); // 获取原始指针
        p[0] = 1;
        p[1] = 2;
        p[2] = 3;
        // operator[] 应同 data()[i]
        EXPECT_EQ(a[0], 1);
        EXPECT_EQ(a[1], 2);
        EXPECT_EQ(a[2], 3);
    }

    // 测试 at() 的范围检查
    TEST(ArrayTest, AtThrows)
    {
        array<int, 2> a;
        a.fill(5);
        EXPECT_NO_THROW(a.at(0)); // 有效下标不抛
        EXPECT_NO_THROW(a.at(1));
        EXPECT_THROW(a.at(2), std::out_of_range); // 越界应抛 out_of_range
        EXPECT_THROW(a.at(100), std::out_of_range);
    }

    // 测试迭代器 begin()/end() 遍历
    TEST(ArrayTest, IteratorTraversal)
    {
        array<int, 3> a;
        a.fill(9);
        size_t count = 0;
        for (int & it : a)
        {
            EXPECT_EQ(it, 9);
            ++count;
        }
        EXPECT_EQ(count, a.max_size()); // 遍历次数应等于 size
    }

    // 测试 const_iterator
    TEST(ArrayTest, ConstIterator)
    {
        const array<char, 4> a = []()
        { array<char,4> t; t.fill('x'); return t; }();
        size_t cnt = 0;
        for (char it : a)
        {
            EXPECT_EQ(it, 'x');
            ++cnt;
        }
        EXPECT_EQ(cnt, a.max_size());
    }

    // 测试 initializer_list 构造（注意：array 没有移动构造和移动赋值）
    TEST(ArrayTest, InitializerListConstructor)
    {
        array<int, 3> a = {1, 2, 3};
        EXPECT_EQ(a.max_size(), 3u);
        EXPECT_EQ(a[0], 1);
        EXPECT_EQ(a[1], 2);
        EXPECT_EQ(a[2], 3);
    }
} // namespace zstl
