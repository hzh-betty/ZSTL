#pragma once
#include <iostream>
#include <cassert>
#include <cstring>
#include "../iterator/reverse_iterator.hpp"
#include "../allocator/alloc.hpp"
#include "../allocator/memory.hpp"

namespace zstl
{
    // 通用字符特性模板，提供字符类型的基本操作
    template <class CharType>
    struct char_traits
    {
        using char_type = CharType; // 字符类型别名

        // 计算以终止符结尾的字符串长度
        static size_t length(const char_type *str) noexcept
        {
            size_t len = 0;
            for (; *str != char_type(0); ++str)
                ++len;
            return len;
        }

        // 按字典序比较两个字符串的前n个字符
        static int compare(const char_type *s1, const char_type *s2, size_t n) noexcept
        {
            for (; n; --n, ++s1, ++s2)
            {
                if (*s1 < *s2)
                    return -1;
                if (*s2 < *s1)
                    return 1;
            }
            return 0;
        }

        // 安全拷贝n个字符（要求源和目标内存不重叠）
        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            assert(src + n <= dst || dst + n <= src);
            char_type *r = dst;
            for (; n; --n, ++dst, ++src)
                *dst = *src;
            return r;
        }

        // 安全移动n个字符（支持内存重叠情况）
        static char_type *move(char_type *dst, const char_type *src, size_t n) noexcept
        {
            char_type *r = dst;
            if (dst < src)
            {
                for (; n; --n, ++dst, ++src)
                    *dst = *src;
            }
            else if (src < dst)
            {
                dst += n;
                src += n;
                for (; n; --n)
                    *--dst = *--src;
            }
            return r;
        }

        // 用指定字符填充内存区域
        static char_type *fill(char_type *dst, char_type ch, size_t count) noexcept
        {
            char_type *r = dst;
            for (; count; --count, ++dst)
                *dst = ch;
            return r;
        }
    };

    // char类型特化版本，使用标准库函数优化性能
    template <>
    struct char_traits<char>
    {
        using char_type = char;
        static size_t length(const char_type *str) noexcept { return strlen(str); }
        static int compare(const char_type *s1, const char_type *s2, size_t n) noexcept { return memcmp(s1, s2, n); }
        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            assert(src + n <= dst || dst + n <= src);
            return reinterpret_cast<char_type *>(memcpy(dst, src, n));
        }
        static char_type *move(char_type *dst, const char_type *src, size_t n) noexcept { return reinterpret_cast<char_type *>(memmove(dst, src, n)); }
        static char_type *fill(char_type *dst, char_type ch, size_t count) noexcept { return reinterpret_cast<char_type *>(memset(dst, ch, count)); }
    };

    // 动态字符串模板类，使用 zstl::allocator 和 allocator_traits 管理内存
    template <typename CharT, typename Traits = char_traits<CharT>, typename Alloc = alloc<CharT>>
    class basic_string
    {
    public:
        // 类型定义
        using traits_type = Traits;
        using value_type = typename Traits::char_type;
        using allocator_type = Alloc;
        using traits_allocator = allocator_traits<allocator_type>;
        using pointer = typename traits_allocator::pointer;
        using const_pointer = typename traits_allocator::const_pointer;
        using reference = value_type &;
        using const_reference = const value_type &;
        using size_type = typename traits_allocator::size_type;
        using difference_type = typename traits_allocator::difference_type;

        inline static const size_type npos = static_cast<size_type>(-1);

    public:
        // 迭代器相关
        using iterator = value_type *;
        using const_iterator = const value_type *;
        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<const_iterator>;

        iterator begin() noexcept { return str_; }
        const_iterator begin() const noexcept { return str_; }
        iterator end() noexcept { return str_ + size_; }
        const_iterator end() const noexcept { return str_ + size_; }
        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    public:
        // 构造、析构与赋值
        basic_string()
            : str_(traits_allocator::allocate(alloc_, 1)), size_(0), capacity_(0)
        {
            Traits::fill(str_, value_type(0), 1);
        }

        basic_string(const value_type *s)
        {
            size_ = Traits::length(s);
            capacity_ = size_;
            str_ = traits_allocator::allocate(alloc_, capacity_ + 1);
            Traits::copy(str_, s, size_ + 1);
        }

        basic_string(const basic_string &o)
            : size_(o.size_), capacity_(o.capacity_)
        {
            str_ = traits_allocator::allocate(alloc_, capacity_ + 1);
            Traits::copy(str_, o.str_, size_ + 1);
        }

        basic_string(basic_string &&o) noexcept
            : str_(o.str_), size_(o.size_), capacity_(o.capacity_)
        {
            o.str_ = nullptr;
            o.size_ = o.capacity_ = 0;
        }

        ~basic_string()
        {
            if (str_)
            {
                traits_allocator::deallocate(alloc_, str_, capacity_ + 1);
                str_ = nullptr;
                size_ = capacity_ = 0;
            }
        }

        basic_string &operator=(const basic_string &o)
        {
            if (this != &o)
            {
                basic_string tmp(o);
                swap(tmp);
            }
            return *this;
        }

        basic_string &operator=(basic_string &&o) noexcept
        {
            if (this != &o)
            {
                traits_allocator::deallocate(alloc_, str_, capacity_ + 1);
                str_ = o.str_;
                size_ = o.size_;
                capacity_ = o.capacity_;
                o.str_ = nullptr;
                o.size_ = o.capacity_ = 0;
            }
            return *this;
        }

        // 大小与容量
        size_type size() const noexcept { return size_; }
        size_type capacity() const noexcept { return capacity_; }
        bool empty() const noexcept { return size_ == 0; }

        // 预分配内存
        void reserve(size_type n)
        {
            if (n > capacity_)
            {
                value_type *tmp = traits_allocator::allocate(alloc_, n + 1);
                Traits::copy(tmp, str_, size_ + 1);
                traits_allocator::deallocate(alloc_, str_, capacity_ + 1);
                str_ = tmp;
                capacity_ = n;
            }
        }

        // 调整长度
        void resize(size_type n, value_type c = value_type())
        {
            if (n > size_)
            {
                reserve(n);
                Traits::fill(str_ + size_, c, n - size_);
            }
            size_ = n;
            str_[size_] = value_type(0);
        }

        // 清空内容
        void clear() noexcept
        {
            size_ = 0;
            if (str_)
                Traits::fill(str_, value_type(0), 1);
        }

        // 下标访问
        reference operator[](size_type p)
        {
            assert(p < size_);
            return str_[p];
        }
        const_reference operator[](size_type p) const
        {
            assert(p < size_);
            return str_[p];
        }

        reference &front() { return operator[](0); }
        const_reference front() const { return operator[](0); }
        reference &back() { return operator[](size_ - 1); }
        const_reference back() const { return operator[](size_ - 1); }
        const_pointer c_str() const noexcept { return str_; }

        // 追加操作
        void push_back(value_type ch)
        {
            if (size_ + 1 > capacity_)
                reserve(capacity_ ? capacity_ * 2 : 4);
            str_[size_++] = ch;
            str_[size_] = value_type(0);
        }

        basic_string &operator+=(value_type ch)
        {
            push_back(ch);
            return *this;
        }
        basic_string &operator+=(const value_type *s)
        {
            append(s);
            return *this;
        }

        void append(const value_type *s)
        {
            size_type l = Traits::length(s);
            if (size_ + l > capacity_)
                reserve(size_ + l);
            Traits::copy(str_ + size_, s, l + 1);
            size_ += l;
        }

        void insert(size_type pos, value_type ch)
        {
            assert(pos <= size_);
            if (size_ + 1 > capacity_)
                reserve(capacity_ ? capacity_ * 2 : 4);
            Traits::move(str_ + pos + 1, str_ + pos, size_ - pos + 1);
            str_[pos] = ch;
            ++size_;
        }

        void insert(size_type pos, const_pointer s)
        {
            assert(pos <= size_);
            size_type l = Traits::length(s);
            if (size_ + l > capacity_)
                reserve(size_ + l);
            Traits::move(str_ + pos + l, str_ + pos, size_ - pos + 1);
            Traits::copy(str_ + pos, s, l);
            size_ += l;
        }

        void erase(size_type pos, size_type len = npos)
        {
            assert(pos < size_);
            if (len == npos || pos + len >= size_)
            {
                size_ = pos;
                str_[pos] = value_type(0);
            }
            else
            {
                Traits::move(str_ + pos, str_ + pos + len, size_ - pos - len + 1);
                size_ -= len;
            }
        }

        void pop_back()
        {
            if (size_)
            {
                --size_;
                str_[size_] = value_type(0);
            }
        }

        void swap(basic_string &o) noexcept
        {
            std::swap(str_, o.str_);
            std::swap(size_, o.size_);
            std::swap(capacity_, o.capacity_);
        }

        // 查找
        size_type find(value_type ch, size_type pos = 0) const noexcept
        {
            for (; pos < size_; ++pos)
                if (str_[pos] == ch)
                    return pos;
            return npos;
        }

        size_type find(const_pointer s, size_type pos = 0) const noexcept
        {
            size_type l = Traits::length(s);
            for (; pos + l <= size_; ++pos)
            {
                if (Traits::compare(str_ + pos, s, l) == 0)
                    return pos;
            }
            return npos;
        }

        basic_string substr(size_type pos, size_type len = npos) const
        {
            basic_string r;
            if (pos > size_)
                return r;
            if (len == npos || pos + len > size_)
                len = size_ - pos;
            r.reserve(len);
            Traits::copy(r.str_, str_ + pos, len);
            r.size_ = len;
            r.str_[len] = value_type(0);
            return r;
        }

        // 比较运算符
        bool operator<(const basic_string &s) const noexcept
        {
            size_type m = std::min(size_, s.size_);
            int c = Traits::compare(str_, s.str_, m);
            return c < 0 || (c == 0 && size_ < s.size_);
        }
        bool operator>(const basic_string &s) const noexcept { return s < *this; }
        bool operator==(const basic_string &s) const noexcept
        {
            return size_ == s.size_ && Traits::compare(str_, s.str_, size_) == 0;
        }
        bool operator<=(const basic_string &s) const noexcept { return !(*this > s); }
        bool operator>=(const basic_string &s) const noexcept { return !(*this < s); }
        bool operator!=(const basic_string &s) const noexcept { return !(*this == s); }

        // I/O 操作
        friend std::istream &operator>>(std::istream &is, basic_string &str)
        {
            value_type buf[4096];
            is >> buf;
            basic_string t(buf);
            str.swap(t);
            return is;
        }

        friend std::ostream &operator<<(std::ostream &os, const basic_string &str)
        {
            for (size_type i = 0; i < str.size_; ++i)
                os << str.str_[i];
            return os;
        }

    private:
        value_type *str_ = nullptr; // 数据存储指针
        size_type size_ = 0;        // 当前元素数量
        size_type capacity_ = 0;    // 当前分配容量
        allocator_type alloc_;      // 分配器实例
    };

    using string = basic_string<char>;
    using wstring = basic_string<wchar_t>;
}; // namespace zstl