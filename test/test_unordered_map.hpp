#pragma once
#include"../include/unordered_map.hpp"
#include <gtest/gtest.h>
namespace zstl
{

    // 测试 Fixture
class UnorderedMapTest : public ::testing::Test {
    protected:
        unordered_map<int, int> intMap;
        unordered_map<string, int> strMap;
    
        void SetUp() override {
            // 初始化操作（每个测试用例前都会执行）
        }
    
        void TearDown() override {
            // 清理操作（每个测试用例后都会执行）
        }
    };
    
    // 测试插入和 size
    TEST_F(UnorderedMapTest, InsertAndSize) {
        EXPECT_TRUE(intMap.empty());  // 初始应为空
        auto result1 = intMap.insert({1, 100});
        EXPECT_TRUE(result1.second);
        EXPECT_EQ(intMap.size(), 1);
        // 再次插入相同键，应该不插入
        auto result2 = intMap.insert({1, 200});
        EXPECT_FALSE(result2.second);
        EXPECT_EQ(intMap.size(), 1);
    }
    
    // 测试 operator[] 和 find
    TEST_F(UnorderedMapTest, BracketOperatorAndFind) {
        intMap[2] = 250;
        // operator[] 应插入键
        EXPECT_EQ(intMap.size(), 1);
        auto it = intMap.find(2);
        ASSERT_NE(it, intMap.end());
        EXPECT_EQ(it->second, 250);
        // 对不存在的键使用 find
        auto it2 = intMap.find(3);
        EXPECT_EQ(it2, intMap.end());
    }
    
    // 测试 erase 和 count
    TEST_F(UnorderedMapTest, EraseAndCount) {
        intMap.insert({5, 500});
        EXPECT_EQ(intMap.count(5), 1);
        size_t erased = intMap.erase(5);
        EXPECT_EQ(erased, 1);
        EXPECT_EQ(intMap.count(5), 0);
        // 再次删除，返回 0
        EXPECT_EQ(intMap.erase(5), 0);
    }
    
    // 测试 clear 和 empty
    TEST_F(UnorderedMapTest, ClearAndEmpty) {
        intMap.insert({10, 1000});
        intMap.insert({20, 2000});
        EXPECT_FALSE(intMap.empty());
        intMap.clear();
        EXPECT_TRUE(intMap.empty());
        EXPECT_EQ(intMap.size(), 0);
    }
    
    // 测试 swap
    TEST_F(UnorderedMapTest, SwapMaps) {
        unordered_map<int, int> other;
        intMap[1] = 10;
        other[2] = 20;
        intMap.swap(other);
        EXPECT_EQ(intMap.count(2), 1);
        EXPECT_EQ(other.count(1), 1);
    }
    
    // 测试字符串键的哈希和操作
    TEST_F(UnorderedMapTest, StringKeyTests) {
        string key1("hello");
        string key2("world");
        strMap[key1] = 1;
        strMap[key2] = 2;
        EXPECT_EQ(strMap.size(), 2);
        EXPECT_EQ(strMap[key1], 1);
        EXPECT_EQ(strMap[key2], 2);
        // 修改已有键的值
        strMap[key1] = 100;
        EXPECT_EQ(strMap[key1], 100);
    }
    
    // 测试迭代器遍历
    TEST_F(UnorderedMapTest, IteratorTraversal) {
        intMap[1] = 10;
        intMap[2] = 20;
        intMap[3] = 30;
        int sum = 0;
        for (auto& kv : intMap) {
            sum += kv.second;
        }
        EXPECT_EQ(sum, 60);
    }
};