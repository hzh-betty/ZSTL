#pragma once
#include <iostream>
#include <cstring>
#include <cassert>
namespace zstl
{
    class string
    {
    public:
    public:
        // 构造函数
        string(const char *str = "")
            : size_(strlen(str)), capacity_(size_), str_(new char[capacity_ + 1])
        {
            strcpy(str_, str);
        }

        // 拷贝函数
        string(const string &tmp)
            : size_(0), capacity_(0), str_(nullptr)
        {
            string t(tmp.str_);
            swap(t);
        }
        // 赋值重载
        string &operator=(string tmp)
        {
            swap(tmp);
            return *this;
        }

        // 获取c风格字符串
        const char *c_str() const
        {
            return str_;
        }

        // []重载
        char &operator[](size_t pos)
        {
            assert(pos < size_);
            return str_[pos];
        }

        const char &operator[](size_t pos) const
        {
            assert(pos < size_);
            return str_[pos];
        }

        size_t capacity() const
        {
            return capacity_;
        }

        size_t size() const
        {
            return size_;
        }

        void reserve(size_t n)
        {
            if (n > capacity_)
            {
                char *tmp = new char[n + 1];
                strcpy(tmp, str_);
                capacity_ = n;
                delete[] str_;
                str_ = tmp;
            }
        }

        void resize(size_t newSize, char c = '\0')
        {
            // 1. 如果大于size
            if (newSize > size_)
            {
                // 2. 如果大于capacity
                if (newSize > capacity_)
                {
                    reserve(newSize);
                }
                memset(str_ + size_, c, newSize - size_);
            }

            // 3. 调整大小
            size_ = newSize;
            str_[newSize] = '\0';
        }

        void clear()
        {
            str_[0] = '\0';
            size_ = 0;
        }

        bool empty() const
        {
            return size_ == 0;
        }

        void push_back(char ch)
        {
            // 1. 判断是否需要扩容
            if (size_ == capacity_)
            {
                size_t newCapacity = capacity_ == 0 ? 4 : capacity_ * 2;
                reserve(newCapacity);
            }

            // 2. 赋值
            str_[size_++] = ch;
            str_[size_] = '\0';
        }

        void append(const char *s)
        {
            size_t len = strlen(s);
            if (len + size_ > capacity_)
            {
                reserve(len + size_);
            }
            memcpy(str_ + size_, s, len + 1);
            size_ += len;
        }

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

        size_t find(char ch, size_t pos = 0) const
        {
            for (size_t i = pos; i < size_; i++)
            {
                if (str_[i] == ch)
                {
                    return i;
                }
            }
            return npos;
        }

        size_t find(const char *s, size_t pos = 0) const
        {
            const char *p = strstr(str_ + pos, s);
            if (p != nullptr)
            {
                return p - str_;
            }
            return npos;
        }

        string substr(size_t pos, size_t len = npos)
        {
            string ret;
            size_t end = pos + len;
            // 1.判断是否越界
            if (len == npos || pos + len >= size_)
            {
                len = size_ - pos;
                end = size_;
            }

            // 2. 赋值
            ret.reserve(len);
            for (size_t i = pos; i < end; i++)
            {
                ret += str_[i];
            }
            return ret;
        }

        void insert(size_t pos, char ch)
        {
            assert(pos <= size_);

            // 1. 判断是否需要扩容
            if (size_ == capacity_)
            {
                size_t newCapacity = capacity_ == 0 ? 4 : capacity_ * 2;
                reserve(newCapacity);
            }

            // 2. 移动并赋值
            size_t end = size_ + 1;
            while (end > pos)
            {
                str_[end] = str_[end + 1];
                --end;
            }
            str_[end] = ch;
            ++size_;
        }

        void insert(size_t pos, const char *s)
        {
            assert(pos <= size_);

            // 1. 扩容
            size_t len = strlen(s);
            if (size_ + len > capacity_)
            {
                reserve(size_ + len);
            }

            // 2. 移动
            size_t end = size_ + len;
            while (end > pos)
            {
                str_[end] = str_[end - len];
                --end;
            }
            memcpy(str_ + pos, s, len);
        }
        // 析构函数
        ~string()
        {
            if (str_)
            {
                delete str_;
                str_ = nullptr;
                size_ = capacity_ = 0;
            }
        }

    private:
        void swap(string &s)
        {
            std::swap(size_, s.size_);
            std::swap(capacity_, s.capacity_);
            std::swap(str_, s.str_);
        }

    private:
        size_t size_;                      // 当前大小
        size_t capacity_;                  // 当前容量
        char *str_;                        // 存储的字符串
        static constexpr size_t npos = -1; // 默认大小
    };
};