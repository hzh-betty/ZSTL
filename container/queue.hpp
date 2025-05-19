#pragma once
#include "deque.hpp"
#include "vector.hpp"
#include "../functor/functional.hpp"
#include "../algorithm/heap_algo.hpp"
namespace zstl
{
    template <typename T, typename Container = deque<T>>
    class queue
    {
    public:
        queue() = default;
        ~queue() = default;

        // 移动构造函数
        queue(queue &&other)
            : con_(std::move(other.con_))
        {
        }

        // 移动赋值运算符
        queue &operator=(queue &&other)
        {
            if (this != &other)
            {
                con_ = std::move(other.con_);
            }
            return *this;
        }

        // 在 queue 类中添加以下代码
        template <typename... Args>
        void emplace(Args &&...args)
        {
            con_.emplace_back(std::forward<Args>(args)...);
        }

        // 插入元素
        void push(const T &x)
        {
            con_.push_back(x);
        }

        // 弹出一个元素
        void pop()
        {
            con_.pop_front();
        }

        // 获取队头元素
        T &front()
        {
            return con_.front();
        }

        const T &front() const
        {
            return con_.front();
        }

        // 获取队尾元素
        T &back()
        {
            return con_.back();
        }

        const T &back() const
        {
            return con_.back();
        }

        // 判断是否为空
        bool empty() const
        {
            return con_.empty();
        }

        // 获取大小
        size_t size() const
        {
            return con_.size();
        }

    private:
        Container con_;
    };

    /*优先级队列*/
    template <typename T, typename Container = vector<T>, typename Compare = zstl::less<T>>
    class priority_queue
    {
    public:
        priority_queue() = default;

        template <typename InputIt>
        priority_queue(InputIt first, InputIt last)
            : con_(first, last)
        {
            zstl::make_heap(con_.begin(), con_.end(), Compare());
        }

        // 移动构造函数
        priority_queue(priority_queue &&other) noexcept
            : con_(std::move(other.con_))
        {
        }

        // 移动赋值运算符
        priority_queue &operator=(priority_queue &&other) noexcept
        {
            if (this != &other)
            {
                con_ = std::move(other.con_);
            }
            return *this;
        }

        template <typename... Args>
        void emplace(Args &&...args)
        {
            con_.emplace_back(std::forward<Args>(args)...);
            zstl::push_heap(con_.begin(), con_.end(), Compare());
        }

        // 插入元素
        void push(const T &x)
        {
            con_.push_back(x);
            zstl::push_heap(con_.begin(), con_.end(), Compare());
        }

        // 弹出一个元素
        void pop()
        {
            zstl::pop_heap(con_.begin(), con_.end(), Compare());
            con_.pop_back();
        }

        // 获取队首元素
        T &top()
        {
            return con_.front();
        }

        const T &top() const
        {
            return con_.front();
        }

        // 判断是否为空
        bool empty() const
        {
            return con_.empty();
        }

        // 获取大小
        size_t size() const
        {
            return con_.size();
        }

    private:
        Container con_;
    };
};