#pragma once
#include <iostream>
#include "deque.hpp"
namespace zstl
{
    template <typename T, typename Container = deque<T>>
    class stack
    {
    public:
        stack() = default;
        ~stack() = default;
        // 移动构造函数
        stack(stack &&other) 
            : con_(std::move(other.con_))
        {
        }

        // 移动赋值运算符
        stack &operator=(stack &&other) 
        {
            if (this != &other)
            {
                con_ = std::move(other.con_);
            }
            return *this;
        }

        // emplace 接口
        template <typename... Args>
        void emplace(Args &&...args)
        {
            con_.emplace_back(std::forward<Args>(args)...);
        }

        // 向栈顶插入元素
        void push(const T &x)
        {
            con_.push_back(x);
        }

        // 弹出栈顶元素
        void pop()
        {
            con_.pop_back();
        }

        // 获取栈顶元素
        T &top()
        {
            return con_.back();
        }

        const T &top() const
        {
            return con_.back();
        }

        // 判断是否为空
        bool empty() const
        {
            return con_.empty();
        }

        // 获取大小
        size_t size()
        {
            return con_.size();
        }

    private:
        Container con_;
    };
};