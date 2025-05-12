#pragma once
#include <iostream>
#include <cassert>
#include "../iterator/reverse_iterator.hpp"

namespace zstl
{
    // 前置声明
    template <typename T>
    struct ListNodeBase;
    template <typename T>
    struct ListNode;

    // 节点特性萃取
    template <typename T>
    struct node_traits
    {
        using base_ptr = ListNodeBase<T> *;
        using node_ptr = ListNode<T> *;
    };

    /* 链表基节点（不包含数据）*/
    template <typename T>
    struct ListNodeBase
    {
        using base_ptr = typename node_traits<T>::base_ptr;
        using node_ptr = typename node_traits<T>::node_ptr;

        base_ptr prev_;
        base_ptr next_;

        ListNodeBase()
            : prev_(this), next_(this)
        {
        }

        // 转换为节点数据指针
        node_ptr as_node()
        {
            return static_cast<node_ptr>(this);
        }
    };

    /*带头双向链表的节点模板*/
    template <typename T>
    struct ListNode : public ListNodeBase<T>
    {
        using base_ptr = typename node_traits<T>::base_ptr;

        // 完美转发构造函数
        template <typename... Args>
        ListNode(Args &&...args) : data_(std::forward<Args>(args)...)
        {
        }

        // 转换为基节点指针
        base_ptr as_base()
        {
            return static_cast<base_ptr>(this);
        }

        T data_; // 节点存储的数据
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

    /*list 类模板，实现双向链表*/
    template <typename T>
    class list
    {
    private:
        using base_ptr = typename node_traits<T>::base_ptr;
        using node_ptr = typename node_traits<T>::node_ptr;

    public:
        using iterator = ListIterator<T, T &, T *>;                   // 普通迭代器类型
        using const_iterator = ListIterator<T, const T &, const T *>; // const 迭代器类型

        // 返回指向首个有效数据节点的迭代器
        iterator begin()
        {
            return head_->next_;
        }

        // 返回指向尾部（头节点）的迭代器
        iterator end()
        {
            return head_;
        }

        // const 版本的 begin
        const_iterator begin() const
        {
            return head_->next_;
        }

        // const 版本的 end
        const_iterator end() const
        {
            return head_;
        }

        // 反向迭代器
        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<iterator>;

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    public:
        // 默认构造函数
        list()
        {
            empty_init();
        }
        // 析构函数，清空链表并释放头节点内存
        ~list()
        {
            if (head_ != nullptr)
            {
                clear();
                delete head_;
                head_ = nullptr;
            }
        }
        list(int n, const T &val = T())
        {
            fill_init(n, val);
        }
        list(size_t n, const T &val = T())
        {
            fill_init(n, val);
        }

        template <typename InputIter>
        list(InputIter first, InputIter last)
        {
            range_init(first, last);
        }

        // initializer_list 构造函数
        list(std::initializer_list<T> il)
        {
            range_init(il.begin(), il.end());
        }

        // 拷贝构造函数，将传入的链表中的每个元素插入到新链表中
        list(const list &other)
        {
            empty_init();
            // 通过迭代器遍历源链表，并依次尾插数据
            for (auto &it : other)
            {
                push_back(it);
            }
        }

        // 赋值运算符，采用拷贝-交换策略
        list<T> &operator=(const list<T> &it)
        {
            if (this != &it)
            {
                list<T> tmp(it);
                swap(tmp);
            }
            return *this;
        }

        // 移动构造函数
        list(list &&other)
            : head_(other.head_), size_(other.size_)
        {
            other.head_ = nullptr;
            other.size_ = 0;
        }

        // 移动赋值运算符
        list<T> &operator=(list<T> &&other)
        {
            if (this != &other)
            {
                clear();
                delete head_;
                head_ = other.head_;
                size_ = other.size_;
                other.head_ = nullptr;
                other.size_ = 0;
            }
            return *this;
        }

        // emplace_back 接口
        template <typename... Args>
        void emplace_back(Args &&...args)
        {
            insert_node(end(), std::forward<Args>(args)...);
        }

        // 在迭代器 pos 之前插入值为 val 的新节点，并返回新节点的迭代器
        iterator insert(iterator pos, const T &val)
        {
            // 1. 创建新节点
            node_ptr new_node = create_node(val);

            // 2. 将节点链接到链表
            link_nodes(pos.base_, new_node->as_base(), new_node->as_base());

            // 3. 更新大小并返回迭代器
            ++size_;
            return iterator(new_node->as_base());
        }

        // 在链表尾部添加新元素
        void push_back(const T &val)
        {
            emplace_back(val);
        }

        // 在链表头部添加新元素
        void push_front(const T &val)
        {
            insert(begin(), val);
        }

        // 删除迭代器 pos 指向的节点，并返回下一个节点的迭代器
        iterator erase(const_iterator pos)
        {
            // 1. 删除前检查，确保 pos 不是尾节点（头节点）
            assert(pos.base_ != head_);
            // 获取被删除节点前后的节点
            base_ptr prev_node = pos.base_->prev_;
            base_ptr next_node = pos.base_->next_;

            // 2. 重新链接前后节点
            prev_node->next_ = next_node;
            next_node->prev_ = prev_node;

            // 3. 删除节点
            destory_node(static_cast<node_ptr>(pos.base_));
            --size_;
            return iterator(next_node);
        }

        // 删除尾部元素
        void pop_back()
        {
            // --end() 表示指向最后一个有效节点
            erase(--end());
        }

        // 删除头部元素
        void pop_front()
        {
            erase(begin());
        }

        // 获取头节点元素
        T &front()
        {
            return *(begin());
        }
        const T &front() const
        {
            return *(begin());
        }
        // 获取尾节点元素
        T &back()
        {
            return *(--end());
        }

        const T &back() const
        {
            return *(--end());
        }

        // 调整大小：newSize>size时填充字符，否则截断
        void resize(size_t n, const T &val = T())
        {
            if (n < size_)
            {
                while (size_ != n)
                {
                    pop_back();
                }
            }
            else
            {
                while (size_ != n)
                {
                    push_back(val);
                }
            }
        }

        // 清空链表中所有元素
        void clear()
        {
            while (!empty())
            {
                pop_front();
            }
        }
        // 交换两个链表的内部数据
        void swap(list<T> &it)
        {
            std::swap(head_, it.head_);
            std::swap(size_, it.size_);
        }

        bool empty() const
        {
            return size_ == 0;
        }

        size_t size() const
        {
            return size_;
        }

    private:
        // 初始化空链表，构造头节点，并使链表空（环状连接头节点）
        void empty_init()
        {
            head_ = new ListNodeBase<T>(); // 构造头节点，注意头节点不存数据
            head_->next_ = head_;
            head_->prev_ = head_;
            size_ = 0; // 初始时链表中无数据节点
        }

        void fill_init(size_t n, const T &val)
        {
            empty_init();
            while (n--)
            {
                emplace_back(val);
            }
        }

        template <typename InputIter>
        void range_init(InputIter first, InputIter last)
        {
            empty_init();
            for (; first != last; ++first)
            {
                emplace_back(*first);
            }
        }

        // 制造节点
        template <typename... Args>
        node_ptr create_node(Args &&...args)
        {
            return new ListNode<T>(std::forward<Args>(args)...);
        }

        // 销毁节点
        void destory_node(node_ptr p)
        {
            delete p;
        }

        // 在pos之前链接节点
        void link_nodes(base_ptr pos, base_ptr first, base_ptr last)
        {
            first->prev_ = pos->prev_;
            last->next_ = pos;
            pos->prev_->next_ = first;
            pos->prev_ = last;
        }

        // 在pos之前插入节点
        template <typename... Args>
        iterator insert_node(const_iterator pos, Args &&...args)
        {
            node_ptr new_node = create_node(std::forward<Args>(args)...);
            link_nodes(pos.base_, new_node->as_base(), new_node->as_base());
            ++size_;
            return iterator(new_node);
        }

    private:
        base_ptr head_; // 链表的头节点（不存储有效数据）
        size_t size_;   // 链表有效节点的数量
    };

};
