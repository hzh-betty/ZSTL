#pragma once
#include <type_traits>
#include "../iterator/iterator.hpp"
namespace zstl
{
    class Construct
    {
    public:
        // 就地构造 -- 即使类型平凡构造，也要在内存上写入默认或零值，必须调用 placement-new；
        // 完美转发构造：在 ptr 指向的内存上调用 T(args...)
        template <typename T, typename... Args>
        static void construct(T *ptr, Args &&...args)
        {
            ::new ((void *)ptr) T(std::forward<Args>(args)...);
        }

        /*显式析构 -- 在大型容器（上百万元素）的析构循环里，这些空函数调用会被展开成循环体的额外指令，
        增加指令缓存压力和函数体大小，平凡析构不关心对象内部状态，
        存储重用本身就结束生命周期，可跳过调用*/

        // 单对象析构：根据类型特征选择是否调用析构函数
        template <typename T>
        static void destroy(T *ptr)
        {
            destroy_impl(ptr, std::is_trivially_destructible<T>{});
        }

        // 范围析构：对[first, last)内的每个元素调用destroy
        template <typename ForwardIter>
        static void destroy_range(ForwardIter first, ForwardIter last)
        {
            using ValueType = typename iterator_traits<ForwardIter>::value_type;
            destroy_range_impl(first, last, std::is_trivially_destructible<ValueType>{});
        }

    private:
        // 平凡析构类型：不执行任何操作
        template <typename T>
        static void destroy_impl(T *, std::true_type)
        {
            // 平凡析构，无需显式调用
        }

        // 非平凡析构类型：显式调用析构函数
        template <typename T>
        static void destroy_impl(T *ptr, std::false_type)
        {
            if (ptr)
            {
                ptr->~T();
            }
        }

        // 范围析构（平凡析构类型）：空实现
        template <typename ForwardIter>
        static void destroy_range_impl(ForwardIter, ForwardIter, std::true_type)
        {
            // 平凡析构类型范围，无需操作
        }

        // 范围析构（非平凡析构类型）：循环调用destroy
        template <typename ForwardIter>
        static void destroy_range_impl(ForwardIter first, ForwardIter last, std::false_type)
        {
            for (; first != last; ++first)
            {
                destroy(&*first);
            }
        }
    };
}
