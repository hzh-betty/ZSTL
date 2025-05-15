#pragma once
#include <iostream>
#include <initializer_list>
#include <cassert>
#include "../iterator/reverse_iterator.hpp"
#include "../allocator/alloc.hpp"
#include "../allocator/memory.hpp"

namespace zstl
{
    inline constexpr size_t BufferSize = 25;
    //--------------------------------------------------------------------------------
    // deque 迭代器模板
    // T: 元素类型；Ptr: 指针类型；Ref: 引用类型
    // 支持随机访问，包括跨缓冲区跳转
    //--------------------------------------------------------------------------------
    template <typename T, typename Ptr, typename Ref>
    struct DequeIterator
    {
    private:
        using value_pointer = T *; // 元素指针类型
        using map_pointer = T **;  // 指向缓冲区指针的指针类型
        using Self = DequeIterator<T, Ptr, Ref>;

    public:
        // 迭代器萃取必需的五种类型
        using iterator_category = random_access_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = Ptr;
        using reference = Ref;

    public:
        // 默认构造，初始化所有指针为空
        DequeIterator()
            : first_(nullptr), last_(nullptr), cur_(nullptr), node_(nullptr)
        {
        }

        // 构造函数：传入新的缓冲区节点指针
        DequeIterator(map_pointer newnode)
        {
            set_node(newnode); // 切换到该节点
            cur_ = first_;     // 当前指向缓冲区起始
        }

        // 切换到新的缓冲区节点，并更新 first_、last_
        void set_node(map_pointer newnode)
        {
            node_ = newnode;             // 保存中控数组中的节点位置
            first_ = *node_;             // 缓冲区首地址
            last_ = first_ + BufferSize; // 缓冲区尾后地址
        }

        // 解引用运算符，返回当前元素的引用
        Ref operator*() const { return *cur_; }

        // -> 运算符，返回当前元素指针
        Ptr operator->() const { return cur_; }

        // 计算两个迭代器之间的距离（元素个数差）
        difference_type operator-(const Self &x) const
        {
            if (node_ == x.node_)
                return cur_ - x.cur_;
            return BufferSize * (node_ - x.node_ - 1) + (cur_ - first_) + (x.last_ - x.cur_);
        }

        // 前置++：移动到下一个元素，若到达缓冲区末尾则跳到下一个缓冲区
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

        // 后置--：返回旧值，再--
        Self operator--(int)
        {
            Self tmp(*this);
            --(*this);
            return tmp;
        }

        // += 偏移 n 个位置：可能跨缓冲区
        Self &operator+=(difference_type n)
        {
            difference_type offset = n + (cur_ - first_);
            if (offset >= 0 && offset < difference_type(BufferSize))
            {
                cur_ += n;
            }
            else
            {
                difference_type node_offset =
                    offset > 0 ? offset / BufferSize
                               : (offset + 1) / BufferSize - 1;
                set_node(node_ + node_offset);
                cur_ = first_ + (offset - node_offset * BufferSize);
            }
            return *this;
        }

        // + 运算符，生成临时并调用 +=
        Self operator+(difference_type n) const
        {
            Self tmp(*this);
            return tmp += n;
        }

        // -= 运算符，调用 +=(-n)
        Self &operator-=(difference_type n)
        {
            return *this += -n;
        }

        // - n 运算符
        Self operator-(difference_type n) const
        {
            Self tmp(*this);
            return tmp += -n;
        }

        // 下标运算符，通过 +n 后解引用
        Ref operator[](difference_type n) const
        {
            return *(*this + n);
        }

        // 比较运算符，只比较 node_ 和 cur_
        bool operator==(const Self &s) const { return cur_ == s.cur_; }
        bool operator!=(const Self &s) const { return !(*this == s); }
        bool operator<(const Self &s) const
        {
            return node_ == s.node_ ? cur_ < s.cur_ : node_ < s.node_;
        }
        bool operator>=(const Self &s) const { return !(*this < s); }
        bool operator<=(const Self &s) const { return *this < s || *this == s; }
        bool operator>(const Self &s) const { return !(*this <= s); }

        value_pointer first_; // 当前缓冲区首地址
        value_pointer last_;  // 当前缓冲区尾后地址
        value_pointer cur_;   // 当前元素地址
        map_pointer node_;    // 指向中控数组中缓冲区指针的位置
    };

    //--------------------------------------------------------------------------------
    // deque 容器，实现双端队列，支持随机访问
    // 包含自定义分配器支持
    //--------------------------------------------------------------------------------
    template <typename T, typename Alloc = alloc<T>>
    class deque
    {
    private:
        using buffer_type = T *;
        using map_pointer = buffer_type *;

    public:
        // 类型重定义
        using value_type = T;
        using allocator_type = Alloc;
        using traits_allocator = allocator_traits<allocator_type>;
        using pointer = typename traits_allocator::pointer;
        using const_pointer = typename traits_allocator::const_pointer;
        using reference = T &;
        using const_reference = const T &;
        using size_type = size_t;
        using map_allocator_type = typename traits_allocator::template rebind_alloc<buffer_type>;
        using map_traits_alloc = allocator_traits<map_allocator_type>;

    public:
        using iterator = DequeIterator<T, pointer, reference>;
        using const_iterator = DequeIterator<T, const_pointer, const_reference>;
        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<const_iterator>;
        using difference_type = typename iterator_traits<iterator>::difference_type;

        // 迭代器接口
        iterator begin() noexcept { return start_; }
        iterator end() noexcept { return finish_; }
        const_iterator begin() const noexcept { return start_; }
        const_iterator end() const noexcept { return finish_; }
        reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    public:
        // 默认构造：创建至少一个缓冲区
        deque(const allocator_type &alloc = allocator_type())
            : alloc_(alloc), map_alloc_(alloc_), map_(nullptr), map_size_(0)
        {
            create_map(0);
        }

        // 列表初始化
        deque(std::initializer_list<T> il, const allocator_type &alloc = allocator_type())
            : alloc_(alloc), map_alloc_(alloc_)
        {
            create_map(il.size());
            iterator tmp = start_;
            for (auto &v : il)
            {
                traits_allocator::construct(alloc_, tmp.cur_, v);
                ++tmp;
            }
        }

        // 指定大小和初始值
        deque(size_type n, const value_type &val, const allocator_type &alloc = allocator_type())
            : alloc_(alloc), map_alloc_(alloc_)
        {
            create_map(n);
            for (auto it = start_; it != finish_; ++it)
            {
                traits_allocator::construct(alloc_, it.cur_, val);
            }
        }

        // 拷贝构造：允许有状态分配器策略
        deque(const deque &other)
            : alloc_(other.alloc_), map_alloc_(alloc_)
        {
            create_map(other.size());
            std::copy(other.begin(), other.end(), start_);
        }

        // 移动构造：noexcept
        deque(deque &&other) noexcept
            : alloc_(std::move(other.alloc_)), map_alloc_(alloc_),
              start_(other.start_), finish_(other.finish_), map_(other.map_), map_size_(other.map_size_)
        {
            other.map_ = nullptr;
            other.start_ = other.finish_ = iterator();
            other.map_size_ = 0;
        }

        // 赋值运算符：拷贝-交换与移动赋值(noexcept)
        deque &operator=(const deque &other)
        {
            if (this != &other)
            {
                deque tmp(other);
                swap(tmp);
            }
            return *this;
        }
        deque &operator=(deque &&other) noexcept
        {
            if (this != &other)
            {
                clear();
                map_traits_alloc::deallocate(map_alloc_, map_, map_size_);
                alloc_ = std::move(other.alloc_);
                map_alloc_ = map_allocator_type(alloc_);
                start_ = other.start_;
                finish_ = other.finish_;
                map_ = other.map_;
                map_size_ = other.map_size_;
                other.map_ = nullptr;
                other.start_ = other.finish_ = iterator();
                other.map_size_ = 0;
            }
            return *this;
        }

        // 析构：释放缓冲区与 map
        ~deque()
        {
            if (map_)
            {
                clear();
                map_traits_alloc::deallocate(map_alloc_, map_, map_size_);
            }
        }

        // 容量与访问
        bool empty() const noexcept { return start_ == finish_; }
        size_type size() const noexcept { return finish_ - start_; }
        reference front()
        {
            assert(!empty());
            return *start_;
        }
        const_reference front() const
        {
            assert(!empty());
            return *start_;
        }
        reference back()
        {
            assert(!empty());
            return *(finish_ - 1);
        }
        const_reference back() const
        {
            assert(!empty());
            return *(finish_ - 1);
        }

        // 下标访问
        reference operator[](size_type pos)
        {
            assert(pos < size());
            return start_[pos];
        }
        const_reference operator[](size_type pos) const
        {
            assert(pos < size());
            return start_[pos];
        }

        /** 在前端原地构造 */
        template <typename... Args>
        void emplace_front(Args &&...args)
        {
            bool expanded = check_map_size(); // 检查并扩展中控数组
            if (start_.cur_ == start_.first_)
            {
                // 扩展后分配新缓冲区
                *(start_.node_ - 1) = traits_allocator::allocate(alloc_, BufferSize);
            }
            --start_;
            traits_allocator::construct(alloc_, start_.cur_, std::forward<Args>(args)...);
        }

        // 原地构造新元素到末尾
        template <typename... Args>
        void emplace_back(Args &&...args)
        {
            // 直接在当前位置构造对象
            traits_allocator::construct(alloc_, finish_.cur_, std::forward<Args>(args)...);
            bool expanded = check_map_size(); // 检查并扩展中控数组
            if (finish_.cur_ == finish_.last_ - 1)
            {
                // 扩展后分配新缓冲区
                *(finish_.node_ + 1) = traits_allocator::allocate(alloc_, BufferSize);
            }

            ++finish_;
        }

        // push_back / push_front / pop_back / pop_front
        void push_back(const value_type &val) { emplace_back(val); }
        void push_front(const value_type &val) { emplace_front(val); }
        void pop_back() { erase(end() - 1); }
        void pop_front() { erase(begin()); }

        // 在 pos 插入左值，逻辑同上，仅拷贝而非移动
        iterator insert(iterator pos, const value_type &val)
        {
            size_type index = pos - start_;
            size_type sz = size();
            bool expanded = check_map_size();
            iterator iter;
            if (index < sz / 2)
            {
                if (start_.cur_ == start_.first_)
                    *(start_.node_ - 1) = traits_allocator::allocate(alloc_, BufferSize);
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
                if (finish_.cur_ == finish_.last_ - 1)
                    *(finish_.node_ + 1) = traits_allocator::allocate(alloc_, BufferSize);

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
            size_type index = pos - start_;
            size_type sz = size();
            iterator iter;

            if (index < sz / 2) // 前半段，向前覆盖
            {
                iter = pos;
                while (iter != start_)
                {
                    *iter = *(iter - 1);
                    --iter;
                }
                ++start_;
            }
            else // 后半段，向后覆盖
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
        // 调整大小：缩小则移动 finish_，扩大则 push_back
        void resize(size_t n, const value_type &val = T())
        {
            if (n < size())
            {
                while (size() > n)
                    pop_back();
            }
            else
            {
                while (size() < n)
                    push_back(val);
            }
        }

        void clear()
        {
            traits_allocator::destroy_range(alloc_, start_, finish_);
            for (map_pointer p = start_.node_; p <= finish_.node_; ++p)
            {
                traits_allocator::deallocate(alloc_, *p, BufferSize);
            }
            create_map(0);
        }

        void swap(deque &d) noexcept
        {
            std::swap(start_, d.start_);
            std::swap(finish_, d.finish_);
            std::swap(map_, d.map_);
            std::swap(map_size_, d.map_size_);
            std::swap(alloc_, d.alloc_);
            std::swap(map_alloc_, d.map_alloc_);
        }

    private:
        // 创建中控数组及缓冲区
        void create_map(size_type n)
        {
            // 计算需要的节点数与中控数组大小
            size_type nodes = n / BufferSize + 1;
            map_size_ = nodes < 8 ? 8 : nodes + 2;
            map_ = map_traits_alloc::allocate(map_alloc_, map_size_);

            // 居中分配节点区间
            map_pointer nstart = map_ + (map_size_ - nodes) / 2;
            map_pointer nfinish = nstart + nodes - 1;
            for (map_pointer p = nstart; p <= nfinish; ++p)
            {
                *p = traits_allocator::allocate(alloc_, BufferSize); // 为每个节点分配缓冲区数组
            }

            // 设置 start_ / finish_
            start_.set_node(nstart);
            finish_.set_node(nfinish);
            start_.cur_ = start_.first_;
            finish_.cur_ = finish_.first_ + (n % BufferSize);
        }

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
        void expanse_capacity(size_type add_num)
        {
            size_type old_nodes = finish_.node_ - start_.node_ + 1;                   // 旧节点数
            size_type new_nodes = old_nodes + add_num;                                // 扩展后节点数
            size_type new_map_sz = map_size_ + std::max(map_size_, add_num) + 2;      // 新 map 大小
            map_pointer new_map = map_traits_alloc::allocate(map_alloc_, new_map_sz); // 分配新中控数组
            map_pointer new_start = new_map + (new_map_sz - new_nodes) / 2;
            map_pointer new_finish = new_start + old_nodes - 1;

            // 将旧缓冲区指针复制到新中控数组
            map_pointer dst = new_start;
            for (map_pointer src = start_.node_; src != finish_.node_ + 1; ++src)
                *dst++ = *src;

            map_traits_alloc::deallocate(map_alloc_, map_, map_size_);
            map_ = new_map;
            map_size_ = new_map_sz;
            // 更新 start_ 和 finish_ 的 node、first、last
            start_.set_node(new_start);
            finish_.set_node(new_finish);
        }

    private:
        allocator_type alloc_;         // 用户分配器
        map_allocator_type map_alloc_; // map 数组分配器
        iterator start_;               // 起始迭代器
        iterator finish_;              // 结束迭代器
        map_pointer map_;              // 中控数组指针
        size_type map_size_;           // 中控数组大小
    };
}