#pragma once
#include <iostream>
#include <cassert>
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
        using iterator = DequeIterator<T, T *, T &, BufferSize>;
        using const_iterator = DequeIterator<T, const T *, const T &, BufferSize>;
        iterator begin()
        {
            return start_;
        }

        iterator end()
        {
            return finish_;
        }

        const_iterator begin() const
        {
            return start_;
        }

        const_iterator end() const
        {
            return finish_;
        }

        void push_back(const T &val)
        {
            if (finish_.cur_ != finish_.last_ - 1)
            {
                *finish_ = val;
                ++finish_;
            }
            else
            {
                // 尾部增加缓冲区
                check_tail_map_size();
                *(finish_.node_ + 1) = new T[BufferSize];
                ++finish_;
                *finish_ = val;
            }
        }

        void push_front(const T &val)
        {
            if (start_.cur_ != start_.first_)
            {
                --start_;
                *start_ = val;
            }
            else
            {
                // 头部增加缓冲区
                check_tail_map_size();
                *(finish_.node_ - 1) = new T[BufferSize];
                --finish_;
                *finish_ = val;
            }
        }

        void pop_back()
        {
            assert(!empty());
            --finish_;
        }

        void pop_front()
        {
            assert(!empty());
            ++start_;
        }
        deque()
        {
            create_map(0);
        }

        deque(size_t n, const T &val)
        {
            create_map(n);
            map_pointer cur;
            for (cur = start_.node_; cur < finish_.node_; ++cur)
            {
                fill(*cur, (*cur).last_, val);
            }
            fill(*cur, (*cur).cur_, val);
        }

    private:
        void check_tail_map_size()
        {
            if (finish_ + 1 - *map_ >= map_size_)
            {
                expanse_capacity(2 * map_size_);
            }
        }

        void expanse_capacity(size_t add_num)
        {
            // 1. 计算新的中控数组大小
            size_t old_size = finish_.node_ - start_.node_ + 1;
            size_t new_size = old_size + add_num;
            size_t new_map_size = map_size_ + (map_size_ > add_num ? map_size_ : add_num) + 2;
            map_pointer new_map = new pointer[new_map_size];

            // 2. 计算新的迭代器区间
            map_pointer new_start = new_map + (new_map_size - new_size) / 2;
            map_pointer new_finish = new_start + old_size - 1;

            // 3. 赋值转移
            map_pointer tmp = new_start;
            for (iterator begin = start_; begin != finish_; ++begin)
            {
                *tmp = *begin;
                ++tmp;
            }

            // 4. 销毁原中控数组并更新
            map_ = new_map;
            map_size_ = new_map_size;
            start_.set_node(new_start);
            finish_.set_node(new_finish);
            delete[] map_;
        }

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

        template <typename Inputiterator, typename T>
        void fill(Inputiterator first, Inputiterator end, const T &val)
        {
            while (fistr != last)
            {
                *first = val;
                ++first;
            }
        }

    private:
        iterator start_;  // 开始元素
        iterator finish_; // 最后一个元素的下一个
        map_pointer map_; // 中控数组
        size_t map_size_; // 数组个数
    };
};
