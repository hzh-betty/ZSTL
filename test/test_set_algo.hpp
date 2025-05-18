#pragma once  
#include <gtest/gtest.h>
#include "../algorithm/set_algo.hpp"
#include "../container/vector.hpp"
#include "../container/list.hpp"
#include "../container/string.hpp"
#include "../iterator/iterator.hpp"
namespace zstl
{
    // 并集测试
    TEST(SetAlgoTest, SetUnion)
    {
        vector<int> a{1, 2, 4, 5};
        vector<int> b{2, 3, 5, 6};
        vector<int> result(8);
        auto it = set_union(a.begin(), a.end(), b.begin(), b.end(), result.begin());
        result.resize(it - result.begin());
        vector<int> expect{1, 2, 3, 4, 5, 6};
        for (int i = 0; i < expect.size(); ++i)
        {
            EXPECT_EQ(*next(result.begin(), i), *next(expect.begin(), i));
        }

        // 自定义比较器
        vector<int> result2(8);
        auto it2 = zstl::set_union(a.begin(), a.end(), b.begin(), b.end(), result2.begin(), std::less<>());
        result2.resize(it2 - result2.begin());
        for (int i = 0; i < expect.size(); ++i)
        {
            EXPECT_EQ(*next(result2.begin(), i), *next(expect.begin(), i));
        }

        // list测试
        list<int> la{1, 2, 4, 5};
        list<int> lb{2, 3, 5, 6};
        list<int> lresult(8);
        auto lit = zstl::set_union(la.begin(), la.end(), lb.begin(), lb.end(), lresult.begin());
        for (auto it = lit; it != lresult.end(); ++it)
        {
            lresult.erase(it);
        }
        list<int> lexpect{1, 2, 3, 4, 5, 6};

        for (int i = 0; i < lexpect.size(); ++i)
        {
            EXPECT_EQ(*next(lresult.begin(), i), *next(lexpect.begin(), i));
        }
    }

    // 交集测试
    TEST(SetAlgoTest, SetIntersection)
    {
        vector<int> a{1, 2, 4, 5};
        vector<int> b{2, 3, 5, 6};
        vector<int> result(4);
        auto it = zstl::set_intersection(a.begin(), a.end(), b.begin(), b.end(), result.begin());
        result.resize(it - result.begin());
        vector<int> expect{2, 5};
        for (int i = 0; i < expect.size(); ++i)
        {
            EXPECT_EQ(*next(result.begin(), i), *next(expect.begin(), i));
        }

        // 自定义比较器
        vector<int> result2(4);
        auto it2 = zstl::set_intersection(a.begin(), a.end(), b.begin(), b.end(), result2.begin(), std::less<>());
        result2.resize(it2 - result2.begin());
        for (int i = 0; i < expect.size(); ++i)
        {
            EXPECT_EQ(*next(result2.begin(), i), *next(expect.begin(), i));
        }

        // list测试
        list<int> la{1, 2, 4, 5};
        list<int> lb{2, 3, 5, 6};
        list<int> lresult(4);
        auto lit = zstl::set_intersection(la.begin(), la.end(), lb.begin(), lb.end(), lresult.begin());
        for (auto it = lit; it != lresult.end(); ++it)
        {
            lresult.erase(it);
        }
        list<int> lexpect{2, 5};

        for (int i = 0; i < lexpect.size(); ++i)
        {
            EXPECT_EQ(*next(lresult.begin(), i), *next(lexpect.begin(), i));
        }
    }

    // 差集测试
    TEST(SetAlgoTest, SetDifference)
    {
        vector<int> a{1, 2, 4, 5};
        vector<int> b{2, 3, 5, 6};
        vector<int> result(4);
        auto it = set_difference(a.begin(), a.end(), b.begin(), b.end(), result.begin());
        result.resize(it - result.begin());
        vector<int> expect{1, 4};
        for (int i = 0; i < expect.size(); ++i)
        {
            EXPECT_EQ(*next(result.begin(), i), *next(expect.begin(), i));
        }
        // 自定义比较器
        vector<int> result2(4);
        auto it2 = zstl::set_difference(a.begin(), a.end(), b.begin(), b.end(), result2.begin(), std::less<>());
        result2.resize(it2 - result2.begin());
        for (int i = 0; i < expect.size(); ++i)
        {
            EXPECT_EQ(*next(result2.begin(), i), *next(expect.begin(), i));
        }

        // list测试
        list<int> la{1, 2, 4, 5};
        list<int> lb{2, 3, 5, 6};
        list<int> lresult(4);
        auto lit = set_difference(la.begin(), la.end(), lb.begin(), lb.end(), lresult.begin());
        for (auto it = lit; it != lresult.end(); ++it)
        {
            lresult.erase(it);
        }
        list<int> lexpect{1, 4};

        for (int i = 0; i < lexpect.size(); ++i)
        {
            EXPECT_EQ(*next(lresult.begin(), i), *next(lexpect.begin(), i));
        }
    }

    // 对称差集测试
    TEST(SetAlgoTest, SetSymmetricDifference)
    {
        vector<int> a{1, 2, 4, 5};
        vector<int> b{2, 3, 5, 6};
        vector<int> result(8);
        auto it = zstl::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(), result.begin());
        result.resize(it - result.begin());
        vector<int> expect{1, 3, 4, 6};
        for (int i = 0; i < expect.size(); ++i)
        {
            EXPECT_EQ(*next(result.begin(), i), *next(expect.begin(), i));
        }

        // 自定义比较器
        vector<int> result2(8);
        auto it2 = zstl::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(), result2.begin(), std::less<>());
        result2.resize(it2 - result2.begin());
        for (int i = 0; i < expect.size(); ++i)
        {
            EXPECT_EQ(*next(result2.begin(), i), *next(expect.begin(), i));
        }

        // list测试
        list<int> la{1, 2, 4, 5};
        list<int> lb{2, 3, 5, 6};
        list<int> lresult(8);
        auto lit = set_symmetric_difference(la.begin(), la.end(), lb.begin(), lb.end(), lresult.begin());
        for (auto it = lit; it != lresult.end(); ++it)
        {
            lresult.erase(it);
        }

        list<int> lexpect{1, 3, 4, 6};

        for (int i = 0; i < lexpect.size(); ++i)
        {
            EXPECT_EQ(*next(lresult.begin(), i), *next(lexpect.begin(), i));
        }
    }

    // 字符串并集测试
    TEST(SetAlgoTest, StringSetUnion)
    {
        string a = "acxz";
        string b = "bcdz";
        string result;
        result.resize(a.size() + b.size());
        auto it = zstl::set_union(a.begin(), a.end(), b.begin(), b.end(), result.begin());
        string expect = "abcdxz";
        EXPECT_STREQ(result.c_str(), expect.c_str());
    }
}
