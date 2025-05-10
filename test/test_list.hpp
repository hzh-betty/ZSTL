#pragma once
#include "gtest/gtest.h"
#include "../container/list.hpp"
namespace zstl
{
    // 测试空链表的初始化，验证 size() 以及 begin() 与 end() 是否正确
    TEST(ListTest, EmptyInitialization)
    {
        list<int> li;
        EXPECT_EQ(li.size(), 0);
        // 当链表为空时，begin() 返回的迭代器应和 end() 一致（指向头节点）
        EXPECT_EQ(li.begin().base_, li.end().base_);
    }

    // 测试 push_back 和 push_front 成员函数
    TEST(ListTest, PushBackAndPushFront)
    {
        list<int> li;
        // 测试 push_back
        li.push_back(1); // 列表： [1]
        EXPECT_EQ(li.size(), 1);
        EXPECT_EQ(li.front(), 1);
        EXPECT_EQ(li.back(), 1);

        // 测试 push_front，在头部插入数据
        li.push_front(2); // 列表： [2, 1]
        EXPECT_EQ(li.size(), 2);
        EXPECT_EQ(li.front(), 2);
        EXPECT_EQ(li.back(), 1);

        // 再次在尾部插入数据
        li.push_back(3); // 列表： [2, 1, 3]
        EXPECT_EQ(li.size(), 3);
        EXPECT_EQ(li.front(), 2);
        EXPECT_EQ(li.back(), 3);
    }

    // 测试正向迭代器遍历（使用 ++ 运算符）
    TEST(ListTest, IteratorForwardTraversal)
    {
        list<int> li;
        li.push_back(1);
        li.push_back(2);
        li.push_back(3); // 列表为 [1, 2, 3]
        auto it = li.begin();
        EXPECT_EQ(*it, 1);
        it++;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
        it++;
        // 当迭代遍历完整个链表后，应与 end() 相等
        EXPECT_EQ(it, li.end());
    }

    // 测试迭代器后向移动（使用 -- 运算符）
    TEST(ListTest, IteratorBackwardTraversal)
    {
        list<int> li;
        li.push_back(10);
        li.push_back(20);
        li.push_back(30); // 列表为 [10, 20, 30]
        auto it = li.end();
        // 后置--操作：指向最后一个有效节点
        it--;
        EXPECT_EQ(*it, 30);
        it--;
        EXPECT_EQ(*it, 20);
        it--;
        EXPECT_EQ(*it, 10);
        // 此时 it 应该与 begin() 相等
        EXPECT_EQ(it, li.begin());
    }

    // 测试 erase 操作（删除中间的节点）
    TEST(ListTest, EraseOperation)
    {
        list<int> li;
        li.push_back(1);
        li.push_back(2);
        li.push_back(3); // 列表为 [1, 2, 3]
        auto it = li.begin();
        it++;         // 指向值 2
        li.erase(it); // 删除值为 2 的节点，列表变为 [1, 3]
        EXPECT_EQ(li.size(), 2);

        auto iter = li.begin();
        EXPECT_EQ(*iter, 1);
        iter++;
        EXPECT_EQ(*iter, 3);
    }

    // 测试 pop_front 和 pop_back 操作
    TEST(ListTest, PopFrontAndPopBack)
    {
        list<int> li;
        li.push_back(1);
        li.push_back(2);
        li.push_back(3); // 列表为 [1, 2, 3]

        li.pop_front(); // 删除头节点：结果为 [2, 3]
        EXPECT_EQ(li.size(), 2);
        EXPECT_EQ(li.front(), 2);

        li.pop_back(); // 删除尾节点：结果为 [2]
        EXPECT_EQ(li.size(), 1);
        EXPECT_EQ(li.back(), 2);
    }

    // 测试 clear 操作，清空整个列表
    TEST(ListTest, ClearOperation)
    {
        list<int> li;
        li.push_back(1);
        li.push_back(2);
        li.clear();
        EXPECT_EQ(li.size(), 0);
        // 列表为空时，begin() 与 end() 应一致
        EXPECT_EQ(li.begin().base_, li.end().base_);
    }

    // 测试复制构造函数（深拷贝）
    TEST(ListTest, CopyConstructor)
    {
        list<int> li;
        li.push_back(10);
        li.push_back(20);

        list<int> li2(li); // 使用复制构造函数创建新链表
        EXPECT_EQ(li2.size(), li.size());

        auto it1 = li.begin();
        auto it2 = li2.begin();
        while (it1 != li.end() && it2 != li2.end())
        {
            EXPECT_EQ(*it1, *it2);
            ++it1;
            ++it2;
        }
    }

    // 测试赋值运算符（拷贝-交换策略）
    TEST(ListTest, AssignmentOperator)
    {
        list<int> li;
        li.push_back(100);
        li.push_back(200);

        list<int> li2;
        li2 = li; // 赋值操作
        EXPECT_EQ(li2.size(), li.size());

        auto it1 = li.begin();
        auto it2 = li2.begin();
        while (it1 != li.end() && it2 != li2.end())
        {
            EXPECT_EQ(*it1, *it2);
            ++it1;
            ++it2;
        }
    }

    // 测试 resize 操作：增大链表（新增节点填充默认值）
    TEST(ListTest, ResizeIncreasing)
    {
        list<int> li;
        li.push_back(1);
        li.push_back(2); // 当前列表为 [1, 2]

        li.resize(5, 7); // 当新大小大于当前节点数时，尾部新增 7；最终列表 [1, 2, 7, 7, 7]
        EXPECT_EQ(li.size(), 5);

        auto it = li.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 7);
        ++it;
        EXPECT_EQ(*it, 7);
        ++it;
        EXPECT_EQ(*it, 7);
    }

    // 测试 resize 操作：减小链表（截断）
    TEST(ListTest, ResizeDecreasing)
    {
        list<int> li;
        li.push_back(1);
        li.push_back(2);
        li.push_back(3); // 当前列表为 [1, 2, 3]

        li.resize(1); // 列表截断为 [1]
        EXPECT_EQ(li.size(), 1);
        EXPECT_EQ(li.front(), 1);
    }

    // 测试 swap 操作：交换两个链表的内容
    TEST(ListTest, SwapOperation)
    {
        list<int> li1;
        li1.push_back(1);
        li1.push_back(2); // li1: [1, 2]

        list<int> li2;
        li2.push_back(3);
        li2.push_back(4); // li2: [3, 4]

        li1.swap(li2);

        EXPECT_EQ(li1.size(), 2);
        EXPECT_EQ(li2.size(), 2);

        auto it1 = li1.begin();
        EXPECT_EQ(*it1, 3);
        ++it1;
        EXPECT_EQ(*it1, 4);

        auto it2 = li2.begin();
        EXPECT_EQ(*it2, 1);
        ++it2;
        EXPECT_EQ(*it2, 2);
    }

    // 测试模板类型为 std::string 时的行为
    TEST(ListTest, StringType)
    {
        list<std::string> ls;
        ls.push_back("hello");
        ls.push_back("world");
        EXPECT_EQ(ls.front(), "hello");
        EXPECT_EQ(ls.back(), "world");
    }

    // 测试移动构造
    TEST(ListTest, MoveConstructor)
    {
        list<int> li1;
        li1.push_back(1);
        li1.push_back(2);
        li1.push_back(3);

        list<int> li2(std::move(li1));
        EXPECT_EQ(li2.size(), 3);
        auto it = li2.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
        EXPECT_EQ(li1.size(), 0);
    }

    // 测试移动赋值
    TEST(ListTest, MoveAssignment)
    {
        list<int> li1;
        li1.push_back(1);
        li1.push_back(2);
        li1.push_back(3);

        list<int> li2;
        li2 = std::move(li1);
        EXPECT_EQ(li2.size(), 3);
        auto it = li2.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
        EXPECT_EQ(li1.size(), 0);
    }

    // 测试 initializer_list 构造
    TEST(ListTest, InitializerListConstructor)
    {
        list<int> li = {1, 2, 3};
        EXPECT_EQ(li.size(), 3);
        auto it = li.begin();
        EXPECT_EQ(*it, 1);
        ++it;
        EXPECT_EQ(*it, 2);
        ++it;
        EXPECT_EQ(*it, 3);
    }

    TEST(ListTest, EmplaceBack)
    {
        list<int> l;
        l.emplace_back(42);
        EXPECT_EQ(l.back(), 42);
        EXPECT_EQ(l.size(), 1);
    }
};
