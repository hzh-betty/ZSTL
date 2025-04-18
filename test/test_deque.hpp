#include <gtest/gtest.h>
#include "../include/deque.hpp" 

// 测试 zstl::deque 类的基本功能
class DequeTest : public ::testing::Test
{
protected:
    zstl::deque<int> d;

    // 每个测试前都会调用此函数
    void SetUp() override
    {
        d.clear(); // 清空 deque 确保每个测试开始时 deque 是空的
    }
};

// 测试默认构造函数
TEST_F(DequeTest, DefaultConstructor)
{
    // 验证 deque 是否为空
    EXPECT_TRUE(d.empty());
    EXPECT_EQ(d.size(), 0);
}

// 测试使用值初始化 deque
TEST_F(DequeTest, ConstructorWithValue)
{
    zstl::deque<int> d(5, 10); // 初始化包含 5 个 10 的 deque
    EXPECT_EQ(d.size(), 5);
    EXPECT_EQ(d.front(), 10);
    EXPECT_EQ(d.back(), 10);
}

// 测试 push_back 和 pop_back
TEST_F(DequeTest, PushBackPopBack)
{
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);

    // 验证 deque 的大小和内容
    EXPECT_EQ(d.size(), 3);
    EXPECT_EQ(d.front(), 1);
    EXPECT_EQ(d.back(), 3);

    // pop_back 后的验证
    d.pop_back();
    EXPECT_EQ(d.size(), 2);
    EXPECT_EQ(d.back(), 2);
}

// 测试 push_front 和 pop_front
TEST_F(DequeTest, PushFrontPopFront)
{
    d.push_front(1);
    d.push_front(2);
    d.push_front(3);

    // 验证 deque 的大小和内容
    EXPECT_EQ(d.size(), 3);
    EXPECT_EQ(d.front(), 3);
    EXPECT_EQ(d.back(), 1);

    // pop_front 后的验证
    d.pop_front();
    EXPECT_EQ(d.size(), 2);
    EXPECT_EQ(d.front(), 2);
}

// 测试下标运算符 []
TEST_F(DequeTest, SubscriptOperator)
{
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);

    EXPECT_EQ(d[0], 1); // 验证第一个元素
    EXPECT_EQ(d[1], 2); // 验证第二个元素
    EXPECT_EQ(d[2], 3); // 验证第三个元素
}

// 测试 insert 操作
TEST_F(DequeTest, Insert)
{
    d.push_back(1);
    d.push_back(3);
    d.push_back(4);

    // 在第二个位置插入 2
    auto iter = d.insert(d.begin() + 1, 2);

    EXPECT_EQ(d.size(), 4);
    EXPECT_EQ(d[0], 1);
    EXPECT_EQ(d[1], 2);
    EXPECT_EQ(d[2], 3);
    EXPECT_EQ(d[3], 4);
}

// 测试 erase 操作
TEST_F(DequeTest, Erase)
{
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);
    d.push_back(4);

    // 删除第二个元素
    auto iter = d.erase(d.begin() + 1);

    EXPECT_EQ(d.size(), 3);
    EXPECT_EQ(d[0], 1);
    EXPECT_EQ(d[1], 3);
    EXPECT_EQ(d[2], 4);
}

// 测试 resize 操作
TEST_F(DequeTest, Resize)
{
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);

    // 扩容到 5 个元素，填充默认值 0
    d.resize(5);
    EXPECT_EQ(d.size(), 5);
    EXPECT_EQ(d[3], 0);
    EXPECT_EQ(d[4], 0);

    // 缩小到 2 个元素
    d.resize(2);
    EXPECT_EQ(d.size(), 2);
    EXPECT_EQ(d[0], 1);
    EXPECT_EQ(d[1], 2);
}

// 测试 clear 操作
TEST_F(DequeTest, Clear)
{
    d.push_back(1);
    d.push_back(2);
    d.push_back(3);

    EXPECT_EQ(d.size(), 3);

    d.clear();
    EXPECT_EQ(d.size(), 0);
    EXPECT_TRUE(d.empty());
}

// 测试交换两个 deque
TEST_F(DequeTest, Swap)
{
    zstl::deque<int> d2;
    d.push_back(1);
    d.push_back(2);
    d2.push_back(3);
    d2.push_back(4);

    d.swap(d2);

    // 验证交换后的内容
    EXPECT_EQ(d.size(), 2);
    EXPECT_EQ(d.front(), 3);
    EXPECT_EQ(d.back(), 4);
    EXPECT_EQ(d2.size(), 2);
    EXPECT_EQ(d2.front(), 1);
    EXPECT_EQ(d2.back(), 2);
}

// 测试大容量的初始化和操作
TEST_F(DequeTest, LargeSize)
{
    zstl::deque<int> d(1000, 42); // 初始化包含 1000 个 42 的 deque
    EXPECT_EQ(d.size(), 1000);
    EXPECT_EQ(d.front(), 42);
    EXPECT_EQ(d.back(), 42);
}
