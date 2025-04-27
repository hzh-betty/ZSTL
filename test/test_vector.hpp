#pragma once
#include <gtest/gtest.h>
#include "../include/vector.hpp"

namespace zstl
{
    // 测试 push_back() 和 pop_back() 基本功能
    TEST(VectorTest, PushBackPopBack)
    {
        vector<int> vec;
        EXPECT_TRUE(vec.empty());

        // 插入元素
        vec.push_back(10);
        vec.push_back(20);
        EXPECT_EQ(vec.size(), 2u);
        EXPECT_EQ(vec[0], 10);
        EXPECT_EQ(vec[1], 20);

        // 删除尾部元素
        vec.pop_back();
        EXPECT_EQ(vec.size(), 1u);
        EXPECT_EQ(vec[0], 10);

        vec.pop_back();
        EXPECT_TRUE(vec.empty());
    }

    // 测试拷贝构造和赋值操作符
    TEST(VectorTest, CopyAndAssignment)
    {
        vector<zstl::string> v1;
        v1.push_back("a");
        v1.push_back("b");
        vector<zstl::string> v2(v1); // 拷贝构造
        EXPECT_EQ(v2.size(), 2u);
        EXPECT_EQ(v2[0], "a");
        EXPECT_EQ(v2[1], "b");

        vector<zstl::string> v3;
        v3 = v1; // 赋值运算符
        EXPECT_EQ(v3.size(), 2u);
        EXPECT_EQ(v3[0], "a");
        EXPECT_EQ(v3[1], "b");
    }

    // 测试范围构造函数
    TEST(VectorTest, RangeConstructor)
    {
        int arr[] = {1, 2, 3, 4, 5};
        vector<int> vec(arr, arr + 5);
        EXPECT_EQ(vec.size(), 5u);
        for (int i = 0; i < 5; ++i)
        {
            EXPECT_EQ(vec[i], arr[i]);
        }
    }

    // 测试 resize 操作（扩容和缩小）
    TEST(VectorTest, Resize)
    {
        vector<int> vec;
        // 先添加一些数据
        for (int i = 1; i <= 3; ++i)
        {
            vec.push_back(i); // 得到 1,2,3
        }
        EXPECT_EQ(vec.size(), 3u);

        // 扩容至5个元素，新元素初始化为 0
        vec.resize(5, 0);
        EXPECT_EQ(vec.size(), 5u);
        EXPECT_EQ(vec[0], 1);
        EXPECT_EQ(vec[1], 2);
        EXPECT_EQ(vec[2], 3);
        EXPECT_EQ(vec[3], 0);
        EXPECT_EQ(vec[4], 0);

        // 缩小至2个元素
        vec.resize(2);
        EXPECT_EQ(vec.size(), 2u);
        EXPECT_EQ(vec[0], 1);
        EXPECT_EQ(vec[1], 2);
    }

    // 测试 insert 操作
    TEST(VectorTest, Insert)
    {
        vector<int> vec;
        // 初始化 vector 为 1,2,3
        for (int i = 1; i <= 3; ++i)
        {
            vec.push_back(i);
        }
        // 在第二个位置插入 100：期望结果为 1,100,2,3
        auto it = vec.begin() + 1;
        vec.insert(it, 100);
        EXPECT_EQ(vec.size(), 4u);
        EXPECT_EQ(vec[0], 1);
        EXPECT_EQ(vec[1], 100);
        EXPECT_EQ(vec[2], 2);
        EXPECT_EQ(vec[3], 3);

        // 在容器开始位置插入元素
        vec.insert(vec.begin(), 200); // 期望 200,1,100,2,3
        EXPECT_EQ(vec.size(), 5u);
        EXPECT_EQ(vec[0], 200);
        EXPECT_EQ(vec[1], 1);
    }

    // 测试 erase 操作
    TEST(VectorTest, Erase)
    {
        vector<int> vec;
        // 填充 vector 为 0,1,2,3,4
        for (int i = 0; i < 5; ++i)
        {
            vec.push_back(i);
        }
        // 删除第三个元素（下标为2，值为2），期望结果为 0,1,3,4
        auto it = vec.begin() + 2;
        vec.erase(it);
        EXPECT_EQ(vec.size(), 4u);
        EXPECT_EQ(vec[0], 0);
        EXPECT_EQ(vec[1], 1);
        EXPECT_EQ(vec[2], 3);
        EXPECT_EQ(vec[3], 4);
    }

    // 测试 clear 操作
    TEST(VectorTest, Clear)
    {
        vector<int> vec;
        for (int i = 0; i < 10; ++i)
        {
            vec.push_back(i);
        }
        EXPECT_FALSE(vec.empty());
        vec.clear();
        EXPECT_EQ(vec.size(), 0u);
        EXPECT_TRUE(vec.empty());
    }

    // 测试 swap 操作
    TEST(VectorTest, Swap)
    {
        vector<int> vec1;
        vector<int> vec2;
        // vec1: 1,2,3
        for (int i = 1; i <= 3; ++i)
        {
            vec1.push_back(i);
        }
        // vec2: 10,20
        for (int i = 1; i <= 2; ++i)
        {
            vec2.push_back(10 * i);
        }

        vec1.swap(vec2);
        // 交换后，vec1 应为 10,20，vec2 应为 1,2,3
        EXPECT_EQ(vec1.size(), 2u);
        EXPECT_EQ(vec2.size(), 3u);
        EXPECT_EQ(vec1[0], 10);
        EXPECT_EQ(vec1[1], 20);
        EXPECT_EQ(vec2[0], 1);
        EXPECT_EQ(vec2[1], 2);
        EXPECT_EQ(vec2[2], 3);
    }

    // 测试迭代器遍历
    TEST(VectorTest, Iterator)
    {
        vector<int> vec;
        for (int i = 0; i < 5; ++i)
        {
            vec.push_back(i);
        }
        int expected = 0;
        for (auto it = vec.begin(); it != vec.end(); ++it)
        {
            EXPECT_EQ(*it, expected++);
        }
    }
};
