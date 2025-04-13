#pragma once
#include <iostream>
namespace zstl
{
    template <typename T, typename Ptr, typename Ref, size_t BufferSize>
    struct DequeIterator
    {
    private:
        using pointer = T *;
        using map_pointer = pointer *;
        using Self = DequeIterator<T, Ptr, Ref, BufferSize>;

    public:
        DequeIterator()
            : first_(nullptr), last_(nullptr), cur_(nullptr), node_(nullptr)
        {
        }

        // 调到下一个缓冲区
        void set_node(map_pointer newnode)
        {
            node_ = newnode;
            first_ = *node_;
            last_ = first_ + BufferSize;
        }

        Ref operator*()
        {
            return *cur_;
        }

        Ptr operator->()
        {
            return cur_;
        }

        // 获取元素个数
        int operator-(const Self &x)
        {
            return int(BufferSize * (node_ - x.node_ - 1)) +
                   (cur_ - first_) +
                   (x.last_ - x.cur_);
        }

        Self &operator++()
        {
            ++cur_;
            if (cur_ == last_)
            {
                set_node(node_ + 1);
                cur_ = first_;
            }
            return *this;
        }

        Self operator++(int)
        {
            Self tmp(*this);
            ++(*this);
            return tmp;
        }

        Self &operator--()
        {
            if (cur_ == first_)
            {
                set_node(node_ - 1);
                cur_ = last_ - 1;
            }
            else
            {
                --cur_;
            }
            return *this;
        }

        Self operator--(int)
        {
            Self tmp(*this);
            --(*this) return tmp;
        }

        Self &operator+=(int n)
        {
            // 1. 计算偏移量
            int offset = n + (cur_ - first_);
            if (offset >= 0 && offset < BufferSize)
            {
                cur_ += n;
            }
            // 2. 不在该缓冲区
            else
            {
                int offset_node = offset > 0 ? offset / (int)BufferSize : (offset + 1) / (int)BufferSize - 1;
                setnode(node_ + offset_node);
                cur_ = first_ + offset - offset_node * BufferSize;
            }
            return *this;
        }

        Self operator+(int n)
        {
            Self tmp(*this);
            return tmp += n;
        }

        Self &operator-=(int n)
        {
            return *this += (-n);
        }

        Self operator-(int n)
        {
            Self tmp(*this);
            return tmp += (-n);
        }

        Ref operator[](int n)
        {
            return *(*this + n);
        }

        bool operator!=(const Self &s) const
        {
            return cur_ != s.cur_;
        }

        bool operator==(const Self &s) const
        {
            return cur_ == s.cur_;
        }

        bool operator<(const Self &s) const
        {
            return node_ == s.node_ ? cur_ < s.cur_ : node_ < s.node_;
        }

        bool operator<=(const Self &s) const
        {
            return *this < s || *this == s;
        }

        bool operator>(const Self &s) const
        {
            return !(*this <= s);
        }

        pointer first_;    // 指向缓冲区开始位置
        pointer last_;     // 指向缓冲区结束位置
        pointer cur_;      // 指向缓冲区现在位置
        map_pointer node_; // 指向中控数组
    };

    template <typename T, size_t BufferSize = 25>
    class deque
    {
    private:
        using pointer = T *;
        using map_pointer = pointer *;

    public:
        using iteraotr = DequeIterator<T, T *, T &, BufferSize>;
        using const_iteraotr = DequeIterator<T, const T *, const T &, BufferSize>;

        deque()
        {
        }

    private:
        void create_map(size_t n)
        {
            // 1. 判断开辟多少个中数组，并在首尾两侧预分配空间
            size_t node_num = n / BufferSize + 1;
            map_size_ = node_num < 8 ? 8 : node_num + 2;
            map_ = new pointer[map_size_];

            // 2. 计算[nstart,nfinsh]
            map_pointer nstart = map_ + (map_size_ - (n / BufferSize)) / 2; // 保证中控数组头空出一个
            map_pointer nfinsh = nstart + node_num - 1;

            // 3. 初始化[nstart,nfinsh]
            map_pointer tmp = nstart;
            while (tmp != nfinsh)
            {
                *tmp = new T[BufferSize];
                ++tmp;
            }

            // 4. 初始化迭代器
            start_.setnode(nstart);
            finish_.set_node(nfinsh);
            start.cur_ = start_.first_;
            finish_.cur = finish_.first_ + (n % BufferSize);
        }

    private:
        iteraotr start_;  // 开始元素
        iteraotr finish_;    // 最后一个元素的下一个
        map_pointer map_; // 中控数组
        size_t map_size_; // 数组个数
    };
};
