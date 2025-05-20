#pragma once
#include <type_traits>
#include <cstddef>
namespace zstl
{
    /*五种迭代器类型标签*/

    // 输入迭代器
    struct input_iterator_tag
    {
    };

    // 输出迭代器
    struct output_iterator_tag
    {
    };

    // 前向迭代器
    struct forward_iterator_tag : public input_iterator_tag
    {
    };

    // 双向迭代器
    struct bidirectional_iterator_tag : public forward_iterator_tag
    {
    };

    // 随机迭代器
    struct random_access_iterator_tag : public bidirectional_iterator_tag
    {
    };

    // 没有含有五个嵌套类型
    template <typename It, typename = std::void_t<>>
    struct has_iterator_typedefs : public std::false_type
    {
    };

    // 检测是否含有五个嵌套类型
    template <typename It>
    struct has_iterator_typedefs<It, std::void_t<
                                         typename It::iterator_category,
                                         typename It::value_type,
                                         typename It::pointer,
                                         typename It::reference,
                                         typename It::difference_type>> : public std::true_type
    {
    };

    // 主模板：若检测通过，提取嵌套类型；否则无成员
    template <typename It, bool = has_iterator_typedefs<It>::value>
    struct iterator_impl
    {
    };

    template <typename It>
    struct iterator_impl<It, true>
    {
        using iterator_category = typename It::iterator_category;
        using value_type = typename It::value_type;
        using pointer = typename It::pointer;
        using reference = typename It::reference;
        using difference_type = typename It::difference_type;
    };

    // 对外提供迭代器萃取的接口
    template <typename It>
    struct iterator_traits : public iterator_impl<It>
    {
    };

    // 对于原生指针进行偏特化
    template <typename T>
    struct iterator_traits<T *>
    {
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using pointer = T *;
        using reference = T &;
        using difference_type = ptrdiff_t;
    };

    template <typename T>
    struct iterator_traits<const T *>
    {
        using iterator_category = random_access_iterator_tag;
        using value_type = T; // C++ 标准明确规定，iterator_traits<T*>::value_type 固定为 T，无论 T* 还是 const T*
        using pointer = const T *;
        using reference = const T &;
        using difference_type = ptrdiff_t;
    };

    // 判断是否属于某个迭代器
    template <typename It>
    inline constexpr bool is_input_iterator_v =
        std::is_base_of_v<input_iterator_tag, typename iterator_traits<It>::iterator_category>;

    template <typename It>
    inline constexpr bool is_exactly_input_iterator_v =
        std::is_same_v<input_iterator_tag, typename iterator_traits<It>::iterator_category>;

    template <typename It>
    inline constexpr bool is_output_iterator_v =
        std::is_base_of_v<output_iterator_tag, typename iterator_traits<It>::iterator_category>;

    template <typename It>
    inline constexpr bool is_forward_iterator_v =
        std::is_base_of_v<forward_iterator_tag, typename iterator_traits<It>::iterator_category>;

    template <typename It>
    inline constexpr bool is_bidirectional_iterator_v =
        std::is_base_of_v<bidirectional_iterator_tag, typename iterator_traits<It>::iterator_category>;

    template <typename It>
    inline constexpr bool is_random_access_iterator_v =
        std::is_base_of_v<random_access_iterator_tag, typename iterator_traits<It>::iterator_category>;

    template <typename It>
    inline constexpr bool is_iterator_v = is_input_iterator_v<It> || is_output_iterator_v<It>;

    // 通用distance的实现
    template <typename It>
    typename iterator_traits<It>::difference_type distance(It first, It last)
    {
        if constexpr (is_random_access_iterator_v<It>)
        {
            return last - first;
        }
        else
        {
            typename iterator_traits<It>::difference_type n = 0;
            while (first != last)
            {
                ++first;
                ++n;
            }
            return n;
        }
    }

    // 通用advance的使用
    template <typename It, typename Dist>
    void advance(It &it, Dist n)
    {
        if constexpr (is_random_access_iterator_v<It>) // 随机
        {
            it += n;
        }
        else if constexpr (is_bidirectional_iterator_v<It>) // 双向
        {
            if (n >= 0)
            {
                while (n--)
                    ++it;
            }
            else
            {
                while (n++)
                {
                    --it;
                }
            }
        }
        else // 正向
        {
            if (n < 0)
                return;
            while (n--)
                ++it;
        }
    }

    template <typename It>
    It next(It it, typename iterator_traits<It>::difference_type n = 1)
    {
        advance(it, n);
        return it;
    }

    template <typename It>
    It prev(It it, typename iterator_traits<It>::difference_type n = 1)
    {
        advance(it, -n);
        return it;
    }
}