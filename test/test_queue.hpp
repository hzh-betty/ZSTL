#pragma once
#include <gtest/gtest.h>
#include "../include/queue.hpp"
#include "../include/list.hpp"

namespace zstl
{
    // 使用测试夹具（Test Fixture）来共享测试数据
    class QueueTest : public ::testing::Test
    {
    protected:
        // 在每个测试用例之前执行
        void SetUp() override
        {
            q1.push(10);
            q2.push(20);
            q2.push(30);
        }

        // 在每个测试用例之后执行
        void TearDown() override
        {
            // 清理工作（如果需要）
        }

        queue<int> q0; // 空队列
        queue<int> q1; // 含有一个元素的队列
        queue<int> q2; // 含有两个元素的队列
    };

    // 测试队列是否为空
    TEST_F(QueueTest, IsEmptyInitially)
    {
        EXPECT_TRUE(q0.empty());  // 空队列应返回 true
        EXPECT_FALSE(q1.empty()); // 非空队列应返回 false
    }

    // 测试队列大小
    TEST_F(QueueTest, Size)
    {
        EXPECT_EQ(q0.size(), 0); // 空队列大小应为 0
        EXPECT_EQ(q1.size(), 1); // 含有一个元素的队列大小应为 1
        EXPECT_EQ(q2.size(), 2); // 含有两个元素的队列大小应为 2
    }

    // 测试队头元素
    TEST_F(QueueTest, Front)
    {
        EXPECT_EQ(q1.front(), 10); // 队头元素应为 10
        EXPECT_EQ(q2.front(), 20); // 队头元素应为 20
    }

    // 测试队尾元素
    TEST_F(QueueTest, Back)
    {
        EXPECT_EQ(q1.back(), 10); // 队尾元素应为 10
        EXPECT_EQ(q2.back(), 30); // 队尾元素应为 30
    }

    // 测试入队操作
    TEST_F(QueueTest, Push)
    {
        q0.push(100);
        EXPECT_EQ(q0.size(), 1);    // 入队后队列大小应为 1
        EXPECT_EQ(q0.front(), 100); // 队头元素应为 100
        EXPECT_EQ(q0.back(), 100);  // 队尾元素应为 100
    }

    // 测试出队操作
    TEST_F(QueueTest, Pop)
    {
        q2.pop();
        EXPECT_EQ(q2.size(), 1);   // 出队后队列大小应为 1
        EXPECT_EQ(q2.front(), 30); // 队头元素应为 30
    }

    // 测试常量队列的访问
    TEST_F(QueueTest, ConstAccess)
    {
        const queue<int> &const_q = q2;
        EXPECT_EQ(const_q.front(), 20); // 队头元素应为 20
        EXPECT_EQ(const_q.back(), 30);  // 队尾元素应为 30
    }

    TEST_F(QueueTest, MultipleContainers)
    {
        // 测试使用自定义容器
        queue<int, zstl::list<int>> s;
        s.push(10);
        s.push(20);
        EXPECT_EQ(s.front(), 10);
        EXPECT_EQ(s.back(), 20);
    }

    // 测试移动构造
    TEST_F(QueueTest, MoveConstructor)
    {
        queue<int> q1;
        q1.push(1);
        q1.push(2);
        q1.push(3);

        queue<int> q2(std::move(q1));
        EXPECT_EQ(q2.size(), 3);
        EXPECT_EQ(q2.front(), 1);
        q2.pop();
        EXPECT_EQ(q2.front(), 2);
        q2.pop();
        EXPECT_EQ(q2.front(), 3);
        EXPECT_EQ(q1.size(), 0);
    }

    // 测试移动赋值
    TEST_F(QueueTest, MoveAssignment)
    {
        queue<int> q1;
        q1.push(1);
        q1.push(2);
        q1.push(3);

        queue<int> q2;
        q2 = std::move(q1);
        EXPECT_EQ(q2.size(), 3);
        EXPECT_EQ(q2.front(), 1);
        q2.pop();
        EXPECT_EQ(q2.front(), 2);
        q2.pop();
        EXPECT_EQ(q2.front(), 3);
        EXPECT_EQ(q1.size(), 0);
    }

    TEST_F(QueueTest, Emplace)
    {
        queue<int> q;
        q.emplace(42);
        EXPECT_EQ(q.front(), 42);
        q.pop();
        EXPECT_TRUE(q.empty());
    }

}
