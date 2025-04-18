#include <gtest/gtest.h>
#include "../include/priority_queue.hpp"
#include "../include/deque.hpp"

namespace zstl {

    // 测试用例1：测试默认构造和push、pop、top、size等基本操作
    TEST(PriorityQueueTest, BasicOperations) {
        priority_queue<int> pq;

        // 测试队列为空
        EXPECT_TRUE(pq.empty());
        EXPECT_EQ(pq.size(), 0);

        // 插入元素并测试top和size
        pq.push(10);
        EXPECT_FALSE(pq.empty());
        EXPECT_EQ(pq.size(), 1);
        EXPECT_EQ(pq.top(), 10);

        pq.push(20);
        pq.push(15);
        EXPECT_EQ(pq.size(), 3);
        EXPECT_EQ(pq.top(), 20);  // 因为默认使用less，所以应该是最大值

        // 弹出元素
        pq.pop();
        EXPECT_EQ(pq.top(), 15);
        pq.pop();
        EXPECT_EQ(pq.top(), 10);
        pq.pop();

        // 队列为空
        EXPECT_TRUE(pq.empty());
        EXPECT_EQ(pq.size(), 0);
    }

    // 测试用例2：测试使用范围构造队列
    TEST(PriorityQueueTest, RangeConstructor) {
        std::vector<int> vec = {5, 1, 8, 3, 7};
        priority_queue<int> pq(vec.begin(), vec.end());

        // 由于默认使用less比较器，所以应该是最大堆
        EXPECT_EQ(pq.top(), 8);
    }

    // 测试用例3：测试使用自定义比较器(greater)
    TEST(PriorityQueueTest, CustomComparatorGreater) {
        priority_queue<int, zstl::vector<int>, zstl::greater<int>> pq;

        pq.push(10);
        pq.push(20);
        pq.push(15);

        // 使用greater比较器，应该是最小堆
        EXPECT_EQ(pq.top(), 10);

        pq.pop();
        EXPECT_EQ(pq.top(), 15);
        pq.pop();
        EXPECT_EQ(pq.top(), 20);
        pq.pop();
    }

    // 测试用例4：测试空队列的操作
    TEST(PriorityQueueTest, EmptyQueue) {
        priority_queue<int> pq;

        // 测试弹出空队列时的行为
        EXPECT_TRUE(pq.empty());
    }

    // 测试用例5：测试队列中只有一个元素的情况
    TEST(PriorityQueueTest, SingleElement) {
        priority_queue<int> pq;

        pq.push(42);

        // 队列中只有一个元素
        EXPECT_FALSE(pq.empty());
        EXPECT_EQ(pq.size(), 1);
        EXPECT_EQ(pq.top(), 42);

        // 弹出后队列为空
        pq.pop();
        EXPECT_TRUE(pq.empty());
        EXPECT_EQ(pq.size(), 0);
    }

    // 测试用例6：测试大数据量的插入和弹出
    TEST(PriorityQueueTest, LargeData) {
        priority_queue<int> pq;

        for (int i = 0; i < 1000; ++i) {
            pq.push(i);
        }

        // 由于默认使用less比较器，队列应该是最大堆，top应该是最大值
        EXPECT_EQ(pq.top(), 999);

        // 弹出1000次，直到队列为空
        for (int i = 999; i >= 0; --i) {
            EXPECT_EQ(pq.top(), i);
            pq.pop();
        }

        EXPECT_TRUE(pq.empty());
        EXPECT_EQ(pq.size(), 0);
    }

    // 测试用例7：测试自定义比较器与空队列的结合
    TEST(PriorityQueueTest, CustomComparatorWithEmptyQueue) {
        priority_queue<int, zstl::deque<int>, zstl::less<int>> pq;

        pq.push(42);

        // 队列中只有一个元素
        EXPECT_FALSE(pq.empty());
        EXPECT_EQ(pq.size(), 1);
        EXPECT_EQ(pq.top(), 42);
    }
}