#pragma once
#include <iostream>

namespace zstl
{
    // 单向链表节点定义
    template <typename T>
    struct ForwardListNode
    {
        T data_;                // 节点存储的数据
        ForwardListNode *next_; // 指向下一个节点的指针

        // 构造函数：初始化数据与下一节点指针
        ForwardListNode(const T &data = T(), ForwardListNode *node = nullptr)
            : data_(data), next_(node)
        {
        }
        // 构造函数：初始化数据与下一节点指针
        ForwardListNode(T &&data, ForwardListNode *node = nullptr)
            : data_(std::move(data)), next_(node)
        {
        }
    };

    // 迭代器模板：支持可读写与只读两种模式
    template <typename T, typename Ref, typename Ptr>
    struct ForwardListIterator
    {
        using Node = ForwardListNode<T>;
        using Self = ForwardListIterator<T, Ref, Ptr>;

        Node *node_; // 当前指向的节点

        // 构造：可指定起始节点，默认为空
        ForwardListIterator(Node *node = nullptr)
            : node_(node)
        {
        }

        // 解引用：获取节点数据引用
        Ref operator*() const { return node_->data_; }

        // 成员访问：获取数据指针
        Ptr operator->() const { return &node_->data_; }

        // 前置++：移动到下一个节点
        Self &operator++()
        {
            if (node_)
                node_ = node_->next_;
            return *this;
        }

        // 后置++：返回递增前的迭代器
        Self operator++(int)
        {
            Self tmp(*this);
            ++(*this);
            return tmp;
        }

        // 相等比较
        bool operator==(const Self &other) const { return node_ == other.node_; }
        bool operator!=(const Self &other) const { return node_ != other.node_; }
    };

    // 自定义 forward_list 容器
    template <typename T>
    class forward_list
    {
        using Node = ForwardListNode<T>;

    public:
        using iterator = ForwardListIterator<T, T &, T *>;
        using const_iterator = ForwardListIterator<T, const T &, const T *>;

        // 获取头前迭代器（哨兵节点）
        iterator before_begin() { return iterator(header_); }
        const_iterator before_begin() const { return const_iterator(header_); }

        // 获取首元素迭代器
        iterator begin() { return iterator(header_->next_); }
        const_iterator begin() const { return const_iterator(header_->next_); }

        // 获取尾后迭代器（空）
        iterator end() { return iterator(nullptr); }
        const_iterator end() const { return const_iterator(nullptr); }

    public:
        // 构造：创建哨兵头节点
        forward_list()
            : header_(new Node())
        {
        }

        // 析构：清空所有节点并删除头节点
        ~forward_list()
        {
            clear();
            delete header_;
            header_ = nullptr;
        }

        // 复制构造：深拷贝链表数据
        forward_list(const forward_list &other)
            : header_(new Node())
        {
            Node *cur = other.header_->next_;
            Node *prev = header_;
            while (cur)
            {
                Node *new_node = new Node(cur->data_);
                prev->next_ = new_node;
                prev = new_node;
                cur = cur->next_;
            }
        }

        // 赋值运算：拷贝并交换
        forward_list &operator=(forward_list other)
        {
            swap(other);
            return *this;
        }

        // 移动构造函数
        forward_list(forward_list &&other) noexcept
            : header_(other.header_)
        {
            other.header_ = new Node();
        }

        // 移动赋值运算符
        forward_list &operator=(forward_list &&other) noexcept
        {
            if (this != &other)
            {
                clear();
                delete header_;
                header_ = other.header_;
                other.header_ = new Node();
            }
            return *this;
        }

        // initializer_list 构造函数
        forward_list(std::initializer_list<T> il)
            : header_(new Node())
        {
            Node *tail = header_;
            for (const auto &val : il)
            {
                Node *new_node = new Node(val);
                tail->next_ = new_node;
                tail = new_node;
            }
        }

        // emplace_after 接口
        template <typename... Args>
        iterator emplace_after(iterator pos, Args &&...args)
        {
            Node *new_node = new Node(T(std::forward<Args>(args)...), pos.node_->next_);
            pos.node_->next_ = new_node;
            return iterator(new_node);
        }
        // 访问首元素
        T &front() { return header_->next_->data_; }
        const T &front() const { return header_->next_->data_; }

        // 在头部插入元素
        void push_front(const T &val)
        {
            Node *new_node = new Node(val, header_->next_);
            header_->next_ = new_node;
        }

        // 删除头部元素
        void pop_front()
        {
            if (header_->next_)
            {
                Node *tmp = header_->next_;
                header_->next_ = tmp->next_;
                delete tmp;
            }
        }

        // 在指定位置之后插入元素，返回新节点迭代器
        iterator insert_after(iterator pos, const T &val)
        {
            Node *new_node = new Node(val, pos.node_->next_);
            pos.node_->next_ = new_node;
            return iterator(new_node);
        }

        // 删除指定位置之后的节点，返回下一个节点迭代器
        iterator erase_after(iterator pos)
        {
            if (pos.node_ && pos.node_->next_)
            {
                Node *tmp = pos.node_->next_;
                pos.node_->next_ = tmp->next_;
                delete tmp;
                return iterator(pos.node_->next_);
            }
            return end();
        }

        // 清空列表
        void clear()
        {
            while (header_->next_)
                pop_front();
        }

        // 与另一个列表交换节点
        void swap(forward_list &other)
        {
            std::swap(header_->next_, other.header_->next_);
        }

        // 判断是否为空（仅检查首节点是否存在）
        bool empty() const { return header_->next_ == nullptr; }

    private:
        Node *header_; // 哨兵头节点，不存储有效数据
    };
} // namespace zstl