#pragma once
#include <gtest/gtest.h>
#include "../functor/functional.hpp"
#include "../container/string.hpp"

namespace zstl
{
    // 普通函数
    int add(int a, int b) { return a + b; }
    int sub(int a, int b) { return a - b; }

    // 函数对象
    struct Multiplier
    {
        int factor;
        explicit Multiplier(int f) : factor(f) {}
        int operator()(int x) const { return x * factor; }
    };

    // 测试类
    struct Foo
    {
        int data = 0;
        int add(int x) { return data + x; }
        [[nodiscard]] int add_const(int x) const { return data + x + 100; }
        static int static_add(int x, int y) { return x + y + 1000; }
    };

    TEST(FunctionTest, FunctionPointer)
    {
        function<int(int, int)> f = add;
        EXPECT_EQ(f(1, 2), 3);

        f = sub;
        EXPECT_EQ(f(5, 3), 2);

        f = nullptr;
        EXPECT_FALSE(f);
    }

    TEST(FunctionTest, Lambda)
    {
        function<int(int, int)> f = [](int a, int b)
        { return a * b; };
        EXPECT_EQ(f(2, 3), 6);
    }

    TEST(FunctionTest, Functor)
    {
        Multiplier m(10);
        function<int(int)> f = m;
        EXPECT_EQ(f(5), 50);

        f = Multiplier(3);
        EXPECT_EQ(f(7), 21);
    }

    TEST(FunctionTest, MemberFunctionPointer_Object)
    {
        Foo foo{42};
        function<int(Foo &, int)> f = &Foo::add;
        EXPECT_EQ(f(foo, 8), 50);
    }

    TEST(FunctionTest, MemberFunctionPointer_Pointer)
    {
        Foo foo{10};
        function<int(Foo *, int)> f = &Foo::add;
        EXPECT_EQ(f(&foo, 5), 15);
    }

    TEST(FunctionTest, MemberFunctionPointer_Const)
    {
        Foo foo{1};
        function<int(const Foo &, int)> f = &Foo::add_const;
        EXPECT_EQ(f(foo, 2), 103);
    }

    TEST(FunctionTest, StaticMemberFunction)
    {
        function<int(int, int)> f = &Foo::static_add;
        EXPECT_EQ(f(1, 2), 1003);
    }

    TEST(BindTest, SimpleBind)
    {
        auto f = bind(add, 1, 2);
        EXPECT_EQ(f(), 3);

        auto f2 = bind(add, zstl::placeholders::_1, 10);
        EXPECT_EQ(f2(5), 15);

        auto f3 = bind(add, 100, zstl::placeholders::_2);
        EXPECT_EQ(f3(1, 2), 2 + 100);
    }

    TEST(BindTest, LambdaBind)
    {
        auto f = bind([](int x, int y)
                      { return x * y; }, zstl::placeholders::_2, zstl::placeholders::_1);
        EXPECT_EQ(f(3, 4), 12);
    }

    TEST(BindTest, FunctorBind)
    {
        Multiplier m(7);
        auto f = bind(m, zstl::placeholders::_1);
        EXPECT_EQ(f(6), 42);
    }

    TEST(BindTest, MemberFunctionBind_Object)
    {
        Foo foo{20};
        auto f = bind(&Foo::add, foo, zstl::placeholders::_1);
        EXPECT_EQ(f(5), 25);
    }

    TEST(BindTest, MemberFunctionBind_Pointer)
    {
        Foo foo{30};
        auto f = bind(&Foo::add, &foo, zstl::placeholders::_1);
        EXPECT_EQ(f(7), 37);
    }

    TEST(BindTest, MemberFunctionBind_PlaceholderObj)
    {
        auto f = bind(&Foo::add, zstl::placeholders::_1, 100);
        Foo foo{50};
        EXPECT_EQ(f(foo), 150);
    }

    TEST(BindTest, NestedBind)
    {
        auto f = bind(add, 1, 2);
        auto g = bind(add, zstl::placeholders::_1, 10);
        auto h = bind(add, f(), g(5));
        EXPECT_EQ(h(), 3 + 15);
    }

    TEST(BindTest, BindWithFunction)
    {
        function<int(int, int)> f = bind(add, zstl::placeholders::_2, zstl::placeholders::_1);
        EXPECT_EQ(f(3, 8), 11);
    }

    TEST(BindTest, BindWithLambdaAndFunction)
    {
        function<int(int, int)> f = bind([](int a, int b)
                                         { return a - b; }, zstl::placeholders::_2, zstl::placeholders::_1);
        EXPECT_EQ(f(5, 2), -3);
    }

    TEST(BindTest, BindWithConstMemberFunction)
    {
        Foo foo{7};
        auto f = bind(&Foo::add_const, foo, zstl::placeholders::_1);
        EXPECT_EQ(f(8), 115);
    }

    TEST(BindTest, BindStaticMemberFunction)
    {
        auto f = bind(&Foo::static_add, zstl::placeholders::_1, zstl::placeholders::_2);
        EXPECT_EQ(f(10, 20), 1030);
    }
}