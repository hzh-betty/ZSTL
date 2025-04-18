#pragma once
#include <iostream>
#include "deque.hpp"
namespace zstl
{
    template <typename T, typename Container = deque<T>>
    class stack
    {
    public:

        // 向栈顶插入元素
        void push(const T&x)
        {
            con_.push_back(x);
        }

        // 弹出栈顶元素
        void pop()
        {
            con_.pop_back();
        }

        // 获取栈顶元素
        T&top()
        {
            return con_.back();
        }

        const T&top()const
        {
            return con_.back();
        }

        // 判断是否为空
        bool empty()const
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