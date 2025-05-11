#include "gtest/gtest.h"
#include "../allocator/allocator.hpp" // 假设重构后头文件名
#include "../container/vector.hpp"
#include "../container/string.hpp"

namespace zstl
{
    // 测试 fixture: 通用 allocator 测试
    class AllocatorTest : public ::testing::Test
    {
    protected:
        // 可以在此共享初始化数据
    };

    // 测试：分配单个 int 并构造与销毁
    TEST_F(AllocatorTest, AllocateIntAndConstructDestroy)
    {
        // 分配一个 int
        int *p = allocator<int>::allocate();
        ASSERT_NE(p, nullptr) << "allocate() 返回 nullptr";

        // 构造为指定值
        allocator<int>::construct(p, 42);
        EXPECT_EQ(*p, 42) << "construct 后值不正确";

        // 销毁并释放
        allocator<int>::destroy(p);
        allocator<int>::deallocate(p);
    }

    // 测试：分配多个元素
    TEST_F(AllocatorTest, AllocateMultipleElements)
    {
        constexpr size_t N = 5;
        zstl::string *arr = allocator<zstl::string>::allocate(N);
        ASSERT_NE(arr, nullptr) << "allocate(N) 返回 nullptr";

        // 构造多个 zstl::string
        for (size_t i = 0; i < N; ++i)
        {
            allocator<zstl::string>::construct(&arr[i], "test" + (i + 'a'));
            EXPECT_EQ(arr[i], "test" + (i + 'a'));
        }

        // 范围销毁
        allocator<zstl::string>::destroy(arr, arr + N);
        // 释放内存
        allocator<zstl::string>::deallocate(arr, N);
    }

    // 测试：分配 0 个元素，应返回 nullptr
    TEST_F(AllocatorTest, AllocateZero)
    {
        double *p = allocator<double>::allocate(0);
        EXPECT_EQ(p, nullptr) << "allocate(0) 应返回 nullptr";
    }

    // 测试：对齐检查，小于 MAX_BYTES，应走 SecondaryAlloc
    TEST_F(AllocatorTest, SecondaryAllocPath)
    {
        // 分配小于 128 字节的块
        constexpr size_t count = 3;
        struct S
        {
            int a;
            char b;
        };
        S *p = allocator<S>::allocate(count);
        ASSERT_NE(p, nullptr);
        p[0].a = 1;
        p[1].a = 2;
        p[2].a = 3;
        EXPECT_EQ(p[2].a, 3);
        allocator<S>::deallocate(p, count);
    }

    // 测试：大于 MAX_BYTES，应走 PrimaryAlloc
    TEST_F(AllocatorTest, PrimaryAllocPath)
    {
        // 分配大于 128 字节的块
        constexpr size_t count = 100; // 假设 sizeof(double)=8, 100*8=800 >128
        double *p = allocator<double>::allocate(count);
        ASSERT_NE(p, nullptr);
        for (size_t i = 0; i < count; ++i)
            p[i] = i * 1.0;
        EXPECT_DOUBLE_EQ(p[count - 1], (count - 1) * 1.0);
        allocator<double>::deallocate(p, count);
    }

    // 测试：OOM Handler 功能
    void oomHandlerCalled()
    {
        // 测试时可以设置标志，简化这里只打印
        fprintf(stderr, "OOM Handler invoked\n");
    }

    TEST_F(AllocatorTest, OomHandler)
    {
        // 仅测试注册和反注册，不实际触发 OOM
        auto old = PrimaryAlloc<long>::set_oom_handler(&oomHandlerCalled);
        EXPECT_EQ(old, nullptr);
        auto old2 = PrimaryAlloc<long>::set_oom_handler(nullptr);
        EXPECT_EQ(old2, &oomHandlerCalled);
    }

    // 测试：多次 allocate/deallocate 不泄露
    TEST_F(AllocatorTest, RepeatedAllocDealloc)
    {
        for (int i = 0; i < 1000; ++i)
        {
            int *p = allocator<int>::allocate();
            allocator<int>::deallocate(p);
        }

        for (int i = 0; i < 1000; ++i)
        {
            int *p = allocator<int>::allocate(i + 1);
            allocator<int>::deallocate(p, i + 1);
        }
        SUCCEED();
    }

};