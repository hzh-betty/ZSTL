#pragma once
#include <type_traits>
#include <tuple>
#include "../container/string.hpp"
namespace zstl
{
    // 一元谓词取反适配器
    template <typename Predicate>
    class unary_negate
    {
    public:
        explicit unary_negate(const Predicate &pred) : pred_(pred) {}
        template <typename Arg>
        bool operator()(Arg &&arg) const
        {
            return !pred_(std::forward<Arg>(arg));
        }

    private:
        Predicate pred_;
    };

    // not1辅助函数
    template <typename Predicate>
    unary_negate<Predicate> not1(const Predicate &pred)
    {
        return unary_negate<Predicate>(pred);
    }

    // 二元谓词取反适配器
    template <typename Predicate>
    class binary_negate
    {
    public:
        explicit binary_negate(const Predicate &pred) : pred_(pred) {}
        template <typename Arg1, typename Arg2>
        bool operator()(Arg1 &&arg1, Arg2 &&arg2) const
        {
            return !pred_(std::forward<Arg1>(arg1), std::forward<Arg2>(arg2));
        }

    private:
        Predicate pred_;
    };

    // not2辅助函数
    template <typename Predicate>
    binary_negate<Predicate> not2(const Predicate &pred)
    {
        return binary_negate<Predicate>(pred);
    }

    // 算术和逻辑/位运算函数对象
    // 加法运算
    template <typename T>
    struct plus
    {
        constexpr T operator()(const T &lhs, const T &rhs) const { return lhs + rhs; }
    };

    // 减法运算
    template <typename T>
    struct minus
    {
        constexpr T operator()(const T &lhs, const T &rhs) const { return lhs - rhs; }
    };

    // 乘法运算
    template <typename T>
    struct multiplies
    {
        constexpr T operator()(const T &lhs, const T &rhs) const { return lhs * rhs; }
    };

    // 除法运算
    template <typename T>
    struct divides
    {
        constexpr T operator()(const T &lhs, const T &rhs) const { return lhs / rhs; }
    };

    // 取模运算
    template <typename T>
    struct modulus
    {
        constexpr T operator()(const T &lhs, const T &rhs) const { return lhs % rhs; }
    };

    // 取反运算
    template <typename T>
    struct negate
    {
        constexpr T operator()(const T &x) const { return -x; }
    };

    // 逻辑运算
    template <typename T>
    struct logical_and
    {
        constexpr bool operator()(const T &lhs, const T &rhs) const { return lhs && rhs; }
    };

    template <typename T>
    struct logical_or
    {
        constexpr bool operator()(const T &lhs, const T &rhs) const { return lhs || rhs; }
    };

    template <typename T>
    struct logical_not
    {
        constexpr bool operator()(const T &x) const { return !x; }
    };

    // 位运算
    template <typename T>
    struct bit_and
    {
        constexpr T operator()(const T &lhs, const T &rhs) const { return lhs & rhs; }
    };

    template <typename T>
    struct bit_or
    {
        constexpr T operator()(const T &lhs, const T &rhs) const { return lhs | rhs; }
    };

    template <typename T>
    struct bit_xor
    {
        constexpr T operator()(const T &lhs, const T &rhs) const { return lhs ^ rhs; }
    };

    // 比较
    template <typename T>
    struct equal_to
    {
        constexpr bool operator()(const T &lhs, const T &rhs) const { return lhs == rhs; }
    };

    template <typename T>
    struct not_equal_to
    {
        constexpr bool operator()(const T &lhs, const T &rhs) const { return lhs != rhs; }
    };

    template <typename T>
    struct greater
    {
        constexpr bool operator()(const T &lhs, const T &rhs) const { return lhs > rhs; }
    };

    template <typename T>
    struct less
    {
        constexpr bool operator()(const T &lhs, const T &rhs) const { return lhs < rhs; }
    };

    template <typename T>
    struct greater_equal
    {
        constexpr bool operator()(const T &lhs, const T &rhs) const { return lhs >= rhs; }
    };

    template <typename T>
    struct less_equal
    {
        constexpr bool operator()(const T &lhs, const T &rhs) const { return lhs <= rhs; }
    };

    // 通用模板，使用类型不支持时会报错
    template <typename T>
    struct hash
    {
        size_t operator()(const T &key) const
        {
            static_assert(sizeof(T) == 0, "zstl::hash<T> not implemented for this type");
            return key;
        }
    };

    // 指针类型特化
    template <typename T>
    struct hash<T *>
    {
        size_t operator()(T *ptr) const noexcept
        {
            return reinterpret_cast<size_t>(ptr);
        }
    };

    // 辅助函数：对任意字节序列进行逐位哈希（FNV-1a 算法）
    inline size_t bitwise_hash(const unsigned char *first, size_t count)
    {
#if (_MSC_VER && _WIN64) || ((__GNUC__ || __clang__) && __SIZEOF_POINTER__ == 8)
        // 64位平台使用64位FNV参数
        const size_t fnv_offset = 14695981039346656037ull;
        const size_t fnv_prime = 1099511628211ull;
#else
        // 32位平台使用32位FNV参数
        const size_t fnv_offset = 2166136261u;
        const size_t fnv_prime = 16777619u;
#endif
        size_t result = fnv_offset;
        for (size_t i = 0; i < count; ++i)
        {
            result ^= static_cast<size_t>(first[i]);
            result *= fnv_prime;
        }
        return result;
    }

    // float 特化：对 float 类型逐位哈希
    template <>
    struct hash<float>
    {
        size_t operator()(const float &val) const noexcept
        {
            // 对于0，直接返回0，避免+0.0和-0.0哈希不同
            return val == 0.0f ? 0 : bitwise_hash(reinterpret_cast<const unsigned char *>(&val), sizeof(float));
        }
    };

    // double 特化：对 double 类型逐位哈希
    template <>
    struct hash<double>
    {
        size_t operator()(const double &val) const noexcept
        {
            // 对于0，直接返回0，避免+0.0和-0.0哈希不同
            return val == 0.0 ? 0 : bitwise_hash(reinterpret_cast<const unsigned char *>(&val), sizeof(double));
        }
    };

    // long double 特化：对 long double 类型逐位哈希
    template <>
    struct hash<long double>
    {
        size_t operator()(const long double &val) const noexcept
        {
            // 对于0，直接返回0，避免+0.0和-0.0哈希不同
            return val == 0.0L ? 0 : bitwise_hash(reinterpret_cast<const unsigned char *>(&val), sizeof(long double));
        }
    };

    // string 特化
    template <>
    struct hash<zstl::string>
    {
        size_t operator()(const zstl::string &s) const noexcept
        {
            size_t h = 0;
            for (auto c : s)
                h = h * 131 + c;
            return h;
        }
    };

    // 前置声明
    template <typename Fun, typename... Args>
    struct bind_t;

    // 抽象基类，定义统一的可调用接口
    template <typename R, typename... Args>
    class invoker_base
    {
    public:
        // 调用接口，派生类需实现
        virtual R operator()(Args... args) const = 0;
        // 虚析构，保证多态删除安全
        virtual ~invoker_base() {}
        // 克隆自身（用于拷贝 function）
        virtual invoker_base *clone() const = 0;
    };

    // 普通函数指针适配器
    template <typename R, typename... Args>
    class function_ptr_invoker : public invoker_base<R, Args...>
    {
        R (*func_)(Args...); // 保存函数指针

    public:
        function_ptr_invoker(R (*func)(Args...)) : func_(func) {}

        // 调用函数指针
        R operator()(Args... args) const override
        {
            return (func_)(std::forward<Args>(args)...);
        }

        // 克隆自身
        invoker_base<R, Args...> *clone() const override
        {
            return new function_ptr_invoker(func_);
        }
    };

    // 支持成员变量/成员函数指针的调用
    template <typename R, typename MT, typename C, typename T1, typename... Args>
    R invoke_memptr(MT C::*memptr, T1 &&t1, Args &&...args)
    {
        if constexpr (std::is_function_v<MT>)
        {
            // 成员函数指针
            // 这里使用is_base_of_v方便处理多态类型
            if constexpr (std::is_base_of_v<C, std::decay_t<T1>>)
            { // t1为对象
                return (std::forward<T1>(t1).*memptr)(std::forward<Args>(args)...);
            }
            else
            { // t1为指针
                return (std::forward<T1>(t1)->*memptr)(std::forward<Args>(args)...);
            }
        }
        else
        {
            // 成员变量指针
            static_assert(std::is_object_v<MT> && sizeof...(args) == 0);
            if constexpr (std::is_base_of_v<C, std::decay_t<T1>>)
            { // t1为对象
                return std::forward<T1>(t1).*memptr;
            }
            else
            { // t1为指针
                return std::forward<T1>(t1)->*memptr;
            }
        }
    }

    // 成员指针适配器（支持成员变量/成员函数）
    template <typename R, typename MT, typename C, typename... Args>
    class member_ptr_invoker : public invoker_base<R, Args...>
    {
        MT C::*memptr_; // 保存成员指针

    public:
        member_ptr_invoker(MT C::*memptr) : memptr_(memptr) {}

        // 调用成员指针
        R operator()(Args... args) const override
        {
            return invoke_memptr<R>(memptr_, std::forward<Args>(args)...);
        }

        // 克隆自身
        invoker_base<R, Args...> *clone() const override
        {
            return new member_ptr_invoker(memptr_);
        }
    };

    // 仿函数/可调用对象/
    template <typename R, typename F, typename... Args>
    class function_object_invoker : public invoker_base<R, Args...>
    {
        mutable F f_; // mutable 允许const调用非const 函数

    public:
        function_object_invoker(F f) : f_(f) {}

        // 调用可调用对象
        R operator()(Args... args) const override
        {
            return f_(std::forward<Args>(args)...);
        }

        // 克隆自身
        invoker_base<R, Args...> *clone() const override
        {
            return new function_object_invoker(f_);
        }
    };

    // function主模板声明
    template <typename>
    class function;

    // function偏特化，支持任意可调用对象
    template <typename R, typename... Args>
    class function<R(Args...)>
    {
        invoker_base<R, Args...> *invoker_ = nullptr; // 多态指针，保存实际调用器

    public:
        // 默认构造
        function() noexcept {}

        // nullptr构造
        function(nullptr_t fn) noexcept {}

        // 普通函数指针构造
        function(R (*func)(Args...)) : invoker_(new function_ptr_invoker<R, Args...>(func)) {}

        // 成员指针构造
        template <typename MT, typename C>
        function(MT C::*mptr) : invoker_(new member_ptr_invoker<R, MT, C, Args...>(mptr)) {}

        // 可调用对象（lambda/仿函数）构造
        template <typename F>
        function(F f) : invoker_(new function_object_invoker<R, F, Args...>(f)) {}

        // 拷贝构造
        function(const function &x) : invoker_(x.invoker_ ? x.invoker_->clone() : 0) {}

        // 移动构造
        function(function &&x) : invoker_(x.invoker_)
        {
            x.invoker_ = nullptr;
        }

        // nullptr赋值
        function &operator=(nullptr_t fn)
        {
            delete invoker_;
            invoker_ = nullptr;
            return *this;
        }

        // 普通函数指针赋值
        function &operator=(R (*func)(Args...))
        {
            delete invoker_;
            invoker_ = new function_ptr_invoker<R, Args...>(func);
            return *this;
        }

        // 成员指针赋值
        template <typename MT, typename C>
        function &operator=(MT C::*mptr)
        {
            delete invoker_;
            invoker_ = new member_ptr_invoker<R, MT, C, Args...>(mptr);
            return *this;
        }

        // 可调用对象赋值
        template <typename T>
        function &operator=(T t)
        {
            delete invoker_;
            invoker_ = new function_object_invoker<R, T, Args...>(t);
            return *this;
        }

        // 拷贝赋值
        function &operator=(const function &rhs)
        {
            function(rhs).swap(*this);
            return *this;
        }

        // 移动赋值
        function &operator=(function &&rhs) noexcept
        {
            rhs.swap(*this);
            function().swap(rhs);
            return *this;
        }

        // 交换
        function &swap(function &rhs) noexcept
        {
            std::swap(invoker_, rhs.invoker_);
            return *this;
        }

        // 调用接口
        R operator()(Args... args) const
        {
            return (*invoker_)(std::forward<Args>(args)...);
        }

        // 判断是否可调用
        explicit operator bool() const noexcept
        {
            return invoker_;
        }

        // 析构，释放资源
        ~function()
        {
            delete invoker_;
        }
    };
    template <int I>
    struct placeholder_t
    {
    };

    namespace placeholders
    {
        inline constexpr placeholder_t<1> _1{};
        inline constexpr placeholder_t<2> _2{};
        inline constexpr placeholder_t<3> _3{};
        inline constexpr placeholder_t<4> _4{};
        inline constexpr placeholder_t<5> _5{};
        inline constexpr placeholder_t<6> _6{};
        inline constexpr placeholder_t<7> _7{};
        inline constexpr placeholder_t<8> _8{};
        inline constexpr placeholder_t<9> _9{};
        inline constexpr placeholder_t<10> _10{};
    }

    // 结果类型推导：默认模板委托给可调用对象的 operator()
    template <typename Callable>
    struct result_traits : result_traits<decltype(&std::decay_t<Callable>::operator())>
    {
    };

    // 普通函数指针
    template <typename R, typename... Params>
    struct result_traits<R (*)(Params...)>
    {
        using type = R;
    };

    // 成员函数指针
    template <typename R, typename C, typename... Params>
    struct result_traits<R (C::*)(Params...)>
    {
        using type = R;
    };
    template <typename R, typename C, typename... Params>
    struct result_traits<R (C::*)(Params...) const>
    {
        using type = R;
    };

    // 参数选择器：根据类型决定如何获取实际参数
    // 1. 普通值，直接返回
    template <typename T, typename Tuple>
    inline auto select_arg(T &&val, Tuple &)
    {
        return std::forward<T>(val);
    }
    // 2. 占位符，从调用参数元组中取第 I-1 个
    template <int I, typename Tuple>
    inline auto select_arg(placeholder_t<I> &, Tuple &tpl)
    {
        return std::get<I - 1>(tpl);
    }

    // invoke工具：根据可调用对象类型选择正确的调用方式
    template <typename T>
    using is_pointer_noref = std::is_pointer<std::remove_reference_t<T>>;
    template <typename T>
    using is_memfunc_noref = std::is_member_function_pointer<std::remove_reference_t<T>>;

    // 1. 普通函数指针
    template <typename R, typename F, typename... Params>
    inline auto invoke_callable(F &&f, Params &&...args)
        -> std::enable_if_t<is_pointer_noref<F>::value, R>
    {
        return (*std::forward<F>(f))(std::forward<Params>(args)...);
    }
    // 2. 成员函数指针，通过指针对象
    template <typename R, typename F, typename Obj, typename... Params>
    inline auto invoke_callable(F &&f, Obj &&obj, Params &&...args)
        -> std::enable_if_t<is_memfunc_noref<F>::value && is_pointer_noref<Obj>::value, R>
    {
        return (std::forward<Obj>(obj)->*f)(std::forward<Params>(args)...);
    }
    // 3. 成员函数指针，通过对象
    template <typename R, typename F, typename Obj, typename... Params>
    inline auto invoke_callable(F &&f, Obj &&obj, Params &&...args)
        -> std::enable_if_t<is_memfunc_noref<F>::value && !is_pointer_noref<Obj>::value, R>
    {
        return (std::forward<Obj>(obj).*f)(std::forward<Params>(args)...);
    }
    // 4. 通用可调用对象
    template <typename R, typename F, typename... Params>
    inline auto invoke_callable(F &&f, Params &&...args)
        -> std::enable_if_t<!is_pointer_noref<F>::value && !is_memfunc_noref<F>::value, R>
    {
        return std::forward<F>(f)(std::forward<Params>(args)...);
    }

    // bind_t 主体：保存可调用对象和绑定参数，实现参数重排与占位符替换
    template <typename Callable, typename... BoundArgs>
    struct bind_t
    {
        using CallableType = std::decay_t<Callable>;
        using BoundTuple = std::tuple<std::decay_t<BoundArgs>...>;
        using ReturnType = typename result_traits<CallableType>::type;

        CallableType callable_;
        BoundTuple bound_args_;

        // 构造函数：完美转发可调用对象和绑定参数
        template <typename F, typename... Args,
                  std::enable_if_t<!std::is_same_v<std::decay_t<F>, bind_t>, int> = 0>
        explicit bind_t(F &&f, Args &&...args)
            : callable_(std::forward<F>(f)), bound_args_(std::forward<Args>(args)...) {}

        // 拷贝/移动构造与赋值
        bind_t(const bind_t &) = default;
        bind_t(bind_t &&) = default;
        bind_t &operator=(const bind_t &) = default;
        bind_t &operator=(bind_t &&) = default;

        // 调用操作符：将调用参数打包为元组，展开并替换绑定参数
        template <typename... CallArgs>
        ReturnType operator()(CallArgs &&...call_args)
        {
            auto call_tuple = std::forward_as_tuple(std::forward<CallArgs>(call_args)...);
            return call_with_tuple(std::make_index_sequence<sizeof...(BoundArgs)>{}, call_tuple);
        }

    private:
        // 展开绑定参数，递归 select_arg，最终调用可调用对象
        template <std::size_t... I, typename CallTuple>
        ReturnType call_with_tuple(std::index_sequence<I...>, CallTuple &call_tuple)
        {
            return invoke_callable<ReturnType>(
                callable_, select_arg(std::get<I>(bound_args_), call_tuple)...);
        }
    };

    // 辅助函数：简化 bind 的调用
    template <typename F, typename... Args>
    auto bind(F &&f, Args &&...args)
    {
        return bind_t<F, Args...>(std::forward<F>(f), std::forward<Args>(args)...);
    }
} // namespace zstl