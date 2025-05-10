#pragma once
#include "../container/forward_list.hpp"
#include <gtest/gtest.h>
namespace zstl
{
    // 对空列表进行基本属性测试
    TEST(ForwardListTest, EmptyList)
    {
        forward_list<int> lst;
        // 列表应为空
        EXPECT_TRUE(lst.empty());
        // begin 应等于 end
        EXPECT_EQ(lst.begin(), lst.end());
    }

    // 测试 push_front 和 front
    TEST(ForwardListTest, PushFrontAndFront)
    {
        forward_list<int> lst;
        lst.push_front(10);
        // front 应返回第一个元素
        EXPECT_EQ(lst.front(), 10);
        lst.push_front(20);
        EXPECT_EQ(lst.front(), 20);
    }

    // 测试 pop_front
    TEST(ForwardListTest, PopFront)
    {
        forward_list<int> lst;
        lst.push_front(1);
        lst.push_front(2);
        // 删除头部元素后，front 应更新
        lst.pop_front();
        EXPECT_EQ(lst.front(), 1);
        lst.pop_front();
        // 再次删除后应为空
        EXPECT_TRUE(lst.empty());
    }

    // 测试 insert_after 和 erase_after
    TEST(ForwardListTest, InsertEraseAfter)
    {
        forward_list<int> lst;
        lst.push_front(1);            // 列表: 1
        auto it = lst.before_begin(); // 指向 header
        it = lst.insert_after(it, 2); // 列表: 2->1
        EXPECT_EQ(*it, 2);
        // 在 it 后再插入 3，列表: 2->3->1
        auto it2 = lst.insert_after(it, 3);
        EXPECT_EQ(*it2, 3);
        // 删除 3，列表恢复为 2->1
        lst.erase_after(it);
        EXPECT_EQ(*(++forward_list<int>::iterator(lst.before_begin().node_->next_)), 1);
    }

    // 测试 clear
    TEST(ForwardListTest, Clear)
    {
        forward_list<int> lst;
        lst.push_front(1);
        lst.push_front(2);
        lst.clear();
        EXPECT_TRUE(lst.empty());
        EXPECT_EQ(lst.begin(), lst.end());
    }

    // 测试拷贝构造与深拷贝
    TEST(ForwardListTest, CopyConstructor)
    {
        forward_list<int> lst;
        lst.push_front(3);
        lst.push_front(4);
        forward_list<int> copy_lst(lst);
        // 原列表与拷贝列表应相同但不共享内存
        EXPECT_EQ(copy_lst.front(), lst.front());
        lst.pop_front();
        EXPECT_NE(copy_lst.front(), lst.front());
    }

    // 测试赋值运算符和 swap
    TEST(ForwardListTest, AssignmentAndSwap)
    {
        forward_list<int> a;
        forward_list<int> b;
        a.push_front(1);
        b.push_front(2);
        a = b;
        EXPECT_EQ(a.front(), 2);
        // 测试 swap
        a.push_front(3); // a:3->2
        b.push_front(4); // b:4->2
        a.swap(b);
        EXPECT_EQ(a.front(), 4);
        EXPECT_EQ(b.front(), 3);
    }

    // 测试迭代器遍历
    TEST(ForwardListTest, IteratorTraversal)
    {
        forward_list<int> lst;
        // 插入元素 1,2,3
        lst.push_front(1);
        lst.push_front(2);
        lst.push_front(3); // 列表:3->2->1
        int expected[] = {3, 2, 1};
        int idx = 0;
        for (auto it = lst.begin(); it != lst.end(); ++it)
        {
            EXPECT_EQ(*it, expected[idx++]);
        }
        EXPECT_EQ(idx, 3);
    }

    // 测试移动构造
    TEST(ForwardListTest, MoveConstructor)
    {
        forward_list<int> lst1;
        lst1.push_front(1);
        lst1.push_front(2);
        lst1.push_front(3);

        forward_list<int> lst2(std::move(lst1));
        int expected[] = {3, 2, 1};
        int idx = 0;
        for (auto it = lst2.begin(); it != lst2.end(); ++it)
        {
            EXPECT_EQ(*it, expected[idx++]);
        }
        EXPECT_EQ(idx, 3);

    }

    // 测试移动赋值
    TEST(ForwardListTest, MoveAssignment)
    {
        forward_list<int> lst1;
        lst1.push_front(1);
        lst1.push_front(2);
        lst1.push_front(3);

        forward_list<int> lst2;
        lst2 = std::move(lst1);
        int expected[] = {3, 2, 1};
        int idx = 0;
        for (auto it = lst2.begin(); it != lst2.end(); ++it)
        {
            EXPECT_EQ(*it, expected[idx++]);
        }
        EXPECT_EQ(idx, 3);
    }

    // 测试 initializer_list 构造
    TEST(ForwardListTest, InitializerListConstructor)
    {
        forward_list<int> lst = {1, 2, 3};
        int expected[] = {1, 2, 3};
        int idx = 0;
        for (auto it = lst.begin(); it != lst.end(); ++it)
        {
            EXPECT_EQ(*it, expected[idx++]);
        }
        EXPECT_EQ(idx, 3);
    }
    
    // 测试 emplace_after
    TEST(ForwardListTest, EmplaceAfter)
    {
        forward_list<int> fl;
        fl.push_front(1);
        auto it = fl.begin();
        fl.emplace_after(it, 2);
        ++it;
        EXPECT_EQ(*it, 2);
    }
};