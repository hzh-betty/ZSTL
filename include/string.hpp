#pragma once
#include <iostream>
#include <cassert>
#include <cstring>
namespace zstl
{
    // 通用 char_traits
    template <class CharType>
    struct char_traits
    {
        using char_type = CharType;
        // 计算长度：遍历直到终止符
        static size_t length(const char_type *str) noexcept
        {
            size_t len = 0;
            for (; *str != char_type(0); ++str)
                ++len;
            return len;
        }
        // 比较前 n 个字符
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
        // 拷贝不重叠区间
        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            assert(src + n <= dst || dst + n <= src);
            char_type *r = dst;
            for (; n; --n, ++dst, ++src)
                *dst = *src;
            return r;
        }
        // 移动支持重叠
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
        // 填充指定字符
        static char_type *fill(char_type *dst, char_type ch, size_t count) noexcept
        {
            char_type *r = dst;
            for (; count; --count, ++dst)
                *dst = ch;
            return r;
        }
    };

    // char 特化使用库函数
    template <>
    struct char_traits<char>
    {
        using char_type = char;
        static size_t length(const char_type *str) noexcept { return strlen(str); }
        static int compare(const char_type *s1, const char_type *s2, size_t n) noexcept { return memcmp(s1, s2, n); }
        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            assert(src + n <= dst || dst + n <= src);
            return (char *)memcpy(dst, src, n);
        }
        static char_type *move(char_type *dst, const char_type *src, size_t n) noexcept { return (char *)memmove(dst, src, n); }
        static char_type *fill(char_type *dst, char_type ch, size_t count) noexcept { return (char *)memset(dst, ch, count); }
    };

    // basic_string 类：所有函数在类内实现
    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_string
    {
    public:
        using char_type = CharT;
        using traits_type = Traits;
        using size_type = size_t;
        using iterator = CharT *;
        using const_iterator = const CharT *;
        inline static const size_type npos = -1;

        // 默认构造：分配 1，写入终止符
        basic_string()
        {
            str_ = new CharT[1];
            Traits::fill(str_, CharT(0), 1);
        }
        // C 字符串构造：复制并设置容量
        basic_string(const CharT *s)
        {
            size_ = Traits::length(s);
            capacity_ = size_;
            str_ = new CharT[capacity_ + 1];
            Traits::copy(str_, s, size_ + 1);
        }
        // 拷贝构造：深拷贝缓冲区
        basic_string(const basic_string &o)
            : size_(o.size_), capacity_(o.capacity_)
        {
            if(o.str_)
            {
                str_ = new CharT[capacity_ + 1];
                Traits::copy(str_, o.str_, size_ + 1);
            }
        }
        // 移动构造：接管指针，置空源
        basic_string(basic_string &&o) noexcept
            : str_(o.str_), size_(o.size_), capacity_(o.capacity_)
        {
            o.str_ = nullptr;
            o.size_ = o.capacity_ = 0;
        }
        ~basic_string() { delete[] str_; }

        // 拷贝赋值：拷贝-交换
        basic_string &operator=(const basic_string &o)
        {
            if (this != &o)
            {
                basic_string tmp(o);
                swap(tmp);
            }
            return *this;
        }
        // 移动赋值：释放后接管
        basic_string &operator=(basic_string &&o) noexcept
        {
            if (this != &o)
            {
                delete[] str_;
                str_ = o.str_;
                size_ = o.size_;
                capacity_ = o.capacity_;
                o.str_ = nullptr;
                o.size_ = o.capacity_ = 0;
            }
            return *this;
        }

        // 迭代器
        iterator begin() noexcept { return str_; }
        const_iterator begin() const noexcept { return str_; }
        iterator end() noexcept { return str_ + size_; }
        const_iterator end() const noexcept { return str_ + size_; }

        // 容量相关
        size_type size() const { return size_; }
        size_type capacity() const { return capacity_; }
        bool empty() const noexcept { return size_ == 0; }
        void reserve(size_type n)
        {
            if (n > capacity_)
            {
                CharT *tmp = new CharT[n + 1];
                Traits::copy(tmp, str_, size_ + 1);
                delete[] str_;
                str_ = tmp;
                capacity_ = n;
            }
        }
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
        void clear() noexcept
        {
            size_ = 0;
            if (str_)
                Traits::fill(str_, CharT(0), 1);
        }

        // 元素访问
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
        CharT &front()
        {
            assert(size_);
            return str_[0];
        }
        const CharT &front() const
        {
            assert(size_);
            return str_[0];
        }
        CharT &back()
        {
            assert(size_);
            return str_[size_ - 1];
        }
        const CharT &back() const
        {
            assert(size_);
            return str_[size_ - 1];
        }
        const CharT *c_str() const { return str_; }

        // 修改器
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
        void append(const CharT *s)
        {
            size_type l = Traits::length(s);
            if (size_ + l > capacity_)
                reserve(size_ + l);
            Traits::copy(str_ + size_, s, l + 1);
            size_ += l;
        }
        void insert(size_type pos, CharT ch)
        {
            assert(pos <= size_);
            if (size_ + 1 > capacity_)
                reserve(capacity_ ? capacity_ * 2 : 4);
            Traits::move(str_ + pos + 1, str_ + pos, size_ - pos + 1);
            str_[pos] = ch;
            ++size_;
        }

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

        // 查找与子串
        size_type find(CharT ch, size_type pos = 0) const
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
                if (Traits::compare(str_ + pos, s, l) == 0)
                    return pos;
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
        bool operator<(const basic_string &s) const
        {
            size_type m = size_ < s.size_ ? size_ : s.size_;
            int c = Traits::compare(str_, s.str_, m);
            return c < 0 || (c == 0 && size_ < s.size_);
        }
        bool operator>(const basic_string &s) const { return s < *this; }
        bool operator==(const basic_string &s) const { return size_ == s.size_ && Traits::compare(str_, s.str_, size_) == 0; }
        bool operator<=(const basic_string &s) const { return !(*this > s); }
        bool operator>=(const basic_string &s) const { return !(*this < s); }
        bool operator!=(const basic_string &s) const { return !(*this == s); }

        // I/O 友元
        friend std::istream &operator>>(std::istream &is, basic_string &str)
        {
            CharT *buf = new CharT[4096];
            is >> buf;
            basic_string t(buf);
            str = std::move(t);
            delete[] buf;
            return is;
        }
        friend std::ostream &operator<<(std::ostream &os, const basic_string &str)
        {
            for (size_type i = 0; i < str.size_; ++i)
                os << str.str_[i];
            return os;
        }

    private:
        CharT *str_ = nullptr;
        size_type size_ = 0;
        size_type capacity_ = 0;
    };

    using string = basic_string<char>;
    using wstring = basic_string<wchar_t>;
};