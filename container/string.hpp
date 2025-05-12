#pragma once
#include <iostream>
#include <cassert>
#include <cstring>
#include "../iterator/reverse_iterator.hpp"
#include "../allocator/alloc.hpp"

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

    // 动态字符串模板类，使用 zstl::allocator 管理内存
    template <typename CharT, typename Traits = char_traits<CharT>,typename Alloc = alloc<CharT>>
    class basic_string
    {
    public:

        using char_type = CharT;
        using allocator_type = Alloc;
        using size_type = size_t;
        inline static const size_type npos = static_cast<size_type>(-1);

        using iterator = char_type *;
        using const_iterator = const char_type *;

        // 迭代器接口
        iterator begin() noexcept { return str_; }
        const_iterator begin() const noexcept { return str_; }
        iterator end() noexcept { return str_ + size_; }
        const_iterator end() const noexcept { return str_ + size_; }

        // 反向迭代器
        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<const_iterator>;
        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    public:
        // 默认构造：空字符串，含终止符
        basic_string()
            : str_(allocator_type::allocate(1)), size_(0), capacity_(0)
        {
            Traits::fill(str_, CharT(0), 1);
        }

        // C风格字符串构造：深拷贝
        basic_string(const CharT *s)
        {
            size_ = Traits::length(s);
            capacity_ = size_;
            str_ = allocator_type::allocate(capacity_ + 1);
            Traits::copy(str_, s, size_ + 1);
        }

        // 拷贝构造
        basic_string(const basic_string &o)
            : size_(o.size_), capacity_(o.capacity_)
        {
            str_ = allocator_type::allocate(capacity_ + 1);
            Traits::copy(str_, o.str_, size_ + 1);
        }

        // 移动构造
        basic_string(basic_string &&o) noexcept
            : str_(o.str_), size_(o.size_), capacity_(o.capacity_)
        {
            o.str_ = nullptr;
            o.size_ = o.capacity_ = 0;
        }

        // 析构：销毁并释放
        ~basic_string()
        {
            if (str_)
            {
                // 清空内容后释放内存
                size_ = capacity_ = 0;
                allocator_type::deallocate(str_, capacity_ + 1);
                str_ = nullptr;
            }
        }

        // 拷贝赋值，拷贝-交换范式
        basic_string &operator=(const basic_string &o)
        {
            if (this != &o)
            {
                basic_string tmp(o);
                swap(tmp);
            }
            return *this;
        }

        // 移动赋值
        basic_string &operator=(basic_string &&o) noexcept
        {
            if (this != &o)
            {
                allocator_type::deallocate(str_, capacity_ + 1);
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
                CharT *tmp = allocator_type::allocate(n + 1);
                Traits::copy(tmp, str_, size_ + 1);
                allocator_type::deallocate(str_, capacity_ + 1);
                str_ = tmp;
                capacity_ = n;
            }
        }

        // 调整长度
        void resize(size_type n, CharT c = CharT())
        {
            if (n > size_)
            {
                reserve(n);
                Traits::fill(str_ + size_, c, n - size_);
            }
            size_ = n;
            str_[size_] = CharT(0);
        }

        // 清空内容
        void clear() noexcept
        {
            size_ = 0;
            if (str_)
                Traits::fill(str_, CharT(0), 1);
        }

        // 下标访问
        CharT &operator[](size_type p)
        {
            assert(p < size_);
            return str_[p];
        }
        const CharT &operator[](size_type p) const
        {
            assert(p < size_);
            return str_[p];
        }

        CharT &front() { return operator[](0); }
        const CharT &front() const { return operator[](0); }
        CharT &back() { return operator[](size_ - 1); }
        const CharT &back() const { return operator[](size_ - 1); }
        const CharT *c_str() const noexcept { return str_; }

        // 追加字符
        void push_back(CharT ch)
        {
            if (size_ + 1 > capacity_)
                reserve(capacity_ ? capacity_ * 2 : 4);
            str_[size_++] = ch;
            str_[size_] = CharT(0);
        }

        basic_string &operator+=(CharT ch)
        {
            push_back(ch);
            return *this;
        }

        basic_string &operator+=(const CharT *s)
        {
            append(s);
            return *this;
        }

        // 追加字符串
        void append(const CharT *s)
        {
            size_type l = Traits::length(s);
            if (size_ + l > capacity_)
                reserve(size_ + l);
            Traits::copy(str_ + size_, s, l + 1);
            size_ += l;
        }

        // 插入字符
        void insert(size_type pos, CharT ch)
        {
            assert(pos <= size_);
            if (size_ + 1 > capacity_)
                reserve(capacity_ ? capacity_ * 2 : 4);
            Traits::move(str_ + pos + 1, str_ + pos, size_ - pos + 1);
            str_[pos] = ch;
            ++size_;
        }

        // 插入字符串
        void insert(size_type pos, const CharT *s)
        {
            assert(pos <= size_);
            size_type l = Traits::length(s);
            if (size_ + l > capacity_)
                reserve(size_ + l);
            Traits::move(str_ + pos + l, str_ + pos, size_ - pos + 1);
            Traits::copy(str_ + pos, s, l);
            size_ += l;
        }

        // 删除子串
        void erase(size_type pos, size_type len = npos)
        {
            assert(pos < size_);
            if (len == npos || pos + len >= size_)
            {
                size_ = pos;
                str_[pos] = CharT(0);
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
                str_[size_] = CharT(0);
            }
        }

        void swap(basic_string &o) noexcept
        {
            std::swap(str_, o.str_);
            std::swap(size_, o.size_);
            std::swap(capacity_, o.capacity_);
        }

        // 查找
        size_type find(CharT ch, size_type pos = 0) const noexcept
        {
            for (; pos < size_; ++pos)
                if (str_[pos] == ch)
                    return pos;
            return npos;
        }

        size_type find(const CharT *s, size_type pos = 0) const noexcept
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
            r.str_[len] = CharT(0);
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
            CharT buf[4096];
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
        CharT *str_ = nullptr;   // 数据存储指针
        size_type size_ = 0;     // 当前元素数量
        size_type capacity_ = 0; // 当前分配容量
    };

    using string = basic_string<char>;
    using wstring = basic_string<wchar_t>;
}; // namespace zstl