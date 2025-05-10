#pragma once
#include "../iterator/stream_iteraotor.hpp"
#include "../include/vector.hpp"
#include <gtest/gtest.h>
#include <sstream>
namespace zstl
{
    // 测试输入迭代器读取整数序列
    TEST(IStreamIteratorTest, ReadInts)
    {
        // 准备字符串流，包含多个整数
        std::istringstream iss("10 20 30 40");
        istream_iterator<int> it(iss);
        istream_iterator<int> end;

        // 验证逐个读取到正确值
        EXPECT_EQ(*it, 10);
        ++it;
        EXPECT_EQ(*it, 20);
        ++it;
        EXPECT_EQ(*it, 30);
        ++it;
        EXPECT_EQ(*it, 40);
        ++it;

        // 到达流末尾，应与默认构造的 end 迭代器相等
        EXPECT_EQ(it, end);
    }

    // 测试输入迭代器在遇到非数字时停止
    TEST(IStreamIteratorTest, StopOnInvalid)
    {
        std::istringstream iss("1 2 three 4");
        istream_iterator<int> it(iss);
        istream_iterator<int> end;

        vector<int> values;
        while (it != end)
        {
            values.push_back(*it);
            ++it;
        }

        // 只应读取到 1 和 2，遇到 "three" 时停止
        ASSERT_EQ(values.size(), 2);
        EXPECT_EQ(values[0], 1);
        EXPECT_EQ(values[1], 2);
    }

    // 测试空流情况下的输入迭代器
    TEST(IStreamIteratorTest, EmptyStream)
    {
        std::istringstream iss("");
        istream_iterator<int> it(iss);
        istream_iterator<int> end;
        EXPECT_EQ(it, end);
    }

    // 测试输出迭代器无分隔符输出
    TEST(OStreamIteratorTest, WriteWithoutDelimiter)
    {
        std::ostringstream oss;
        ostream_iterator<int> out_it(oss);

        // 连续赋值
        *out_it = 5;
        ++out_it;
        *out_it = 15;
        ++out_it;
        *out_it = 25;

        // 验证结果
        EXPECT_EQ(oss.str(), "51525");
    }

    // 测试输出迭代器带分隔符输出
    TEST(OStreamIteratorTest, WriteWithDelimiter)
    {
        std::ostringstream oss;
        ostream_iterator<string> out_it(oss, ","); // 使用逗号作为分隔符

        *out_it = "a";
        ++out_it;
        *out_it = "b";
        ++out_it;
        *out_it = "c";

        // 验证带分隔符的输出形式
        EXPECT_EQ(oss.str(), "a,b,c,");
    }

    // 测试输出迭代器与标准算法结合使用
    TEST(OStreamIteratorTest, UseWithCopyAlgorithm)
    {
        vector<int> data = {1, 2, 3, 4};
       std::ostringstream oss;
        ostream_iterator<int> out_it(oss, "-");

        // 使用 std::copy 将 data 输出到 oss
        std::copy(data.begin(), data.end(), out_it);

        // 验证输出结果
        EXPECT_EQ(oss.str(), "1-2-3-4-");
    }

};