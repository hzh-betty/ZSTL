#pragma once
#include <iostream>
#include <initializer_list>
#include <cassert>

namespace zstl
{
    // 双向迭代器，用于在 Deque 中前后遍历元素
    // T: 元素类型；Ptr: 指针类型；Ref: 引用类型；BufferSize: 每个缓冲区的大小
    template <typename T, typename Ptr, typename Ref, size_t BufferSize>
    struct DequeIterator
    {
    private:
        using pointer = T*;            // 元素指针类型
        using map_pointer = pointer*;  // 指向缓冲区指针的指针类型
        using Self = DequeIterator<T, Ptr, Ref, BufferSize>;

    public:
        // 默认构造，初始化所有指针为空
        DequeIterator()
            : first_(nullptr), last_(nullptr), cur_(nullptr), node_(nullptr)
        {
        }

        // 构造函数：传入新的缓冲区节点指针
        DequeIterator(map_pointer newnode)
        {
            set_node(newnode);   // 切换到该节点
            cur_ = first_;       // 当前指向缓冲区起始
        }

        // 切换到新的缓冲区节点，并更新 first_、last_
        void set_node(map_pointer newnode)
        {
            node_ = newnode;         // 保存中控数组中的节点位置
            first_ = *node_;         // 缓冲区首地址
            last_ = first_ + BufferSize; // 缓冲区尾后地址
        }

        // 解引用运算符，返回当前元素的引用
        Ref operator*() const
        {
            return *cur_;
        }

        // -> 运算符，返回当前元素指针
        Ptr operator->() const
        {
            return cur_;
        }

        // 计算两个迭代器之间的距离（元素个数差）
        int operator-(const Self &x) const
        {
            if (*this == x) return 0;
            // 跨缓冲区距离 = 完整缓冲区数 * BufferSize + 本区偏移 + 对方区剩余
            return int(BufferSize * (node_ - x.node_ - 1)) +
                   (cur_ - first_) +
                   (x.last_ - x.cur_);
        }

        // 前置++：移动到下一个元素，若到达缓冲区末尾则跳到下一个缓冲区
        Self &operator++()
        {
            ++cur_;
            if (cur_ == last_)  // 到达本缓冲区末尾
            {
                set_node(node_ + 1); // 切换到下一个缓冲区
                cur_ = first_;       // 指向下一区起始
            }
            return *this;
        }

        // 后置++：返回旧值，再++
        Self operator++(int)
        {
            Self tmp(*this);
            ++(*this);
            return tmp;
        }

        // 前置--：移动到前一个元素，若到达缓冲区起始则跳到前一个缓冲区末尾
        Self &operator--()
        {
            if (cur_ == first_)  // 在本区起始
            {
                set_node(node_ - 1); // 切换到前一个缓冲区
                cur_ = last_ - 1;     // 指向该缓冲区最后一个元素
            }
            else
            {
                --cur_;
            }
            return *this;
        }

        // 后置--：返回旧值，再--
        Self operator--(int)
        {
            Self tmp(*this);
            --(*this);
            return tmp;
        }

        // += 偏移 n 个位置：可能跨缓冲区
        Self &operator+=(int n)
        {
            // 计算相对于 first_ 的新偏移
            int offset = n + (cur_ - first_);
            if (offset >= 0 && offset < BufferSize)
            {
                // 仍在同一缓冲区内
                cur_ += n;
            }
            else
            {
                // 计算需要移动的缓冲区数（向下取整）
                int offset_node = offset > 0 ? offset / int(BufferSize)
                                              : (offset + 1) / int(BufferSize) - 1;
                set_node(node_ + offset_node);                      // 切换缓冲区
                cur_ = first_ + (offset - offset_node * BufferSize); // 定位新元素
            }
            return *this;
        }

        // + 运算符，生成临时并调用 +=
        Self operator+(int n)
        {
            Self tmp(*this);
            return tmp += n;
        }

        // -= 运算符，调用 +=(-n)
        Self &operator-=(int n)
        {
            return *this += (-n);
        }

        // - n 运算符
        Self operator-(int n)
        {
            Self tmp(*this);
            return tmp += (-n);
        }

        // 下标运算符，通过 +n 后解引用
        Ref operator[](int n)
        {
            return *(*this + n);
        }

        // 比较运算符，只比较 cur_ 或 node_
        bool operator!=(const Self &s) const { return cur_ != s.cur_; }
        bool operator==(const Self &s) const { return cur_ == s.cur_; }
        bool operator<(const Self &s) const
        {
            // 同一缓冲区比较 cur_
            return node_ == s.node_ ? cur_ < s.cur_ : node_ < s.node_;
        }
        bool operator>=(const Self &s) const { return !(*this < s); }
        bool operator<=(const Self &s) const { return *this < s || *this == s; }
        bool operator>(const Self &s) const { return !(*this <= s); }

        pointer first_;    // 当前缓冲区首地址
        pointer last_;     // 当前缓冲区尾后地址
        pointer cur_;      // 当前元素地址
        map_pointer node_; // 指向中控数组中缓冲区指针的位置
    };

    // Deque 容器实现
    template <typename T, size_t BufferSize = 25>
    class deque
    {
    private:
        using pointer = T*;
        using map_pointer = pointer*;

    public:
        using iterator = DequeIterator<T, T*, T&, BufferSize>;
        using const_iterator = DequeIterator<T, const T*, const T&, BufferSize>;

        // 返回首尾迭代器
        iterator begin() { return start_; }
        iterator end()   { return finish_; }
        const_iterator begin() const { return start_; }
        const_iterator end()   const { return finish_; }

        // 默认构造：创建空的 map 和一个缓冲区
        deque() { create_map(0); }

        // 列表初始化
        deque(std::initializer_list<T> it)
        {
            create_map(it.size());           // 根据元素数分配缓冲区
            iterator iter = start_;
            for (auto &e : it)
            {
                *iter = e;                   // 逐个拷贝元素
                ++iter;
            }
        }

        // 指定大小和初始值
        deque(size_t n, const T &val)
        {
            create_map(n);
            iterator iter = start_;
            while (iter != finish_)
            {
                *iter = val;                // 初始化每个位置
                ++iter;
            }
        }

        // 拷贝构造：从另一个 deque 逐元素 push_back
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

        // 赋值运算符：拷贝-交换习惯用法
        deque<T>& operator=(const deque &d)
        {
            if (this != &d)
            {
                deque<T> tmp(d);
                swap(tmp);
            }
            return *this;
        }

        // 移动构造：直接拿走对方资源，置空对方
        deque(deque &&d)
            : start_(d.start_), finish_(d.finish_), map_(d.map_), map_size_(d.map_size_)
        {
            d.start_ = d.finish_ = iterator();
            d.map_ = nullptr;
            d.map_size_ = 0;
        }

        // 移动赋值：释放自身资源，再抢占对方资源
        deque& operator=(deque &&d)
        {
            if (this != &d)
            {
                clear();
                delete[] map_;
                start_ = d.start_; finish_ = d.finish_;
                map_ = d.map_; map_size_ = d.map_size_;
                d.start_ = d.finish_ = iterator();
                d.map_ = nullptr; d.map_size_ = 0;
            }
            return *this;
        }

        // 原地构造新元素到末尾
        template <typename... Args>
        void emplace_back(Args&&... args)
        {
            insert(end(), T(std::forward<Args>(args)...));
        }

        // 访问首元素，空则断言
        T& front() { assert(!empty()); return *start_; }
        const T& front() const { assert(!empty()); return *start_; }
        // 访问尾元素
        T& back() { assert(!empty()); return *(finish_ - 1); }
        const T& back() const { assert(!empty()); iterator tmp = finish_; return *(tmp - 1); }

        // 在 pos 插入右值
        iterator insert(iterator pos, T&& val)
        {
            assert(pos >= begin() && pos <= end());
            size_t index = pos - start_;       // 插入位置的索引
            size_t sz = size();
            bool expanded = check_map_size();  // 检查并可能扩容
            iterator iter;

            if (index < sz / 2)  // 插入点在前半段，从前向后搬移
            {
                if (expanded) *(start_.node_ - 1) = new T[BufferSize]; // 新扩容缓冲区
                iter = start_ - 1;    // 在最前面腾出位置
                // 搬移 [start_, pos) 区间内的元素
                while (iter + 1 != pos)
                {
                    *iter = std::move(*(iter + 1));
                    ++iter;
                }
                --start_;  // 更新 start_
            }
            else                 // 插入点在后半段，从后向前搬移
            {
                if (expanded) *(finish_.node_ + 1) = new T[BufferSize];
                iter = finish_;
                while (iter != pos)
                {
                    *iter = std::move(*(iter - 1));
                    --iter;
                }
                ++finish_;
            }
            *iter = std::move(val);  // 放入新值
            return iter;
        }

        // 在 pos 插入左值，逻辑同上，仅拷贝而非移动
        iterator insert(iterator pos, const T& val)
        {
            assert(pos >= begin() && pos <= end());
            size_t index = pos - start_;
            size_t sz = size();
            bool expanded = check_map_size();
            iterator iter;
            if (index < sz / 2)
            {
                if (expanded) *(start_.node_ - 1) = new T[BufferSize];
                iter = start_ - 1;
                while (iter + 1 != pos)
                {
                    *iter = *(iter + 1);
                    ++iter;
                }
                --start_;
            }
            else
            {
                if (expanded) *(finish_.node_ + 1) = new T[BufferSize];
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

        // 删除 pos 位置元素，返回下一位置迭代器
        iterator erase(iterator pos)
        {
            assert(pos >= begin() && pos < end() && !empty());
            size_t index = pos - start_;
            size_t sz = size();
            iterator iter;

            if (index < sz / 2)  // 前半段，向前覆盖
            {
                iter = pos;
                while (iter != start_)
                {
                    *iter = *(iter - 1);
                    --iter;
                }
                ++start_;
            }
            else                 // 后半段，向后覆盖
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

        // push_back / push_front / pop_back / pop_front
        void push_back(const T& val) { insert(end(), val); }
        void push_front(const T& val) { insert(begin(), val); }
        void pop_back() { erase(end() - 1); }
        void pop_front() { erase(begin()); }

        // 调整大小：缩小则移动 finish_，扩大则 push_back
        void resize(size_t n, const T& val = T())
        {
            if (n < size()) finish_ = start_ + n;
            else while (size() < n) push_back(val);
        }

        // 下标访问
        T& operator[](size_t pos) { assert(pos < size()); return start_[pos]; }
        const T& operator[](size_t pos) const { assert(pos < size()); return start_[pos]; }

        // 大小与空判断
        size_t size() const { return finish_ - start_; }
        bool empty() const { return start_ == finish_; }

        // 清空所有元素，重置为单区状态
        void clear()
        {
            for (map_pointer node = start_.node_; node <= finish_.node_; ++node)
                delete[] *node;             // 释放每个缓冲区数组

            map_pointer center = map_ + map_size_ / 2;
            *center = new T[BufferSize];    // 新建单个缓冲区
            start_.set_node(center);
            start_.cur_ = start_.first_;
            finish_ = start_;
        }

        // 交换两者底层指针与状态
        void swap(deque<T>& d)
        {
            std::swap(start_, d.start_);
            std::swap(finish_, d.finish_);
            std::swap(map_, d.map_);
            std::swap(map_size_, d.map_size_);
        }

        // 析构：释放所有缓冲区与中控数组
        ~deque()
        {
            if (map_)
            {
                for (map_pointer node = start_.node_; node <= finish_.node_; ++node)
                    delete[] *node;
                delete[] map_;
            }
        }

    private:
        // 检查中控数组边界，触发扩容返回 true
        bool check_map_size()
        {
            // 如果 start_ 或 finish_ 已经达到 map 边缘，则需要扩容
            if (start_.cur_ == *map_ || finish_.cur_ == (*(map_ + map_size_ - 1) + BufferSize - 1))
            {
                expanse_capacity(map_size_);
                return true;
            }
            return false;
        }

        // 扩展中控数组及其缓冲区容量
        void expanse_capacity(size_t add_num)
        {
            size_t old_nodes = finish_.node_ - start_.node_ + 1;        // 旧节点数
            size_t new_nodes = old_nodes + add_num;                     // 扩展后节点数
            size_t new_map_sz = map_size_ + std::max(map_size_, add_num) + 2; // 新 map 大小
            map_pointer new_map = new pointer[new_map_sz];               // 分配新中控数组
            map_pointer new_start = new_map + (new_map_sz - new_nodes) / 2;
            map_pointer new_finish = new_start + old_nodes - 1;

            // 将旧缓冲区指针复制到新中控数组
            map_pointer dst = new_start;
            for (map_pointer src = start_.node_; src != finish_.node_ + 1; ++src)
                *dst++ = *src;

            delete[] map_;    // 释放旧中控数组
            map_ = new_map;
            map_size_ = new_map_sz;
            // 更新 start_ 和 finish_ 的 node、first、last
            start_.set_node(new_start);
            finish_.set_node(new_finish);
        }

        // 创建初始中控数组和缓冲区
        void create_map(size_t n)
        {
            // 计算需要的节点数与中控数组大小
            size_t nodes = n / BufferSize + 1;
            map_size_ = nodes < 8 ? 8 : nodes + 2;
            map_ = new pointer[map_size_];

            // 居中分配节点区间
            map_pointer nstart = map_ + (map_size_ - nodes) / 2;
            map_pointer nfinish = nstart + nodes - 1;
            for (map_pointer p = nstart; p <= nfinish; ++p)
                *p = new T[BufferSize];    // 为每个节点分配缓冲区数组

            // 设置 start_ / finish_
            start_.set_node(nstart);
            finish_.set_node(nfinish);
            start_.cur_ = start_.first_;
            finish_.cur_ = finish_.first_ + (n % BufferSize);
        }

    private:
        iterator start_;      // 起始迭代器
        iterator finish_;     // 末尾迭代器
        map_pointer map_;     // 中控数组指针
        size_t map_size_;     // 中控数组大小
    };
};
