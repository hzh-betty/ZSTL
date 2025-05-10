#pragma once
#include "../container/rb_tree.hpp"
namespace zstl
{
    // 测试插入和查找功能
    TEST(RBTreeTest, InsertAndFind)
    {
        RBTree<int, int> tree;
        // 插入一系列键值对
        EXPECT_TRUE(tree.Insert({10, 100}));  // 插入根节点，应返回true
        EXPECT_TRUE(tree.Insert({5, 50}));    // 插入左子节点
        EXPECT_TRUE(tree.Insert({15, 150}));  // 插入右子节点
        EXPECT_FALSE(tree.Insert({10, 100})); // 重复插入，应返回false

        // 查找已插入的节点
        auto node10 = tree.Find(10);
        ASSERT_NE(node10, nullptr);
        EXPECT_EQ(node10->kv_.second, 100);
        auto node5 = tree.Find(5);
        ASSERT_NE(node5, nullptr);
        EXPECT_EQ(node5->kv_.second, 50);
        auto node15 = tree.Find(15);
        ASSERT_NE(node15, nullptr);
        EXPECT_EQ(node15->kv_.second, 150);

        // 查找不存在的节点
        EXPECT_EQ(tree.Find(20), nullptr);
        // 验证红黑树平衡性
        EXPECT_TRUE(tree.IsBalance());
    }

    // 测试删除功能
    TEST(RBTreeTest, Erase)
    {
        RBTree<int, int> tree;
        // 准备数据
        for (int i = 1; i <= 20; ++i)
        {
            tree.Insert({i, i * 10});
        }
        EXPECT_TRUE(tree.IsBalance()); // 删除前应平衡

        // 删除根节点
        EXPECT_TRUE(tree.Erase(1));
        EXPECT_EQ(tree.Find(1), nullptr);
        EXPECT_TRUE(tree.IsBalance()); // 删除后仍应平衡

        // 删除中间节点
        EXPECT_TRUE(tree.Erase(10));
        EXPECT_EQ(tree.Find(10), nullptr);
        EXPECT_TRUE(tree.IsBalance());

        // 删除不存在的节点
        EXPECT_FALSE(tree.Erase(100));
    }

    // 测试拷贝构造和赋值操作
    TEST(RBTreeTest, CopyAndAssignment)
    {
        RBTree<int, int> tree1;
        tree1.Insert({3, 30});
        tree1.Insert({1, 10});
        tree1.Insert({4, 40});

        // 拷贝构造
        RBTree<int, int> tree2(tree1);
        EXPECT_TRUE(tree2.IsBalance());
        EXPECT_NE(tree2.Find(3), nullptr);
        EXPECT_EQ(tree2.Find(3)->kv_.second, 30);

        // 赋值操作
        RBTree<int, int> tree3;
        tree3 = tree1;
        EXPECT_TRUE(tree3.IsBalance());
        EXPECT_NE(tree3.Find(1), nullptr);
        EXPECT_EQ(tree3.Find(1)->kv_.second, 10);
    }

    // 测试clear函数
    TEST(RBTreeTest, Clear)
    {
        RBTree<int, int> tree;
        tree.Insert({2, 20});
        tree.Insert({8, 80});
        EXPECT_NE(tree.Find(2), nullptr);
        tree.clear(); // 清空树
        EXPECT_EQ(tree.Find(2), nullptr);
        EXPECT_TRUE(tree.IsBalance()); // 空树应平衡
    }

    // 测试顺序输出(in-order)
    TEST(RBTreeTest, InOrderTraversal)
    {
        RBTree<int, int> tree;
        std::vector<int> values = {7, 3, 9, 1, 5};
        for (int v : values)
        {
            tree.Insert({v, v});
        }

        // 重定向输出
        std::stringstream ss;
        auto cout_buf = std::cout.rdbuf();
        std::cout.rdbuf(ss.rdbuf());

        tree.InOrder(); // 中序遍历

        // 恢复标准输出
        std::cout.rdbuf(cout_buf);

        // 验证输出顺序
        std::string output = ss.str();
        std::vector<int> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        for (int v : sorted)
        {
            // 每行应包含"v:v"
            EXPECT_NE(output.find(std::to_string(v) + ":" + std::to_string(v)), std::string::npos);
        }
    }
};
