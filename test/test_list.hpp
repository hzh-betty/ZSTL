#include "gtest/gtest.h"
#include "../include/list.hpp"  
#include <vector>

// 为了测试，也可以写一个简单的辅助函数获取容器中所有元素（用于比较顺序）
template <typename T>
std::vector<T> ToVector(zstl::list<T>& lst) {
    std::vector<T> vec;
    for (auto it = lst.begin(); it != lst.end(); ++it) {
        vec.push_back(*it);
    }
    return vec;
}


// 测试用例一：空链表的基本状态
TEST(ListTest, EmptyList) {
    zstl::list<int> lst;
    EXPECT_EQ(lst.size(), 0);
    // 空链表的 begin() == end()
    EXPECT_EQ(lst.begin(), lst.end());
}


// 测试用例二：push_back 操作
TEST(ListTest, PushBack) {
    zstl::list<int> lst;
    lst.push_back(10);
    lst.push_back(20);
    lst.push_back(30);
    EXPECT_EQ(lst.size(), 3);

    // 检查遍历顺序是否正确：10, 20, 30
    auto it = lst.begin();
    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
    ++it;
    EXPECT_EQ(*it, 30);
}

// 测试用例三：push_front 操作
TEST(ListTest, PushFront) {
    zstl::list<int> lst;
    lst.push_front(10);
    lst.push_front(20);
    lst.push_front(30);
    EXPECT_EQ(lst.size(), 3);

    // push_front 后的顺序为：30, 20, 10
    auto it = lst.begin();
    EXPECT_EQ(*it, 30);
    ++it;
    EXPECT_EQ(*it, 20);
    ++it;
    EXPECT_EQ(*it, 10);
}

// 测试用例四：pop_back 操作
TEST(ListTest, PopBack) {
    zstl::list<int> lst;
    lst.push_back(10);
    lst.push_back(20);
    lst.push_back(30);
    lst.pop_back();  // 删除尾部节点 (30)
    EXPECT_EQ(lst.size(), 2);

    auto it = lst.begin();
    EXPECT_EQ(*it, 10);
    ++it;
    EXPECT_EQ(*it, 20);
}

// 测试用例五：pop_front 操作
TEST(ListTest, PopFront) {
    zstl::list<int> lst;
    lst.push_back(10);
    lst.push_back(20);
    lst.push_back(30);
    lst.pop_front(); // 删除首个有效节点 (10)
    EXPECT_EQ(lst.size(), 2);

    auto it = lst.begin();
    EXPECT_EQ(*it, 20);
    ++it;
    EXPECT_EQ(*it, 30);
}


// 测试用例六：insert 操作
TEST(ListTest, InsertTest) {
    zstl::list<int> lst;
    lst.push_back(10);
    lst.push_back(30);
    // 找到迭代器指向值为30的节点，然后在其前面插入20
    auto it = lst.begin();
    ++it; // it 指向 30
    lst.insert(it, 20);
    EXPECT_EQ(lst.size(), 3);

    // 遍历顺序应为：10, 20, 30
    auto iter = lst.begin();
    EXPECT_EQ(*iter, 10);
    ++iter;
    EXPECT_EQ(*iter, 20);
    ++iter;
    EXPECT_EQ(*iter, 30);
}


// 测试用例七：erase 操作（删除中间和头/尾节点）
TEST(ListTest, EraseTest) {
    zstl::list<int> lst;
    lst.push_back(10);
    lst.push_back(20);
    lst.push_back(30);
    lst.push_back(40);

    // 删除中间的某个元素，比如删除20
    auto it = lst.begin();
    ++it; // 指向20
    lst.erase(it);
    EXPECT_EQ(lst.size(), 3);
    std::vector<int> expected1 = {10, 30, 40};
    EXPECT_EQ(ToVector(lst), expected1);

    // 删除头部元素
    lst.erase(lst.begin());
    EXPECT_EQ(lst.size(), 2);
    std::vector<int> expected2 = {30, 40};
    EXPECT_EQ(ToVector(lst), expected2);

    // 删除尾部元素
    lst.pop_back();
    EXPECT_EQ(lst.size(), 1);
    std::vector<int> expected3 = {30};
    EXPECT_EQ(ToVector(lst), expected3);
}

// 测试用例八：拷贝构造函数
TEST(ListTest, CopyConstructorTest) {
    zstl::list<int> lst;
    lst.push_back(10);
    lst.push_back(20);
    lst.push_back(30);
    // 使用拷贝构造函数生成新链表
    zstl::list<int> lst_copy(lst);
    EXPECT_EQ(lst.size(), lst_copy.size());

    // 遍历比较两个链表中对应数据是否相同
    auto it1 = lst.begin();
    auto it2 = lst_copy.begin();
    while (it1 != lst.end() && it2 != lst_copy.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}


// 测试用例九：赋值运算符（拷贝-交换策略）
TEST(ListTest, AssignmentOperatorTest) {
    zstl::list<int> lst1;
    lst1.push_back(10);
    lst1.push_back(20);

    zstl::list<int> lst2;
    lst2.push_back(100);
    lst2.push_back(200);
    lst2.push_back(300);

    // 使用赋值运算符
    lst2 = lst1;
    EXPECT_EQ(lst1.size(), lst2.size());

    auto it1 = lst1.begin();
    auto it2 = lst2.begin();
    while (it1 != lst1.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}


// 测试用例十：clear 操作
TEST(ListTest, ClearTest) {
    zstl::list<int> lst;
    lst.push_back(10);
    lst.push_back(20);
    lst.push_back(30);
    lst.clear();
    EXPECT_EQ(lst.size(), 0);
    EXPECT_EQ(lst.begin(), lst.end());
}


// 测试用例十一：swap 操作
TEST(ListTest, SwapTest) {
    zstl::list<int> lst1;
    lst1.push_back(1);
    lst1.push_back(2);

    zstl::list<int> lst2;
    lst2.push_back(10);
    lst2.push_back(20);
    lst2.push_back(30);

    // 交换两个链表
    lst1.swap(lst2);

    // 交换后 lst1 应该拥有原来 lst2 的内容
    EXPECT_EQ(lst1.size(), 3);
    std::vector<int> expected1 = {10, 20, 30};
    EXPECT_EQ(ToVector(lst1), expected1);

    // lst2 应该拥有原来 lst1 的内容
    EXPECT_EQ(lst2.size(), 2);
    std::vector<int> expected2 = {1, 2};
    EXPECT_EQ(ToVector(lst2), expected2);
}

