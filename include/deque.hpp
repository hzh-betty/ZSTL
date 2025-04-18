#pragma once
#include <iostream>
#include <cassert>

namespace zstl
{
    // 双向迭代器，用于遍历 Deque
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

        // 设置新的缓冲区节点
        DequeIterator(map_pointer newnode)
        {
            set_node(newnode);
            cur_ = first_;
        }

        // 切换到下一个缓冲区
        void set_node(map_pointer newnode)
        {
            node_ = newnode;
            first_ = *node_;
            last_ = first_ + BufferSize;
        }

        // 重载解引用运算符，返回当前指向的元素
        Ref operator*()
        {
            return *cur_;
        }

        // 重载箭头运算符，返回当前指向的元素指针
        Ptr operator->()
        {
            return cur_;
        }

        // 计算两个迭代器之间的距离
        int operator-(const Self &x)const
        {
            return int(BufferSize * (node_ - x.node_ - 1)) +
                   (cur_ - first_) +
                   (x.last_ - x.cur_);
        }

        // 前置递增运算符，指向下一个元素
        Self &operator++()
        {
            ++cur_;
            if (cur_ == last_) // 换到下一个缓冲区
            {
                set_node(node_ + 1);
                cur_ = first_;
            }
            return *this;
        }

        // 后置递增运算符
        Self operator++(int)
        {
            Self tmp(*this);
            ++(*this);
            return tmp;
        }

        // 前置递减运算符，指向前一个元素
        Self &operator--()
        {
            if (cur_ == first_) // 换到前一个缓冲区
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

        // 后置递减运算符
        Self operator--(int)
        {
            Self tmp(*this);
            --(*this);
            return tmp;
        }

        // 偏移迭代器
        Self &operator+=(int n)
        {
            // 1. 计算偏移量
            int offset = n + (cur_ - first_);
            if (offset >= 0 && offset < BufferSize)
            {
                cur_ += n;
            }
            // 2. 更新
            else
            {
                int offset_node = offset > 0 ? offset / (int)BufferSize : (offset + 1) / (int)BufferSize - 1;
                set_node(node_ + offset_node);
                cur_ = first_ + offset - offset_node * BufferSize;
            }
            return *this;
        }

        // 右移运算符
        Self operator+(int n)
        {
            Self tmp(*this);
            return tmp += n;
        }

        // 左移运算符
        Self &operator-=(int n)
        {
            return *this += (-n);
        }

        // 减法运算符
        Self operator-(int n)
        {
            Self tmp(*this);
            return tmp += (-n);
        }

        // 重载下标运算符
        Ref operator[](int n)
        {
            return *(*this + n);
        }

        // 不等于运算符
        bool operator!=(const Self &s) const
        {
            return cur_ != s.cur_;
        }

        // 等于运算符
        bool operator==(const Self &s) const
        {
            return cur_ == s.cur_;
        }

        // 小于运算符
        bool operator<(const Self &s) const
        {
            return node_ == s.node_ ? cur_ < s.cur_ : node_ < s.node_;
        }
        
        // 大于等于运算符
        bool operator>=(const Self &s) const
        {
            return !(*this < s);
        }
        

        // 小于等于运算符
        bool operator<=(const Self &s) const
        {
            return *this < s || *this == s;
        }

        // 大于运算符
        bool operator>(const Self &s) const
        {
            return !(*this <= s);
        }

        pointer first_;    // 当前缓冲区的起始位置
        pointer last_;     // 当前缓冲区的末尾位置
        pointer cur_;      // 当前迭代器所在位置
        map_pointer node_; // 指向中控数组中当前缓冲区的指针
    };

    // Deque 类实现
    template <typename T, size_t BufferSize = 25>
    class deque
    {
    private:
        using pointer = T *;
        using map_pointer = pointer *;

    public:
        using iterator = DequeIterator<T, T *, T &, BufferSize>;
        using const_iterator = DequeIterator<T, const T *, const T &, BufferSize>;

        // 获取首尾迭代器
        iterator begin() { return start_; }
        iterator end() { return finish_; }
        const_iterator begin() const { return start_; }
        const_iterator end() const { return finish_; }

        // 默认构造函数
        deque()
        {
            create_map(0);
        }

        // 用指定值初始化 deque
        deque(size_t n, const T &val)
        {
            create_map(n);
            iterator iter = start_;
            while (iter != finish_)
            {
                *iter = val;
                ++iter;
            }
        }

        // 拷贝构造函数
        deque(const deque &d)
        {
            create_map(0);
            iterator iter = d.start_;
            while (iter != d.finish_)
            {
                push_back(*iter);
                ++iter;
            }
        }

        // 赋值运算符重载
        deque<T> &operator=(deque d)
        {
            swap(d);
            return *this;
        }

        // 返回 deque 的前端元素
        T &front()
        {
            assert(!empty());
            return *start_;
        }

        // 返回 deque 的后端元素
        T &back()
        {
            assert(!empty());
            return *(finish_ - 1);
        }

        // 返回常量引用前端元素
        const T &front() const
        {
            assert(!empty());
            return *start_;
        }

        // 返回常量引用后端元素
        const T &back() const
        {
            assert(!empty());
            return *(finish_ - 1);
        }

        // 在 pos 位置插入元素
        iterator insert(iterator pos, const T &val)
        {
            assert(pos >= begin() && pos <= end());
            size_t index = pos - start_;
            size_t size = this->size();
            bool ret = check_map_size();
            iterator iter;

            // 往前移动
            if (index < size / 2)
            {
                if (ret == true)
                {
                    *(start_.node_ - 1) = new T[BufferSize];
                }
                iter = start_ - 1;
                while (iter + 1 != pos)
                {
                    *iter = *(iter + 1);
                    ++iter;
                }
                --start_;
            }
            // 往后移动
            else
            {
                if (ret == true)
                {
                    *(finish_.node_ + 1) = new T[BufferSize];
                }
                iter = finish_;
                while (iter != pos)
                {
                    *iter = *(iter - 1);
                    --iter;
                }
                ++finish_;
            }
            *iter = val;
            return iter;
        }

        // 删除指定位置的元素
        iterator erase(iterator pos)
        {
            assert(pos >= begin() && pos < end());
            assert(!empty());
            size_t index = pos - start_;
            size_t size = this->size();
            iterator iter;

            // 从前往后覆盖
            if (index < size / 2)
            {
                iter = pos;
                while (iter != start_)
                {
                    *iter = *(iter - 1);
                    --iter;
                }
                ++start_;
            }
            // 从后往前覆盖
            else
            {
                iter = pos + 1;
                while (iter != finish_)
                {
                    *(iter - 1) = *iter;
                    ++iter;
                }
                --finish_;
            }

            return start_ + index;
        }

        // 向后插入元素
        void push_back(const T &val)
        {
            insert(end(), val);
        }

        // 向前插入元素
        void push_front(const T &val)
        {
            insert(begin(), val);
        }

        // 删除最后一个元素
        void pop_back()
        {
            erase(end() - 1);
        }

        // 删除第一个元素
        void pop_front()
        {
            erase(begin());
        }

        // 调整大小
        void resize(size_t n, const T &val = T())
        {
            if (n < size())
            {
                finish_ = start_ + n;
            }
            else
            {
                while (finish_ != start_ + n)
                {
                    push_back(val);
                }
            }
        }

        // 重载下标运算符
        T &operator[](size_t pos)
        {
            assert(pos < size());
            return start_[pos];
        }

        // 常量版本的下标运算符
        const T &operator[](size_t pos) const
        {
            assert(pos < size());
            return start_[pos];
        }

        // 获取 deque 的大小
        int size() const
        {
            return finish_ - start_;
        }

        // 判断 deque 是否为空
        bool empty() const
        {
            return start_ == finish_;
        }

        // 清空 deque
        void clear()
        {
            for (map_pointer node = start_.node_; node <= finish_.node_; ++node)
            {
                delete[] *node;
            }

            map_pointer node = map_ + map_size_ / 2;
            *node = new T[BufferSize];
            start_.set_node(node);
            start_.cur_ = start_.first_;
            finish_ = start_;
        }

        // 交换两个 deque
        void swap(deque<T> &d)
        {
            std::swap(start_, d.start_);
            std::swap(finish_, d.finish_);
            std::swap(map_, d.map_);
            std::swap(map_size_, d.map_size_);
        }

        // 析构函数，清理所有资源
        ~deque()
        {
            for (map_pointer node = start_.node_; node <= finish_.node_; ++node)
            {
                delete[] *node;
            }

            delete[] map_;
            start_ = finish_ = iterator();
            map_size_ = 0;
        }

    private:
        // 检查缓冲区是否足够，若不足则扩展
        bool check_map_size()
        {
            if (start_.cur_ == *map_ || finish_.cur_ == (*(map_ + map_size_ - 1) + BufferSize - 1))
            {
                expanse_capacity(map_size_);
                return true;
            }
            return false;
        }

        // 扩展缓冲区容量
        void expanse_capacity(size_t add_num)
        {
            // 1. 计算新的容量
            size_t old_size = finish_.node_ - start_.node_ + 1;
            size_t new_size = old_size + add_num;
            size_t new_map_size = map_size_ + std::max(map_size_, add_num) + 2;
            map_pointer new_map = new pointer[new_map_size];
            map_pointer new_start = new_map + (new_map_size - new_size) / 2;
            map_pointer new_finish = new_start + old_size - 1;

            // 2. 赋值转移
            map_pointer tmp = new_start;
            for (map_pointer begin = start_.node_; begin != finish_.node_ + 1; ++begin)
            {
                *tmp = *begin;
                ++tmp;
            }

            // 3. 销毁原中控数组
            delete[] map_;
            map_ = new_map;
            map_size_ = new_map_size;
            start_.set_node(new_start);
            finish_.set_node(new_finish);
        }

        // 创建缓冲区并初始化
        void create_map(size_t n)
        {
            // 1. 计算中控数组的大小
            size_t node_num = n / BufferSize + 1;
            map_size_ = node_num < 8 ? 8 : node_num + 2;
            map_ = new pointer[map_size_];

            // 2. 分配起始位置与结束位置
            map_pointer nstart = map_ + (map_size_ - node_num) / 2;
            map_pointer nfinish = nstart + node_num - 1;
            for (map_pointer tmp = nstart; tmp <= nfinish; ++tmp)
            {
                *tmp = new T[BufferSize];
            }

            // 3. 填写数据
            start_.set_node(nstart);
            finish_.set_node(nfinish);
            start_.cur_ = start_.first_;
            finish_.cur_ = finish_.first_ + (n % BufferSize);
        }

    private:
        iterator start_;  // 起始迭代器
        iterator finish_; // 末尾迭代器
        map_pointer map_; // 控制缓冲区的数组
        size_t map_size_; // 控制缓冲区的大小
    };
};
