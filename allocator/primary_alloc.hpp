#pragma once
#include <new>
#include <cstddef>
#include <cassert>
#include <limits>

namespace zstl
{

    /**
     * @brief 一级空间配置器（Primary Allocator），当申请内存大于系统阈值时，
     *        直接调用 ::operator new / ::operator delete 实现分配与释放，
     *        并依赖全局 std::new_handler 机制处理 OOM。
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

        using new_handler_t = std::new_handler;

    public:
        /**
         * @brief 分配 n 个对象的未构造内存
         * @param n 要分配的对象数量
         * @return pointer 指向分配到的内存的指针，若 n == 0 则返回 nullptr
         * @throws std::bad_alloc 若分配失败且无 new_handler 或 handler 未能释放内存
         */
        static pointer allocate(size_type n)
        {
            if (n == 0)
                return nullptr;

            // 检查是否会溢出
            if (n > max_size())
                throw std::bad_alloc();

            // 直接调用 operator new，会自动触发 std::new_handler
            void *ptr = ::operator new(n * sizeof(value_type));
            return static_cast<pointer>(ptr);
        }

        /**
         * @brief 释放 n 个对象的内存
         * @param ptr 要释放的指针
         * @param n  原先分配的对象数量，可用于调用匹配的 operator delete
         */
        static void deallocate(pointer ptr, size_type n) noexcept
        {
            if (ptr == nullptr)
                return;
            ::operator delete(ptr, n * sizeof(value_type));
        }

        /**
         * @brief 返回该配置器能分配的最大对象数量
         */
        static constexpr size_type max_size() noexcept
        {
            return static_cast<size_type>(-1) / sizeof(value_type);
        }

        /**
         * @brief 安装全局 new_handler，返回之前的 handler
         * @param handler 用户提供的回调函数，若传入 nullptr 则卸载
         */
        static new_handler_t set_new_handler(new_handler_t handler) noexcept
        {
            return std::set_new_handler(handler);
        }
    };

}; // namespace zstl
