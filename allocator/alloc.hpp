#pragma once
#include "primary_alloc.hpp"
#include "mem_pool_alloc.hpp"
#include "construct.hpp"

namespace zstl
{
    template <typename T>
    class alloc
    {
    public:
        using value_type = T;                   // 分配器管理的对象类型
        using pointer = T *;                    // 指向对象的指针类型
        using const_pointer = const T *;        // 指向常量对象的指针类型
        using reference = T &;                  // 对象的引用类型
        using const_reference = const T &;      // 常量对象的引用类型
        using size_type = std::size_t;          // 大小类型
        using difference_type = std::ptrdiff_t; // 指针差值类型

    public:
        // 可选添加rebind机制
        template <typename U>
        struct rebind
        {
            using other = alloc<U>;
        };

    public:
        // 分配单个对象的内存
        static T *allocate()
        {
            size_type size = sizeof(T);
            pointer ret = nullptr;
            if (size > MAX_BYTES)
            {
                // 大于阈值，用一级分配器
                ret = PrimaryAlloc<T>::allocate(1);
            }
            else
            {
                // 不大于阈值，用二级分配器
                ret = MemoryPool<T>::allocate(1);
            }
            return ret;
        }

        // 分配 n 个对象的连续内存
        static T *allocate(size_type n)
        {
            size_type size = sizeof(T) * n;
            pointer ret = nullptr;
            if (size > MAX_BYTES)
            {
                // 大于阈值，用一级分配器
                ret = PrimaryAlloc<T>::allocate(n);
            }
            else
            {
                // 不大于阈值，用二级分配器
                ret = MemoryPool<T>::allocate(n);
            }
            return ret;
        }

        // 释放单个对象的内存
        static void deallocate(T *ptr)
        {
            size_type size = sizeof(T);
            if (size > MAX_BYTES)
            {
                PrimaryAlloc<T>::deallocate(ptr, 1);
            }
            else
            {
                MemoryPool<T>::deallocate(ptr, 1);
            }
        }

        // 释放 n 个对象的内存
        static void deallocate(T *ptr, size_t n)
        {
            size_type size = sizeof(T) * n;
            if (size > MAX_BYTES)
            {
                PrimaryAlloc<T>::deallocate(ptr, n);
            }
            else
            {
                MemoryPool<T>::deallocate(ptr, n);
            }
        }

        // 原地构造：拷贝构造
        static void construct(T *ptr, const T &value)
        {
            zstl::Construct::construct(ptr, value);
        }

        // 原地构造：移动构造
        static void construct(T *ptr, T &&value)
        {
            zstl::Construct::construct(ptr, std::move(value));
        }

        // 原地构造：完美转发任意参数
        template <class... Args>
        static void construct(T *ptr, Args &&...args)
        {
            zstl::Construct::construct(ptr, std::forward<Args>(args)...);
        }

        // 析构单个对象
        static void destroy(T *ptr)
        {
            zstl::Construct::destroy(ptr);
        }

        // 析构区间内所有对象
        template <typename Iter>
        static void destroy(Iter first, Iter last)
        {
            zstl::Construct::destroy(first, last);
        }
    };
}; // namespace zstl
