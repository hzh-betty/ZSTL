#pragma once
#include <gtest/gtest.h>
#include "../include/stack.hpp"
#include "../include/list.hpp"
#include "../include/vector.hpp"

namespace zstl
{
    // 测试 zstl::stack 类
    TEST(StackTest, PushAndTop)
    {
        stack<int> s;

        // 测试 push 操作
        s.push(10);
        s.push(20);
        s.push(30);

        // 测试栈顶元素是否正确
        EXPECT_EQ(s.top(), 30); // 期望栈顶元素为 30
    }

    TEST(StackTest, Pop)
    {
        stack<int> s;

        // 添加元素
        s.push(10);
        s.push(20);
        s.push(30);

        // 测试 pop 操作
        s.pop();
        EXPECT_EQ(s.top(), 20); // 期望栈顶元素为 20

        s.pop();
        EXPECT_EQ(s.top(), 10); // 期望栈顶元素为 10

        s.pop();
        EXPECT_TRUE(s.empty()); // 期望栈为空
    }

    TEST(StackTest, Empty)
    {
        stack<int> s;

        // 初始时栈应该为空
        EXPECT_TRUE(s.empty());

        // 添加元素后栈不为空
        s.push(100);
        EXPECT_FALSE(s.empty());
    }

    TEST(StackTest, Size)
    {
        stack<int> s;

        // 栈空时大小为 0
        EXPECT_EQ(s.size(), 0);

        // 添加元素后大小应该增加
        s.push(10);
        s.push(20);
        EXPECT_EQ(s.size(), 2);

        // 弹出元素后大小应该减少
        s.pop();
        EXPECT_EQ(s.size(), 1);
    }

    TEST(StackTest, ConstTop)
    {
        stack<int> s;
        s.push(10);
        s.push(20);

        // 使用 const 对象获取栈顶元素，确保 const top() 方法工作正常
        const stack<int> &const_s = s;
        EXPECT_EQ(const_s.top(), 20);
    }

    TEST(StackTest, MultipleContainers)
    {
        // 测试使用自定义容器
        stack<int, list<int>> s;
        s.push(10);
        s.push(20);
        EXPECT_EQ(s.top(), 20);

        stack<int, vector<int>> ss;
        ss.push(10);
        ss.push(20);
        EXPECT_EQ(ss.top(), 20);
    }

    // 测试移动构造
    TEST(StackTest, MoveConstructor)
    {
        stack<int> s1;
        s1.push(1);
        s1.push(2);
        s1.push(3);

        stack<int> s2(std::move(s1));
        EXPECT_EQ(s2.size(), 3);
        EXPECT_EQ(s2.top(), 3);
        s2.pop();
        EXPECT_EQ(s2.top(), 2);
        s2.pop();
        EXPECT_EQ(s2.top(), 1);
        EXPECT_TRUE(s1.empty());
    }

    // 测试移动赋值
    TEST(StackTest, MoveAssignment)
    {
        stack<int> s1;
        s1.push(1);
        s1.push(2);
        s1.push(3);

        stack<int> s2;
        s2 = std::move(s1);
        EXPECT_EQ(s2.size(), 3);
        EXPECT_EQ(s2.top(), 3);
        s2.pop();
        EXPECT_EQ(s2.top(), 2);
        s2.pop();
        EXPECT_EQ(s2.top(), 1);
        EXPECT_TRUE(s1.empty());
    }

    TEST(StackTest, Emplace)
    {
        stack<int> s;
        s.emplace(42);
        EXPECT_EQ(s.top(), 42);
        s.pop();
        EXPECT_TRUE(s.empty());
    }

};