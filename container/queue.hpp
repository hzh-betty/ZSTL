#pragma once
#include <iostream>
#include "deque.hpp"
#include "vector.hpp"
#include <functional>
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
    template <typename T, typename Container = vector<T>, typename Compare = std::less<T>>
    class priority_queue
    {
    public:
        priority_queue() = default;

        template <typename Inputinterpreator>
        priority_queue(Inputinterpreator first, Inputinterpreator last)
        {
            while (first != last)
            {
                con_.push_back(*first);
                ++first;
            }

            // 向下调整建堆
            for (int i = ((con_.size() - 1) - 1) / 2; i >= 0; i--)
            {
                adjust_down(i);
            }
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
            adjust_up(con_.size() - 1);
        }
        // 插入元素
        void push(const T &x)
        {
            con_.push_back(x);
            adjust_up(con_.size() - 1);
        }

        // 弹出一个元素
        void pop()
        {
            // 交换
            std::swap(con_[0], con_[con_.size() - 1]);
            con_.pop_back();

            // 向下调整
            adjust_down(0);
        }

        // 获取队尾元素
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
        // 向上调整
        void adjust_up(size_t child)
        {
            Compare com;
            size_t parent = (child - 1) / 2;
            while (child > 0)
            {
                if (com(con_[parent], con_[child]))
                {
                    std::swap(con_[child], con_[parent]);
                    child = parent;
                    parent = (child - 1) / 2;
                }
                else
                {
                    break;
                }
            }
        }

        // 向下调整
        void adjust_down(size_t parent)
        {
            int child = 2 * parent + 1;
            Compare com;
            while (child < con_.size())
            {
                // 找到最大孩子
                if (child + 1 < con_.size() && com(con_[child], con_[child + 1]))
                {
                    child++;
                }

                if (com(con_[parent], con_[child]))
                {
                    std::swap(con_[parent], con_[child]);
                    parent = child;
                    child = 2 * parent + 1;
                }
                else
                {
                    break;
                }
            }
        }

    private:
        Container con_;
    };
};