#pragma once
#include <iostream>
#include<t>
#include <new>
namespace zstl
{
    class Construct
    {
    public:
        // 构造
        template <typename T>
        static void construct(T *ptr)
        {
            ::new ((void *)ptr) T();
        }

        template <typename T1, typename T2>
        static void construct(T1 *ptr, const T2 &value)
        {
            ::new ((void *)ptr) T(value);
        }

        template <typename T1, typename... Args>
        static void construct(T1 *ptr, Args &&...args)
        {
            ::new ((void *)ptr) T(std::forward<Args>(args)...);
        }

        // 析构
        template <class T>
        static void destroy(T *pointer)
        {
            destroy_one(pointer, std::is_trivially_destructible<T>{});
        }

        template <class ForwardIter>
        static void destroy(ForwardIter first, ForwardIter last)
        {
            destroy_cat(first, last, std::is_trivially_destructible<typename iterator_traits<ForwardIter>::value_type>{});
        }

    private:
        template <typename T>
        static void destory_one(T *ptr, std::true_type)
        {
        }

        template <typename T>
        static void destory_one(T *ptr, std::false_type)
        {
            if (ptr != nullptr)
            {
                ptr->~T();
            }
        }

        template <class ForwardIter>
        static void destroy_cat(ForwardIter, ForwardIter, std::true_type) {}

        template <class ForwardIter>
        static void destroy_cat(ForwardIter first, ForwardIter last, std::false_type)
        {
            for (; first != last; ++first)
                destroy(&*first);
        }
    };
};
