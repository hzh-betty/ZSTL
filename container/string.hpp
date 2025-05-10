#pragma once
#include <iostream>
#include <cassert>
#include <cstring>
#include"../iterator/reverse_iterator.hpp"

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
            for (; *str != char_type(0); ++str) // 遍历直到遇到终止符
                ++len;
            return len;
        }

        // 按字典序比较两个字符串的前n个字符
        static int compare(const char_type *s1, const char_type *s2, size_t n) noexcept
        {
            for (; n; --n, ++s1, ++s2) // 逐字符比较
            {
                if (*s1 < *s2)
                    return -1; // 小于时返回-1
                if (*s2 < *s1)
                    return 1; // 大于时返回1
            }
            return 0; // 相等时返回0
        }

        // 安全拷贝n个字符（要求源和目标内存不重叠）
        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            assert(src + n <= dst || dst + n <= src); // 确保内存不重叠
            char_type *r = dst;
            for (; n; --n, ++dst, ++src) // 逐字符拷贝
                *dst = *src;
            return r;
        }

        // 安全移动n个字符（支持内存重叠情况）
        static char_type *move(char_type *dst, const char_type *src, size_t n) noexcept
        {
            char_type *r = dst;
            if (dst < src) // 正向拷贝（当目标在源前面时）
            {
                for (; n; --n, ++dst, ++src)
                    *dst = *src;
            }
            else if (src < dst) // 逆向拷贝（当目标在源后面时）
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
            for (; count; --count, ++dst) // 逐字符填充
                *dst = ch;
            return r;
        }
    };

    // char类型特化版本，使用标准库函数优化性能
    template <>
    struct char_traits<char>
    {
        using char_type = char;
        // 直接调用标准库函数实现各操作
        static size_t length(const char_type *str) noexcept { return strlen(str); }
        static int compare(const char_type *s1, const char_type *s2, size_t n) noexcept { return memcmp(s1, s2, n); }
        static char_type *copy(char_type *dst, const char_type *src, size_t n) noexcept
        {
            assert(src + n <= dst || dst + n <= src);
            return (char *)memcpy(dst, src, n); // 内存拷贝
        }
        static char_type *move(char_type *dst, const char_type *src, size_t n) noexcept { return (char *)memmove(dst, src, n); } // 内存移动
        static char_type *fill(char_type *dst, char_type ch, size_t count) noexcept { return (char *)memset(dst, ch, count); }   // 内存填充
    };

    // 动态字符串模板类
    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_string
    {
    public:
        // 类型别名定义
        using char_type = CharT;
        using size_type = size_t;
        using iterator = CharT *;
        using const_iterator = const CharT *;
        inline static const size_type npos = -1; // 特殊常量表示无效位置

        // 迭代器访问接口
        iterator begin() noexcept { return str_; }             // 起始迭代器
        const_iterator begin() const noexcept { return str_; } // const版本
        iterator end() noexcept { return str_ + size_; }       // 末尾迭代器
        const_iterator end() const noexcept { return str_ + size_; }

        // 反向迭代器
        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<iterator>;

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    public:
        // 默认构造函数：创建空字符串（含终止符）
        basic_string()
        {
            str_ = new CharT[1];             // 分配最小内存
            Traits::fill(str_, CharT(0), 1); // 写入终止符
        }

        // C风格字符串构造函数：深拷贝内容
        basic_string(const CharT *s)
        {
            size_ = Traits::length(s);        // 获取源字符串长度
            capacity_ = size_;                // 容量与长度一致
            str_ = new CharT[capacity_ + 1];  // 分配足够内存
            Traits::copy(str_, s, size_ + 1); // 拷贝内容含终止符
        }

        // 拷贝构造函数：深拷贝对象
        basic_string(const basic_string &o)
            : size_(o.size_), capacity_(o.capacity_)
        {
            str_ = new CharT[capacity_ + 1];       // 分配新内存
            Traits::copy(str_, o.str_, size_ + 1); // 拷贝内容
        }

        // 移动构造函数：资源转移（原对象置空）
        basic_string(basic_string &&o) noexcept
            : str_(o.str_), size_(o.size_), capacity_(o.capacity_)
        {
            o.str_ = nullptr;          // 源对象指针置空
            o.size_ = o.capacity_ = 0; // 源对象尺寸清零
        }

        ~basic_string()
        {
            size_ = capacity_ = 0;
            delete[] str_;
            str_ = nullptr;
        } // 析构时释放内存

        // 拷贝赋值运算符：使用拷贝交换范式
        basic_string &operator=(const basic_string &o)
        {
            if (this != &o) // 防止自赋值
            {
                basic_string tmp(o); // 创建临时副本
                swap(tmp);           // 交换资源
            }
            return *this;
        }

        // 移动赋值运算符：直接接管资源
        basic_string &operator=(basic_string &&o) noexcept
        {
            if (this != &o) // 防止自移动
            {
                delete[] str_;           // 释放原有内存
                str_ = o.str_;           // 接管指针
                size_ = o.size_;         // 转移尺寸
                capacity_ = o.capacity_; // 转移容量
                o.str_ = nullptr;        // 置空源对象
                o.size_ = o.capacity_ = 0;
            }
            return *this;
        }

        // 容量相关方法
        size_type size() const { return size_; }           // 当前元素数量
        size_type capacity() const { return capacity_; }   // 当前分配容量
        bool empty() const noexcept { return size_ == 0; } // 判空检查

        // 预分配内存（至少扩容到n）
        void reserve(size_type n)
        {
            if (n > capacity_) // 仅当需要扩容时操作
            {
                CharT *tmp = new CharT[n + 1];      // 分配新内存
                Traits::copy(tmp, str_, size_ + 1); // 拷贝原有内容
                delete[] str_;                      // 释放旧内存
                str_ = tmp;                         // 更新指针
                capacity_ = n;                      // 更新容量
            }
        }

        // 调整字符串长度（扩展时填充指定字符）
        void resize(size_type n, CharT c = CharT())
        {
            if (n > size_) // 扩展操作
            {
                reserve(n);                               // 确保容量足够
                Traits::fill(str_ + size_, c, n - size_); // 填充新增空间
            }
            size_ = n;              // 更新尺寸
            str_[size_] = CharT(0); // 确保终止符
        }

        // 清空内容（尺寸归零，保留终止符）
        void clear() noexcept
        {
            size_ = 0;
            if (str_)
                Traits::fill(str_, CharT(0), 1); // 写入终止符
        }

        // 元素访问接口
        CharT &operator[](size_type p) // 非const下标访问
        {
            assert(p < size_); // 边界检查
            return str_[p];
        }
        const CharT &operator[](size_type p) const // const下标访问
        {
            assert(p < size_);
            return str_[p];
        }

        CharT &front() { return operator[](0); } // 首元素
        const CharT &front() const { return operator[](0); }
        CharT &back() { return operator[](size_ - 1); } // 末元素
        const CharT &back() const { return operator[](size_ - 1); }
        const CharT *c_str() const { return str_; } // C风格字符串指针

        // 修改操作
        void push_back(CharT ch) // 追加单个字符
        {
            if (size_ + 1 > capacity_)                  // 容量检查
                reserve(capacity_ ? capacity_ * 2 : 4); // 按需扩容（初始容量为4）
            str_[size_++] = ch;                         // 写入字符
            str_[size_] = CharT(0);                     // 更新终止符
        }

        basic_string &operator+=(CharT ch)
        {
            push_back(ch);
            return *this;
        } // 追加字符运算符

        basic_string &operator+=(const CharT *s)
        {
            append(s);
            return *this;
        } // 追加字符串运算符

        void append(const CharT *s) // 追加字符串
        {
            size_type l = Traits::length(s);      // 计算追加长度
            if (size_ + l > capacity_)            // 容量检查
                reserve(size_ + l);               // 扩容到刚好容纳
            Traits::copy(str_ + size_, s, l + 1); // 拷贝内容（含终止符）
            size_ += l;                           // 更新尺寸
        }

        // 在指定位置插入字符（可能引发内存移动）
        void insert(size_type pos, CharT ch)
        {
            assert(pos <= size_);                       // 位置合法性检查
            if (size_ + 1 > capacity_)                  // 容量检查
                reserve(capacity_ ? capacity_ * 2 : 4); // 扩容策略

            // 移动后续字符腾出位置
            Traits::move(str_ + pos + 1, str_ + pos, size_ - pos + 1);
            str_[pos] = ch; // 插入新字符
            ++size_;        // 更新尺寸
        }

        // 在指定位置插入字符串
        void insert(size_type pos, const CharT *s)
        {
            assert(pos <= size_);
            size_type l = Traits::length(s); // 计算插入长度
            if (size_ + l > capacity_)       // 容量检查
                reserve(size_ + l);          // 扩容到刚好容纳

            // 移动原有数据腾出空间
            Traits::move(str_ + pos + l, str_ + pos, size_ - pos + 1);
            Traits::copy(str_ + pos, s, l); // 插入新内容
            size_ += l;                     // 更新尺寸
        }

        // 删除从pos开始的len个字符
        void erase(size_type pos, size_type len = npos)
        {
            assert(pos < size_);                   // 位置合法性检查
            if (len == npos || pos + len >= size_) // 删除到末尾的情况
            {
                size_ = pos;          // 直接截断
                str_[pos] = CharT(0); // 写入终止符
            }
            else // 部分删除
            {
                // 将后续内容前移覆盖
                Traits::move(str_ + pos, str_ + pos + len, size_ - pos - len + 1);
                size_ -= len; // 更新尺寸
            }
        }

        void pop_back() // 删除末尾字符
        {
            if (size_ != 0)
            {
                --size_;                // 尺寸减一
                str_[size_] = CharT(0); // 更新终止符
            }
        }

        void swap(basic_string &o) noexcept // 交换两个对象内容
        {
            std::swap(str_, o.str_);           // 交换指针
            std::swap(size_, o.size_);         // 交换尺寸
            std::swap(capacity_, o.capacity_); // 交换容量
        }

        // 查找功能
        size_type find(CharT ch, size_type pos = 0) const // 查找字符
        {
            for (; pos < size_; ++pos) // 线性搜索
                if (str_[pos] == ch)
                    return pos;
            return npos; // 未找到
        }

        size_type find(const CharT *s, size_type pos = 0) const noexcept // 查找子串
        {
            size_type l = Traits::length(s); // 子串长度
            for (; pos + l <= size_; ++pos)  // 滑动窗口遍历
            {
                if (Traits::compare(str_ + pos, s, l) == 0) // 匹配检查
                    return pos;
            }
            return npos;
        }

        basic_string substr(size_type pos, size_type len = npos) const // 获取子串
        {
            basic_string r;
            if (pos > size_)
                return r; // 非法位置返回空

            if (len == npos || pos + len > size_) // 处理默认长度
                len = size_ - pos;

            r.reserve(len + 1);                    // 预分配内存
            Traits::copy(r.str_, str_ + pos, len); // 拷贝内容
            r.size_ = len;                         // 设置尺寸
            r.str_[len] = CharT(0);                // 终止符
            return r;
        }

        // 比较运算符
        bool operator<(const basic_string &s) const // 字典序小于比较
        {
            size_type m = std::min(size_, s.size_);
            int c = Traits::compare(str_, s.str_, m);    // 比较共同长度部分
            return c < 0 || (c == 0 && size_ < s.size_); // 短字符串优先
        }
        bool operator>(const basic_string &s) const { return s < *this; } // 反向调用
        bool operator==(const basic_string &s) const                      // 完全相等比较
        {
            return size_ == s.size_ && Traits::compare(str_, s.str_, size_) == 0;
        }
        bool operator<=(const basic_string &s) const { return !(*this > s); } // 组合逻辑
        bool operator>=(const basic_string &s) const { return !(*this < s); }
        bool operator!=(const basic_string &s) const { return !(*this == s); }

        // I/O操作友元函数
        friend std::istream &operator>>(std::istream &is, basic_string &str)
        {
            CharT *buf = new CharT[4096]; // 临时缓冲区
            is >> buf;                    // 从输入流读取
            basic_string t(buf);          // 构造临时对象
            str = std::move(t);           // 移动赋值
            delete[] buf;                 // 释放缓冲区
            return is;
        }

        friend std::ostream &operator<<(std::ostream &os, const basic_string &str)
        {
            for (size_type i = 0; i < str.size_; ++i) // 逐字符输出
                os << str.str_[i];
            return os;
        }

    private:
        char_type *str_ = nullptr; // 数据存储指针
        size_type size_ = 0;       // 当前元素数量
        size_type capacity_ = 0;   // 当前分配容量
    };
    using string = basic_string<char>;
    using wstring = basic_string<wchar_t>;
};