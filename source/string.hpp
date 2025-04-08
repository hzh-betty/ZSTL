#pragma once
#include <iostream>
#include <cstring>
#include <cassert>

namespace zstl
{
    class string
    {
    public:
        typedef char *iterator;
        typedef const char *const_iterator;
        static const size_t npos = -1; // 末尾特殊标记

        // 迭代器操作（直接使用原生指针实现）
        iterator begin() { return str_; }             // 返回首元素指针
        const_iterator begin() const { return str_; } // const版本迭代器
        iterator end() { return str_ + size_; }       // 返回末元素后一位指针
        const_iterator end() const { return str_ + size_; }

    public:
        // 构造函数：默认构造空字符串，深拷贝C字符串
        string(const char *str = "")
            : size_(strlen(str)),
              capacity_(size_),
              str_(new char[capacity_ + 1]) // +1存放'\0'
        {
            strcpy(str_, str);
        }

        // 拷贝构造：通过临时对象交换实现深拷贝
        string(const string &tmp) : size_(0), capacity_(0), str_(nullptr)
        {
            string t(tmp.str_); // 构造临时对象
            swap(t);            // 资源交换
        }

        // 赋值运算符：传值调用自动复用拷贝构造
        string &operator=(string tmp)
        {
            swap(tmp); // 交换资源后tmp自动析构旧数据
            return *this;
        }

        // 获取C风格字符串（保证以'\0'结尾）
        const char *c_str() const { return str_; }

        // 下标访问：非const版本可修改
        char &operator[](size_t pos)
        {
            assert(pos < size_); // 越界检查
            return str_[pos];
        }
        const char &operator[](size_t pos) const
        {
            assert(pos < size_);
            return str_[pos];
        }

        // 容量相关操作
        size_t capacity() const { return capacity_; } // 当前分配容量
        size_t size() const { return size_; }         // 实际字符数（不含'\0'）

        // 扩容：仅当n>当前容量时生效
        void reserve(size_t n)
        {
            if (n > capacity_)
            {
                char *tmp = new char[n + 1]; // 新空间
                strcpy(tmp, str_);           // 复制原内容
                delete[] str_;               // 释放旧空间
                str_ = tmp;
                capacity_ = n; // 更新容量
            }
        }

        // 调整大小：newSize>size时填充字符，否则截断
        void resize(size_t newSize, char c = '\0')
        {
            if (newSize > size_)
            {
                if (newSize > capacity_)
                    reserve(newSize);
                memset(str_ + size_, c, newSize - size_); // 填充指定字符
            }
            size_ = newSize;
            str_[newSize] = '\0'; // 确保终止符
        }

        void clear()
        {
            str_[0] = '\0'; // 清空内容
            size_ = 0;
        }

        bool empty() const { return size_ == 0; }

        // 追加单个字符（2倍扩容策略）
        void push_back(char ch)
        {
            if (size_ == capacity_) // 容量检查
            {
                size_t newCapacity = capacity_ ? capacity_ * 2 : 4;
                reserve(newCapacity);
            }
            str_[size_++] = ch; // 写入字符
            str_[size_] = '\0'; // 维护终止符
        }

        // 追加C字符串
        void append(const char *s)
        {
            size_t len = strlen(s);
            if (len + size_ > capacity_)
                reserve(len + size_);
            memcpy(str_ + size_, s, len + 1); // 包含'\0'
            size_ += len;
        }

        // 运算符重载复用已有函数
        string &operator+=(char ch)
        {
            push_back(ch);
            return *this;
        }
        string &operator+=(const char *s)
        {
            append(s);
            return *this;
        }

        // 查找字符/子串（返回位置或npos）
        size_t find(char ch, size_t pos = 0) const
        {
            for (; pos < size_; ++pos)
                if (str_[pos] == ch)
                    return pos;
            return npos;
        }
        size_t find(const char *s, size_t pos = 0) const
        {
            const char *p = strstr(str_ + pos, s);
            return p ? p - str_ : npos;
        }

        // 截取子串：处理npos和越界情况
        string substr(size_t pos, size_t len = npos)
        {
            string ret;
            size_t end = pos + len;
            if (len == npos || end > size_)
            {
                len = size_ - pos;
                end = size_;
            }
            ret.reserve(len);
            for (; pos < end; ++pos)
                ret += str_[pos];
            return ret;
        }

        // 插入操作
        void insert(size_t pos, char ch)
        {
            assert(pos <= size_);
            if (size_ == capacity_)
                reserve(capacity_ ? capacity_ * 2 : 4);

            // 从后向前移动元素
            for (size_t i = size_ + 1; i > pos; --i)
                str_[i] = str_[i - 1];
            str_[pos] = ch;
            ++size_;
            str_[size_] = '\0';
        }

        void insert(size_t pos, const char *s)
        {
            assert(pos <= size_);
            size_t len = strlen(s);
            if (size_ + len > capacity_)
                reserve(size_ + len);

            // 移动原有元素
            for (size_t i = size_; i >= pos; --i)
                str_[i + len] = str_[i];
            memcpy(str_ + pos, s, len);
            size_ += len;
        }

        // 首尾元素访问
        char &front() { return str_[0]; }
        char &back() { return str_[size_ - 1]; }
        const char &front() const { return str_[0]; }
        const char &back() const { return str_[size_ - 1]; }

        void pop_back()
        {
            if (size_ > 0)
                str_[--size_] = '\0';
        }

        // 删除操作：处理npos和越界
        void erase(size_t pos, size_t len = npos)
        {
            assert(pos < size_);
            if (len >= size_ - pos || len == npos)
            {
                str_[pos] = '\0';
                size_ = pos;
            }
            else
            {
                // 向前移动元素
                for (size_t i = pos + len; i <= size_; ++i)
                    str_[i - len] = str_[i];
                size_ -= len;
            }
        }

        // 比较运算符（直接使用strcmp）
        bool operator<(const string &s) const { return strcmp(str_, s.str_) < 0; }
        bool operator>(const string &s) const { return strcmp(str_, s.str_) > 0; }
        bool operator==(const string &s) const { return strcmp(str_, s.str_) == 0; }
        bool operator<=(const string &s) const { return !(*this > s); }
        bool operator>=(const string &s) const { return !(*this < s); }
        bool operator!=(const string &s) const { return !(*this == s); }

        // 析构函数：释放堆内存
        ~string()
        {
            delete[] str_;  // 释放字符数组
            str_ = nullptr; // 防止悬垂指针
            size_ = capacity_ = 0;
        }

        // 高效交换：直接交换成员变量
        void swap(string &s)
        {
            std::swap(size_, s.size_);
            std::swap(capacity_, s.capacity_);
            std::swap(str_, s.str_);
        }

    private:
        size_t size_;     // 有效字符数（不含'\0'）
        size_t capacity_; // 分配的空间容量
        char *str_;       // 堆内存指针
    };
    // 流输出：使用迭代器遍历输出
    std::ostream &operator<<(std::ostream &out, const string &s)
    {
        for (auto &ch : s)
            out << ch;
        return out;
    }

    // 流输入：使用缓冲区减少扩容次数
    std::istream &operator>>(std::istream &in, string &s)
    {
        s.clear();
        char buf[128]; // 局部缓冲区
        int i = 0;
        char ch = in.get();

        // 读取直到空格或换行
        while (ch != ' ' && ch != '\n')
        {
            buf[i++] = ch;
            if (i == 127)
            { // 缓冲区满时批量处理
                buf[i] = '\0';
                s += buf;
                i = 0;
            }
            ch = in.get();
        }
        if (i > 0)
        { // 处理剩余数据
            buf[i] = '\0';
            s += buf;
        }
        return in;
    }
}