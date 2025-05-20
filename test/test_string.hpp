#pragma once
#include "../container/string.hpp" // 包含你实现的 string 类所在的头文件
#include <sstream>               // 用于测试流输入输出
#include "gtest/gtest.h"         // 包含 Google Test 的头文件

namespace zstl
{
    // ---------- 构造与赋值 ----------
    TEST(StringTest, DefaultCtor)
    {
        string s;
        EXPECT_EQ(s.size(), 0u);
        EXPECT_TRUE(s.empty());
    }
    TEST(StringTest, CStrCtor)
    {
        string s("hello");
        EXPECT_EQ(s.size(), 5u);
        EXPECT_STREQ(s.c_str(), "hello");
    }
    TEST(StringTest, CopyAndMove)
    {
        string a("abc");
        string b(a); // 拷贝构造
        EXPECT_EQ(b, a);
        string c(::std::move(a)); // 移动构造
        EXPECT_EQ(c, string("abc"));
    }
    TEST(StringTest, CopyAssign)
    {
        string a("x"), b;
        b = a;
        EXPECT_EQ(b, a);
    }
    TEST(StringTest, MoveAssign)
    {
        string a("y"), b;
        b = ::std::move(a);
        EXPECT_EQ(b, string("y"));
    }

    // ---------- 容量与修改 ----------
    TEST(StringTest, ReserveResizeClear)
    {
        string s("abc");
        s.reserve(10);
        EXPECT_GE(s.capacity(), 10u);
        s.resize(5, 'x');
        EXPECT_EQ(s.size(), 5u);
        EXPECT_EQ(string(s.c_str()), "abcxx");
        s.clear();
        EXPECT_TRUE(s.empty());
    }
    TEST(StringTest, PushPopAppend)
    {
        string s;
        s.push_back('A');
        EXPECT_EQ(s.back(), 'A');
        s += 'B';
        EXPECT_EQ(s, string("AB"));
        s.append("CD");
        EXPECT_EQ(s, string("ABCD"));
        s.pop_back();
        EXPECT_EQ(s, string("ABC"));
    }
    TEST(StringTest, InsertErase)
    {
        string s("ace");
        s.insert(1, 'b');
        EXPECT_EQ(s, string("abce"));
        s.insert(3, "d");
        EXPECT_EQ(s, string("abcde").substr(0, 5));
        s.erase(1, 2);
        EXPECT_EQ(s, string("ade"));
    }

    // ---------- 元素访问与比较 ----------
    TEST(StringTest, OpIndexFrontBack)
    {
        string s("xyz");
        EXPECT_EQ(s[0], 'x');
        EXPECT_EQ(s.front(), 'x');
        EXPECT_EQ(s.back(), 'z');
    }
    TEST(StringTest, CompareOperators)
    {
        string a("a"), b("b");
        EXPECT_LT(a, b);
        EXPECT_NE(a, b);
        EXPECT_GE(b, a);
    }

    // ---------- 查找与子串 ----------
    TEST(StringTest, FindCharAndCStr)
    {
        string s("hello");
        EXPECT_EQ(s.find('e'), 1u);
        EXPECT_EQ(s.find("ll"), 2u);
        EXPECT_EQ(s.find('z'), string::npos);
    }
    TEST(StringTest, Substr)
    {
        string s("substring");
        auto sub = s.substr(3, 3);
        EXPECT_EQ(sub, string("str"));
    }

    // ---------- 宽字符测试（wstring） ----------
    TEST(WStringTest, BasicOps)
    {
        wstring w(L"你好");
        EXPECT_EQ(w.size(), 2u);
        w.push_back(L'！');
        EXPECT_EQ(w.back(), L'！');
        EXPECT_EQ(w.substr(0, 2), wstring(L"你好"));
    }

}