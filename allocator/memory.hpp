#pragma once
#include <type_traits>
#include <new>
#include <cstddef>


namespace zstl {

//--------------------------------------------------------------------------------
// 辅助类型检测：检测 Alloc 是否定义了特定成员。
//--------------------------------------------------------------------------------

// 检测成员类型
#define ZSTL_HAS_TYPEDEF(Name)                             \
template <typename T, typename = std::void_t<>>            \
struct has_##Name : std::false_type {};                   \
template <typename T>                                     \
struct has_##Name<T, std::void_t<typename T::Name>>       \
    : std::true_type {};

ZSTL_HAS_TYPEDEF(pointer)
ZSTL_HAS_TYPEDEF(const_pointer)
ZSTL_HAS_TYPEDEF(size_type)
ZSTL_HAS_TYPEDEF(difference_type)

#undef ZSTL_HAS_TYPEDEF

template <typename Alloc>
struct allocator_traits {
    using allocator_type    = Alloc;
    using value_type        = typename Alloc::value_type;

    // ----------- 类型别名自动推导 -----------
    // pointer，如无则退化为 value_type*
    using pointer           = std::conditional_t<has_pointer<Alloc>::value,
                                typename Alloc::pointer,
                                value_type*>;
    // const_pointer，如无则退化为 const value_type*
    using const_pointer     = std::conditional_t<has_const_pointer<Alloc>::value,
                                typename Alloc::const_pointer,
                                const value_type*>;
    // void_pointer/const_void_pointer
    using void_pointer      = typename std::pointer_traits<pointer>::template rebind<void>;
    using const_void_pointer= typename std::pointer_traits<pointer>::template rebind<const void>;

    // size & difference，默认退化为 size_t, ptrdiff_t
    using size_type         = std::conditional_t<has_size_type<Alloc>::value,
                                typename Alloc::size_type,
                                std::size_t>;
    using difference_type   = std::conditional_t<has_difference_type<Alloc>::value,
                                typename Alloc::difference_type,
                                std::ptrdiff_t>;

    // rebind_alloc：将 Alloc 重绑定到 U
    template <typename U>
    using rebind_alloc      = typename Alloc::template rebind<U>::other;

    // ----------- 分配与释放 -----------
    static pointer allocate(Alloc& a, size_type n) {
        return a.allocate(n);
    }
    static void deallocate(Alloc& a, pointer p, size_type n) noexcept {
        a.deallocate(p, n);
    }

    // ----------- 构造与析构 -----------
    // construct：优先调用 Alloc::construct，否则使用 placement-new
    template <typename U, typename... Args>
    static void construct(Alloc& a, U* p, Args&&... args) {
        if constexpr (std::is_member_function_pointer_v<decltype(&Alloc::construct)>) {
            a.construct(p, std::forward<Args>(args)...);
        } else {
            ::new (static_cast<void*>(p)) U(std::forward<Args>(args)...);
        }
    }

    // destroy：平凡析构类型无需操作；否则优先调用 Alloc::destroy，或显式析构
    template <typename U>
    static void destroy(Alloc& a, U* p) {
        if constexpr (std::is_trivially_destructible_v<U>) {
            // 平凡析构类型，跳过
        } else if constexpr (std::is_member_function_pointer_v<decltype(&Alloc::destroy)>) {
            a.destroy(p);
        } else {
            p->~U();
        }
    }
};

} // namespace zstl
