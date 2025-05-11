#pragma once
#include <new>
#include <cstddef>
#include <cassert>

namespace zstl
{

    /**
     * @brief 一级空间配置器（Primary Allocator），当申请内存大于系统阈值时，
     *        直接调用 ::operator new / ::operator delete 实现分配与释放。
     *
     * @tparam T 要分配的对象类型
     */
    template <typename T>
    class PrimaryAlloc
    {
    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

        using oom_handler_t = void (*)();

    public:

        /**
         * @brief 分配 n 个对象的未构造内存
         * @param n 要分配的对象数量
         * @return pointer 指向分配到的内存的指针，若 n == 0 则返回 nullptr
         */
        static pointer allocate(size_type n)
        {
            if (n == 0)
            {
                return nullptr;
            }

            return static_cast<pointer>(oom_allocate(n * sizeof(value_type)));
        }

        /**
         * @brief 释放 n 个对象的内存
         * @param ptr 要释放的指针
         * @param n  原先分配的对象数量，可用于调用匹配的 operator delete
         */
        static void deallocate(pointer ptr, size_type n) noexcept
        {
            if (ptr == nullptr)
            {
                return;
            }
            // 调用带尺寸参数的 operator delete（C++14 起支持）
            ::operator delete(ptr, n * sizeof(value_type));
        }

        /**
         * @brief 返回该配置器能分配的最大对象数量
         * @return constexpr size_type 最大对象数
         */
        static constexpr size_type max_size() noexcept
        {
            return static_cast<size_type>(-1) / sizeof(value_type);
        }

        /**
         * @brief 安装 OOM 回调
         * @param handler 用户提供的回调函数，若传入 nullptr 则卸载
         */
        static oom_handler_t set_oom_handler(oom_handler_t handler) noexcept
        {
            oom_handler_t old = oom_handler_;
            oom_handler_ = handler;
            return old;
        }

    private:
        /**
         * @brief 类似 SGI STL 的 oom_malloc，不断尝试分配
         * @param bytes 要分配的字节数
         * @return void* 分配成功的内存地址
         * @throws std::bad_alloc 若分配失败且无回调或回调未能释放内存
         */
        static void *oom_allocate(size_type bytes)
        {
            void *result = nullptr;
            while (true)
            {
                try
                {
                    result = ::operator new(bytes);
                    return result; // 成功则返回
                }
                catch (const std::bad_alloc &)
                {
                    if (oom_handler_)
                    {
                        // 调用用户注册的 OOM 回调，之后重试
                        oom_handler_();
                    }
                    else
                    {
                        // 未注册回调，抛出异常
                        throw;
                    }
                }
            }
        }
        inline static oom_handler_t oom_handler_ = nullptr;
    };

}; // namespace zstl
