#pragma once
#include <iostream>
#include <cstring>
namespace zstl
{
    class string
    {
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

        // 析构函数
        ~string()
        {
            delete str_;
            str_ = nullptr;
            size_ = capacity_ = 0;
        }

    private:
        void swap(string &s)
        {
            std::swap(size_, s.size_);
            std::swap(capacity_, s.capacity_);
            std::swap(str_, s.str_);
        }
    private:
        size_t size_;     // 当前大小
        size_t capacity_; // 当前容量
        char *str_;       // 存储的字符串
    };
};