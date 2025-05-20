#pragma once
#include "iterator.hpp"
#include "../container/string.hpp"
namespace zstl
{
    // 输入迭代器
    template <typename T, typename CharT = char,
              typename Traits = std::char_traits<CharT>, typename Dist = ptrdiff_t>
    class istream_iterator
    {
        using char_type = CharT;
        using traits_type = Traits;
        using istream_type = std::basic_istream<char_type, Traits>;
        using self = istream_iterator<T, char_type, Traits, Dist>;

    public:
        // 必需的五个类型
        using iterator_category = input_iterator_tag;
        using value_type = T;
        using pointer = const T *;
        using reference = const T &;
        using difference_type = Dist;

    public:
        // 构造：默认构造为 end-of-stream 状态，可以作为结束状态
        istream_iterator()
            : stream_(nullptr), at_end_(true)
        {
        }

        // 构造：关联到某个流，并立即读取第一个值
        explicit istream_iterator(istream_type &is)
            : stream_(&is), at_end_(false)
        {
            ++(*this);
        }

        // 解引用：返回最近读取的值
        reference operator*() const { return value_; }

        // 成员访问
        pointer operator->() const { return &value_; }

        // 前置 ++ ：尝试从流中读取，下次解引用可得新值
        self &operator++()
        {
            if (!(*stream_ >> value_))
            {
                at_end_ = true;
                stream_ = nullptr; // 变为 end-of-stream
            }
            return *this;
        }

        // 后置 ++ ：返回旧值的副本
        self operator++(int)
        {
            self tmp = *this;
            ++(*this);
            return tmp;
        }

        // 比较：仅当都为 end-of-stream 时相等
        bool operator==(const self &other) const
        {
            return at_end_ == other.at_end_;
        }
        bool operator!=(const self &other) const
        {
            return at_end_ != other.at_end_;
        }

    private:
        istream_type *stream_;
        value_type value_; // 值类型
        bool at_end_;
    };

    // 输出迭代器
    template <typename T, typename CharT = char,
              typename Traits = std::char_traits<CharT>>
    class ostream_iterator
    {
        using char_type = CharT;
        using traits_type = Traits;
        using ostream_type = std::basic_ostream<char_type, Traits>;
        using self = ostream_iterator<T, char_type, Traits>;

    public:
        // 必需的五个类型
        using iterator_category = output_iterator_tag;
        using value_type = void; // 输出迭代器不提供可读值
        using pointer = void;
        using reference = void;
        using difference_type = void;

    public:
        // 构造：关联到某个流，可选分隔符
        explicit ostream_iterator(ostream_type &os, const char *delim = nullptr)
            : out_(&os), delim_(delim) {}

        // 赋值运算符：输出值并输出分隔符（若提供）
        self &operator=(const T &value)
        {
            *out_ << value;
            if (delim_)
            {
                *out_ << delim_;
            }
            return *this;
        }

        // 解引用/自增都返回自身
        self &operator*() { return *this; }
        self &operator++() { return *this; }
        self operator++(int) { return *this; }

    private:
        ostream_type *out_;
        const char *delim_;
    };
}
