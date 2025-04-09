// test_string.cpp
#include "../include/string.hpp" // 包含你实现的 string 类所在的头文件
#include <sstream>               // 用于测试流输入输出
#include "gtest/gtest.h"         // 包含 Google Test 的头文件

// 测试默认构造函数
TEST(StringTest, DefaultConstructor)
{
    zstl::string s;
    EXPECT_TRUE(s.empty());      // 应该为空
    EXPECT_EQ(s.size(), 0u);     // 长度为 0
    EXPECT_STREQ(s.c_str(), ""); // C字符串为空串
}

// 测试通过 C 字符串构造
TEST(StringTest, CStrConstructor)
{
    zstl::string s("hello");
    EXPECT_FALSE(s.empty());
    EXPECT_EQ(s.size(), 5u);
    EXPECT_STREQ(s.c_str(), "hello");
}

// 测试拷贝构造函数
TEST(StringTest, CopyConstructor)
{
    zstl::string s1("world");
    zstl::string s2(s1);
    EXPECT_EQ(s1, s2); // 依赖于重载的 operator== 比较内容是否相同
}

// 测试赋值运算符（使用传值实现复制并交换）
TEST(StringTest, AssignmentOperator)
{
    zstl::string s1("hello");
    zstl::string s2;
    s2 = s1;
    EXPECT_EQ(s1, s2);

    // 修改原串，测试深拷贝是否生效
    s1 += " world";
    EXPECT_NE(s1, s2);
}

// 测试 push_back 和 append 成员函数
TEST(StringTest, PushBackAndAppend)
{
    zstl::string s;
    s.push_back('a');
    EXPECT_EQ(s.size(), 1u);
    EXPECT_STREQ(s.c_str(), "a");

    s.append("bc");
    EXPECT_EQ(s.size(), 3u);
    EXPECT_STREQ(s.c_str(), "abc");
}

// 测试 operator+= 重载支持追加字符和 C 字符串
TEST(StringTest, OperatorPlusEqual)
{
    zstl::string s("go");
    s += "od"; // 追加字符串
    EXPECT_EQ(s.size(), 4u);
    EXPECT_STREQ(s.c_str(), "good");

    s += '!';
    EXPECT_EQ(s.size(), 5u);
    EXPECT_STREQ(s.c_str(), "good!");
}

// 测试 reserve 和 resize 成员函数
TEST(StringTest, ReserveAndResize)
{
    zstl::string s("test");
    size_t oldCap = s.capacity();
    s.reserve(50);
    EXPECT_GE(s.capacity(), 50u);

    // 调整大小，扩充时填充指定字符，比如这里填充 'x'
    s.resize(6, 'x');
    EXPECT_EQ(s.size(), 6u);
    // 检查前四个字符保持不变，后两个填充 'x'
    EXPECT_EQ(s.c_str()[0], 't');
    EXPECT_EQ(s.c_str()[1], 'e');
    EXPECT_EQ(s.c_str()[2], 's');
    EXPECT_EQ(s.c_str()[3], 't');
    EXPECT_EQ(s.c_str()[4], 'x');
    EXPECT_EQ(s.c_str()[5], 'x');
    EXPECT_STREQ(s.c_str(), "testxx");
}

// 测试 find 成员函数寻找单个字符及子串
TEST(StringTest, Find)
{
    zstl::string s("Hello, world!");
    size_t pos = s.find('w');
    EXPECT_NE(pos, (size_t)-1);
    EXPECT_EQ(s[pos], 'w');

    size_t pos2 = s.find("world");
    EXPECT_NE(pos2, (size_t)-1);
}

// 测试 substr 成员函数返回子串
TEST(StringTest, Substr)
{
    zstl::string s("Hello, world!");
    zstl::string sub = s.substr(7, 5); // 应该返回 "world"
    EXPECT_STREQ(sub.c_str(), "world");
}

// 测试 insert 和 erase 成员函数
TEST(StringTest, InsertAndErase)
{
    zstl::string s("Hell world");
    s.insert(4, 'o'); // 在位置4插入 'o'，使其变为 "Hello world"
    EXPECT_STREQ(s.c_str(), "Hello world");

    s.erase(5, 1); // 删除位置5的空格，结果为 "Helloworld"
    EXPECT_STREQ(s.c_str(), "Helloworld");
}

// 测试 front、back 和 pop_back 成员函数
TEST(StringTest, FrontBackPop)
{
    zstl::string s("abc");
    EXPECT_EQ(s.front(), 'a');
    EXPECT_EQ(s.back(), 'c');

    s.pop_back();
    EXPECT_EQ(s.size(), 2u);
    EXPECT_STREQ(s.c_str(), "ab");
}

// 测试流输入输出操作符（<< 与 >>）
TEST(StringTest, StreamOperators)
{
    // 模拟输入：构造一个 istringstream 对象，输入字符串设定为 "HelloWorld"
    std::istringstream inputStream("HelloWorld");
    zstl::string s;
    inputStream >> s;  // 利用重载的 >> 运算符，从流中读取数据到 s
    EXPECT_STREQ(s.c_str(), "HelloWorld") << "输入运算符 >> 读取数据错误";

    // 模拟输出：构造一个 ostringstream 对象，利用重载的 << 运算符写入 s 的内容
    std::ostringstream outputStream;
    outputStream << s;
    EXPECT_STREQ(outputStream.str().c_str(), "HelloWorld") << "输出运算符 << 写入数据错误";
}


// main 函数，运行所有测试
int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
