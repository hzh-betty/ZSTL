#pragma once
#include <type_traits>
#include <new>
#include <cstddef>
#include "../iterator/iterator.hpp"

namespace zstl
{

//--------------------------------------------------------------------------------
// 辅助宏：检测 Alloc 类型是否定义了特定的内部 typedef
//--------------------------------------------------------------------------------
#define ZSTL_HAS_TYPEDEF(Name)                                           \
    /*                                                                   \
     * 定义两个模板结构体 has_Name：                           \
     * - 默认情况下继承自 std::false_type                        \
     * - 如果 T::Name 存在，则特化版本继承自 std::true_type \
     */                                                                  \
    template <typename T, typename = std::void_t<>>                      \
    struct has_##Name : std::false_type                                  \
    {                                                                    \
    };                                                                   \
    template <typename T>                                                \
    struct has_##Name<T, std::void_t<typename T::Name>>                  \
        : std::true_type                                                 \
    {                                                                    \
    };

    // 检测常见的 typedef：pointer, const_pointer, size_type, difference_type
    ZSTL_HAS_TYPEDEF(pointer)
    ZSTL_HAS_TYPEDEF(const_pointer)
    ZSTL_HAS_TYPEDEF(size_type)
    ZSTL_HAS_TYPEDEF(difference_type)
#undef ZSTL_HAS_TYPEDEF

    //--------------------------------------------------------------------------------
    // 检测 Alloc 模板中是否存在 rebind<U> 嵌套模板，用于类型重绑定
    //--------------------------------------------------------------------------------

    /**
     * 模板 has_rebind:
     * - 默认 false: Alloc::template rebind<U>::other 不存在
     * - 若存在则特化为 true
     */
    template <typename Alloc, typename U, typename = void>
    struct has_rebind : std::false_type
    {
    };

    template <typename Alloc, typename U>
    struct has_rebind<Alloc, U, std::void_t<typename Alloc::template rebind<U>::other>>
        : std::true_type
    {
    };

    //--------------------------------------------------------------------------------
    // 检测 Alloc::construct 和 Alloc::destroy 成员模板
    //--------------------------------------------------------------------------------

    /**
     * 1. 检测用户分配器是否提供了 construct 成员函数模板:
     *    template<class U, class... Args> void construct(U* p, Args&&... args);
     */
    template <typename Alloc, typename U, typename... Args>
    using detect_construct_t = decltype(std::declval<Alloc &>().construct(
        std::declval<U *>(),    // 指向待构造对象的指针
        std::declval<Args>()... // 构造参数
        ));

    // 默认情况下认为没有提供 construct
    template <typename, typename, typename, typename...>
    struct has_member_construct_impl : std::false_type
    {
    };

    // 如果 detect_construct_t 可行（SFINAE），则特化为 true
    template <typename Alloc, typename U, typename... Args>
    struct has_member_construct_impl<
        std::void_t<detect_construct_t<Alloc, U, Args...>>,
        Alloc, U, Args...> : std::true_type
    {
    };

    // 对外提供布尔常量，有值时为 true
    template <typename Alloc, typename U, typename... Args>
    constexpr bool has_member_construct = has_member_construct_impl<void, Alloc, U, Args...>::value;

    /**
     * 2. 检测用户分配器是否提供了 destroy 成员函数模板:
     *    template<class U> void destroy(U* p);
     */
    template <typename Alloc, typename U>
    using detect_destroy_t = decltype(std::declval<Alloc &>().destroy(
        std::declval<U *>() // 指向待析构对象的指针
        ));

    // 默认情况下认为没有提供 destroy
    template <typename, typename, typename>
    struct has_member_destroy_impl : std::false_type
    {
    };

    // 若 detect_destroy_t 可行，则特化为 true
    template <typename Alloc, typename U>
    struct has_member_destroy_impl<
        std::void_t<detect_destroy_t<Alloc, U>>,
        Alloc, U> : std::true_type
    {
    };

    // 对外提供布尔常量
    template <typename Alloc, typename U>
    constexpr bool has_member_destroy = has_member_destroy_impl<void, Alloc, U>::value;

    //--------------------------------------------------------------------------------
    // allocator_traits 实现
    //   提供统一的接口，屏蔽 Alloc 细节，兼容无状态或有状态分配器
    //--------------------------------------------------------------------------------

    template <typename Alloc>
    struct allocator_traits
    {
        // 分配器类型
        using allocator_type = Alloc;
        // 分配器管理的值类型
        using value_type = typename Alloc::value_type;

        // ----------- 类型别名自动推导 -----------
        /**
         * pointer: 如果 Alloc 定义了 pointer，则使用 Alloc::pointer，
         * 否则退化为 value_type*
         */
        using pointer = std::conditional_t<
            has_pointer<Alloc>::value,
            typename Alloc::pointer,
            value_type *>;

        /**
         * const_pointer: 如果 Alloc 定义了 const_pointer，则使用，
         * 否则退化为 const value_type*
         */
        using const_pointer = std::conditional_t<
            has_const_pointer<Alloc>::value,
            typename Alloc::const_pointer,
            const value_type *>;

        // void_pointer / const_void_pointer: 重绑定成 (const_)void*
        using void_pointer = typename std::pointer_traits<pointer>::template rebind<void>;
        using const_void_pointer = typename std::pointer_traits<pointer>::template rebind<const void>;

        /**
         * size_type / difference_type: 如果 Alloc 定义了对应类型，则使用，
         * 否则分别退化为 std::size_t / std::ptrdiff_t
         */
        using size_type = std::conditional_t<
            has_size_type<Alloc>::value,
            typename Alloc::size_type,
            std::size_t>;
        using difference_type = std::conditional_t<
            has_difference_type<Alloc>::value,
            typename Alloc::difference_type,
            std::ptrdiff_t>;

        /**
         * rebind_alloc<U>: 将当前分配器重绑定到类型 U 的分配器
         * 若 Alloc 提供 rebind<U>::other，则使用，否则退化到自定义 alloc<U>
         */
        template <typename U>
        using rebind_alloc = std::conditional_t<
            has_rebind<Alloc, U>::value,
            typename Alloc::template rebind<U>::other,
            alloc<U>>;

        // ----------- 分配与释放 -----------
        /**
         * allocate: 委托给 Alloc::allocate(n)
         */
        static pointer allocate(Alloc &a, size_type n)
        {
            return a.allocate(n);
        }

        /**
         * deallocate: 委托给 Alloc::deallocate(p, n)，并保证 noexcept
         */
        static void deallocate(Alloc &a, pointer p, size_type n) noexcept
        {
            a.deallocate(p, n);
        }

        // ----------- 构造 -----------
        /**
         * construct: 如果用户分配器提供了 construct，则调用；
         * 否则使用 placement-new 完成对象构造
         */
        template <typename U, typename... Args>
        static void construct(Alloc &a, U *p, Args &&...args)
        {
            if constexpr (has_member_construct<Alloc, U, Args...>)
            {
                // 调用用户自定义构造
                a.construct(p, std::forward<Args>(args)...);
            }
            else
            {
                // fallback to placement-new
                ::new (static_cast<void *>(p)) U(std::forward<Args>(args)...);
            }
        }

        // ----------- 析构 -----------
        /**
         * destroy: 对平凡类型不做操作；
         * 如果用户分配器提供了 destroy，则调用；
         * 否则显式调用析构函数
         */
        template <typename U>
        static void destroy(Alloc &a, U *p) noexcept
        {
            if constexpr (std::is_trivially_destructible_v<U>)
            {
                // 平凡类型，无需处理
            }
            else if constexpr (has_member_destroy<Alloc, U>)
            {
                // 调用用户自定义析构
                a.destroy(p);
            }
            else
            {
                // fallback to explicit destructor call
                p->~U();
            }
        }

        /**
         * destroy_range: 对[first, last)范围内的每个元素依次调用 destroy
         * 平凡类型为空实现，非平凡类型循环调用
         */
        template <typename ForwardIter>
        static void destroy_range(Alloc &a, ForwardIter first, ForwardIter last) noexcept
        {
            using U = typename iterator_traits<ForwardIter>::value_type;
            if constexpr (std::is_trivially_destructible_v<U>)
            {
                // 平凡类型，跳过
            }
            else
            {
                for (; first != last; ++first)
                {
                    // 取地址并调用 destroy
                    destroy(a, std::addressof(*first));
                }
            }
        }
    };

} // namespace zstl
