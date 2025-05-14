#pragma once
#include <iostream>
#include <cassert>
#include <memory>
#include "../iterator/reverse_iterator.hpp"
#include "../allocator/alloc.hpp"
#include "../allocator/memory.hpp"

namespace zstl
{
    // 前置声明：链表节点基础和具体节点模板
    template <typename T>
    struct ListNodeBase;
    template <typename T>
    struct ListNode;

    // 节点特性萃取：简化指针类型定义
    template <typename T>
    struct node_traits
    {
        using base_ptr = ListNodeBase<T> *; // 指向不含数据的基节点
        using node_ptr = ListNode<T> *;     // 指向含数据的节点
    };

    /* 链表基节点（不包含数据）*/
    template <typename T>
    struct ListNodeBase
    {
        using base_ptr = typename node_traits<T>::base_ptr;
        using node_ptr = typename node_traits<T>::node_ptr;

        base_ptr prev_; // 前驱指针
        base_ptr next_; // 后继指针

        // 默认构造：使节点自环，便于空链表操作
        ListNodeBase()
            : prev_(this), next_(this)
        {
        }

        // 转换为具体节点指针，以访问 data_
        node_ptr as_node()
        {
            return static_cast<node_ptr>(this);
        }
    };

    /* 双向链表数据节点，继承自 ListNodeBase */
    template <typename T>
    struct ListNode : public ListNodeBase<T>
    {
        using base_ptr = typename node_traits<T>::base_ptr;

        // 完美转发构造：在分配器构造后直接初始化 data_
        template <typename... Args>
        ListNode(Args &&...args)
            : data_(std::forward<Args>(args)...) // 转发构造 T
        {
        }

        // 转换为基节点指针，用于链表内部链接操作
        base_ptr as_base()
        {
            return static_cast<base_ptr>(this);
        }

        T data_; // 存储用户数据
    };

    /*封装为迭代器，根据数据类型、引用类型和指针类型实例化*/
    template <typename T, typename Ref, typename Ptr>
    struct ListIterator
    {
        using base_ptr = typename node_traits<T>::base_ptr;
        using node_ptr = typename node_traits<T>::node_ptr;
        using Self = ListIterator<T, Ref, Ptr>; // 当前迭代器类型

        // 迭代器萃取必需的五个类型
        using iterator_category = bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = Ptr;
        using reference = Ref;

        // 转换构造函数
        ListIterator(const ListIterator<T, T &, T *> &rhs)
            : base_(rhs.base_) {}

        base_ptr base_; // 当前迭代器指向的节点

        // 构造函数，传入节点指针
        ListIterator(base_ptr node) : base_(node)
        {
        }

        // 重载解引用运算符，返回节点存储的数据
        Ref operator*() const
        {
            return base_->as_node()->data_;
        }

        // 重载成员访问运算符，返回数据的指针
        Ptr operator->() const
        {
            return &(base_->as_node()->data_);
        }

        // 前置++运算符，迭代器前移到下一个节点
        Self &operator++()
        {
            base_ = base_->next_;
            return *this;
        }

        // 后置++运算符，先保存当前状态再前移到下一个节点
        Self operator++(int)
        {
            Self self(*this);
            base_ = base_->next_;
            return self;
        }

        // 前置--运算符，迭代器后移到上一个节点
        Self &operator--()
        {
            base_ = base_->prev_;
            return *this;
        }

        // 后置--运算符，先保存当前状态再后移到上一个节点
        Self operator--(int)
        {
            Self self(*this);
            base_ = base_->prev_;
            return self;
        }

        // 重载相等运算符，判断两个迭代器是否指向同一节点
        bool operator==(const Self &s) const
        {
            return base_ == s.base_;
        }

        // 重载不等运算符，判断两个迭代器是否不指向同一节点
        bool operator!=(const Self &s) const
        {
            return base_ != s.base_;
        }
    };

    /* list 类模板，实现双向链表，支持自定义分配器 */
    template <typename T, typename Alloc = std::allocator<T>>
    class list
    {
    public:
        // --------------- 类型重定义 ---------------
        using allocator_type = Alloc;
        using traits_allocator = allocator_traits<allocator_type>;
        using value_type = T;
        using reference = T &;
        using const_reference = const T &;
        using pointer = typename traits_allocator::pointer;
        using const_pointer = typename traits_allocator::const_pointer;
        using size_type = typename traits_allocator::size_type;
        using difference_type = typename traits_allocator::difference_type;

        // 针对节点类型的分配器重绑定
        using node_type = ListNode<T>;
        using node_allocator_type = typename traits_allocator::template rebind_alloc<node_type>;
        using node_traits_alloc = allocator_traits<node_allocator_type>;

    public:
        // 迭代器类型定义
        using iterator = ListIterator<T, T &, T *>;
        using const_iterator = ListIterator<T, const T &, const T *>;
        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<const_iterator>;

        // --------------- 迭代器接口 ---------------
        iterator begin() { return head_->next_; }
        iterator end() { return head_; }
        const_iterator begin() const { return head_->next_; }
        const_iterator end() const { return head_; }
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    public:
        // 默认构造：可传入自定义分配器，默认为 allocator_type()
        list(const allocator_type &alloc = allocator_type())
            : alloc_(alloc), node_alloc_(alloc_) // 同步节点分配器
        {
            empty_init(); // 初始化空链表（头节点自环）
        }

        // 填充构造：n 个 val，使用相同的分配器参数
        list(size_type n, const T &val = T(), const allocator_type &alloc = allocator_type())
            : alloc_(alloc), node_alloc_(alloc_)
        {
            fill_init(n, val);
        }

        // 范围构造：基于输入迭代器区间构造链表
        template <typename InputIter>
        list(InputIter first, InputIter last, const allocator_type &alloc = allocator_type())
            : alloc_(alloc), node_alloc_(alloc_)
        {
            range_init(first, last);
        }

        // initializer_list 构造
        list(std::initializer_list<T> il, const allocator_type &alloc = allocator_type())
            : alloc_(alloc), node_alloc_(alloc_)
        {
            range_init(il.begin(), il.end());
        }

        // 拷贝构造：允许有状态分配器选择复制策略
        list(const list &other)
            : alloc_(other.alloc_),
              node_alloc_(alloc_), head_(nullptr), size_(0)
        {
            empty_init();
            for (auto &v : other)
                push_back(v);
        }

        // 移动构造：保持 noexcept，节点和大小转移，源置空
        list(list &&other) noexcept
            : alloc_(std::move(other.alloc_)), node_alloc_(alloc_),
              head_(other.head_), size_(other.size_)
        {
            other.head_ = nullptr;
            other.size_ = 0;
        }

        // 赋值运算符：拷贝-交换惯用法
        list &operator=(const list &other)
        {
            if (this != &other)
            {
                list tmp(other);
                swap(tmp);
            }
            return *this;
        }

        // 移动赋值
        list &operator=(list &&other) noexcept
        {
            if (this != &other)
            {
                clear();
                destroy_node(reinterpret_cast<node_type *>(head_));
                head_ = other.head_;
                size_ = other.size_;
                alloc_ = std::move(other.alloc_);
                node_alloc_ = node_allocator_type(alloc_);
                other.head_ = nullptr;
                other.size_ = 0;
            }
            return *this;
        }

        // 析构：清空并释放头节点
        ~list()
        {
            if (head_)
            {
                clear();
                // 销毁并释放头节点
                destroy_node(reinterpret_cast<node_type *>(head_));
                head_ = nullptr;
            }
        }

        // --------------- 容量相关 ---------------
        bool empty() const noexcept { return size_ == 0; }
        size_type size() const noexcept { return size_; }

        // --------------- 元素访问 ---------------
        reference front() { return *begin(); }
        const_reference front() const { return *begin(); }
        reference back() { return *--end(); }
        const_reference back() const { return *--end(); }

        // --------------- 修改操作 ---------------
        void clear()
        {
            while (!empty())
                pop_front();
        }

        void push_front(const T &val) {insert(begin(), val); }
        void push_back(const T &val) { emplace_back(val); }
        void pop_back() { erase(--end()); }
        void pop_front() { erase(begin()); }

        iterator insert(iterator pos, const T &val)
        {
            return insert_node(pos, val);
        }

        template <typename... Args>
        void emplace_back(Args &&...args)
        {
            insert_node(end(), std::forward<Args>(args)...);
        }

        iterator erase(iterator pos)
        {
            assert(pos.base_ != head_); // 不允许删除头节点
            auto prev = pos.base_->prev_;
            auto next = pos.base_->next_;
            prev->next_ = next;
            next->prev_ = prev;
            destroy_node(reinterpret_cast<node_type *>(pos.base_));
            --size_;
            return iterator(next);
        }

        void resize(size_type n, const T &val = T())
        {
            if (n < size_)
                while (size_ > n)
                    pop_back();
            else
                while (size_ < n)
                    push_back(val);
        }

        void swap(list &rhs) noexcept
        {
            std::swap(head_, rhs.head_);
            std::swap(size_, rhs.size_);
            std::swap(alloc_, rhs.alloc_);
            std::swap(node_alloc_, rhs.node_alloc_);
        }

    private:
        // 初始化空链表（头节点自环），使用分配器完成内存管理
        void empty_init()
        {
            node_type *p = node_traits_alloc::allocate(node_alloc_, 1);
            node_traits_alloc::construct(node_alloc_, p); // 默认构造 ListNodeBase<T>
            head_ = reinterpret_cast<ListNodeBase<T> *>(p);
            head_->next_ = head_;
            head_->prev_ = head_;
            size_ = 0;
        }

        // 填充初始化：尾部插入 n 个相同值
        void fill_init(size_type n, const T &val)
        {
            empty_init();
            while (n--)
                emplace_back(val);
        }

        // 范围初始化：按序插入[first, last)
        template <typename InputIter>
        void range_init(InputIter first, InputIter last)
        {
            empty_init();
            for (; first != last; ++first)
                emplace_back(*first);
        }

        // 插入节点：分配并构造，再链接到链表中
        template <typename... Args>
        iterator insert_node(iterator pos, Args &&...args)
        {
            node_type *p = create_node(std::forward<Args>(args)...);
            auto b = reinterpret_cast<ListNodeBase<T> *>(p);
            b->prev_ = pos.base_->prev_;
            b->next_ = pos.base_;
            pos.base_->prev_->next_ = b;
            pos.base_->prev_ = b;
            ++size_;
            return iterator(b);
        }

        // 创建节点：分配内存并调用构造
        template <typename... Args>
        node_type *create_node(Args &&...args)
        {
            node_type *p = node_traits_alloc::allocate(node_alloc_, 1);
            node_traits_alloc::construct(node_alloc_, p, std::forward<Args>(args)...);
            return p;
        }

        // 销毁节点：调用析构并释放内存
        void destroy_node(node_type *p)
        {
            node_traits_alloc::destroy(node_alloc_, p);
            node_traits_alloc::deallocate(node_alloc_, p, 1);
        }

    private:
        allocator_type alloc_;           // 用户传入或默认分配器
        node_allocator_type node_alloc_; // 针对节点重绑定的分配器
        ListNodeBase<T> *head_;          // 哨兵头节点，不存储数据
        size_type size_;                 // 有效元素计数
    };
}
