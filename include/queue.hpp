#include <iostream>
#include "deque.hpp"
namespace zstl
{
    template <typename T, typename Container = deque<T>>
    class queue
    {
    public:
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
        size_t size()
        {
            return con_.size();
        }

    private:
        Container con_;
    };
};