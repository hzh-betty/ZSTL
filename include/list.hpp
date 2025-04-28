#pragma once
#include <iostream>
#include <cassert>

namespace zstl
{
    /*带头双向链表的节点模板*/
    template <typename T>
    struct ListNode
    {
        // 构造函数，使用传入的值初始化节点数据，默认使用T()构造默认值
        ListNode(const T &val = T())
            : data_(val), prev_(nullptr), next_(nullptr)
        {
        }
        // 右值引用构造函数
        ListNode(T &&val)
            : data_(std::move(val)), prev_(nullptr), next_(nullptr)
        {
        }
        T data_;            // 节点存储的数据
        ListNode<T> *prev_; // 指向前一个节点的指针
        ListNode<T> *next_; // 指向后一个节点的指针
    };

    /*封装为迭代器，根据数据类型、引用类型和指针类型实例化*/
    template <typename T, typename Ref, typename Ptr>
    struct ListIterator
    {
        using Node = ListNode<T>;               // 节点类型
        using Self = ListIterator<T, Ref, Ptr>; // 当前迭代器类型

        Node *node_; // 当前迭代器指向的节点

        // 构造函数，传入节点指针
        ListIterator(Node *node) : node_(node)
        {
        }

        // 拷贝构造函数
        ListIterator(const Self &s)
            : node_(s.node_)
        {
        }

        // 重载解引用运算符，返回节点存储的数据
        Ref operator*()
        {
            return node_->data_;
        }

        // 重载成员访问运算符，返回数据的指针
        Ptr operator->()
        {
            return &node_->data_;
        }

        // 前置++运算符，迭代器前移到下一个节点
        Self &operator++()
        {
            node_ = node_->next_;
            return *this;
        }

        // 后置++运算符，先保存当前状态再前移到下一个节点
        Self operator++(int)
        {
            Self self(*this);
            node_ = node_->next_;
            return self;
        }

        // 前置--运算符，迭代器后移到上一个节点
        Self &operator--()
        {
            node_ = node_->prev_;
            return *this;
        }

        // 后置--运算符，先保存当前状态再后移到上一个节点
        Self operator--(int)
        {
            Self self(*this);
            node_ = node_->prev_;
            return self;
        }

        // 重载相等运算符，判断两个迭代器是否指向同一节点
        bool operator==(const Self &s) const
        {
            return node_ == s.node_;
        }

        // 重载不等运算符，判断两个迭代器是否不指向同一节点
        bool operator!=(const Self &s) const
        {
            return node_ != s.node_;
        }
    };

    /*模拟 C++ STL 的 list 类模板，实现双向链表*/
    template <typename T>
    class list
    {
    private:
        using Node = ListNode<T>;

    public:
        using iterator = ListIterator<T, T &, T *>;             // 普通迭代器类型
        using const_iterator = ListIterator<T, const T &, T *>; // const 迭代器类型

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

    public:
        // 初始化空链表，构造头节点，并使链表空（环状连接头节点）
        void empty_init()
        {
            head_ = new Node; // 构造头节点，注意头节点不存数据
            head_->next_ = head_;
            head_->prev_ = head_;
            size_ = 0; // 初始时链表中无数据节点
        }

        // 默认构造函数
        list()
        {
            empty_init();
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
        // initializer_list 构造函数
        list(std::initializer_list<T> il)
        {
            empty_init();
            for (const auto &val : il)
            {
                push_back(val);
            }
        }

        // 赋值运算符，采用拷贝-交换策略
        list<T> &operator=(list<T> it)
        {
            swap(it);
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
            insert(end(), T(std::forward<Args>(args)...));
        }
        // 在迭代器 it 之前插入右值元素 val，并返回新节点的迭代器
        iterator insert(iterator it, T &&val)
        {
            // 1. 构造新节点，使用 std::move 将右值传递给节点构造函数
            Node *cur = it.node_;
            Node *prev_node = cur->prev_;
            Node *new_node = new Node(std::move(val));

            // 2. 链接各节点：将新节点插入到 prev_node 和 cur 之间
            prev_node->next_ = new_node;
            cur->prev_ = new_node;
            new_node->prev_ = prev_node;
            new_node->next_ = cur;
            ++size_;

            return new_node;
        }
        // 在迭代器 it 之前插入值为 val 的新节点，并返回新节点的迭代器
        iterator insert(iterator it, const T &val)
        {
            // 1. 构造新节点
            Node *cur = it.node_;
            Node *prev_node = cur->prev_;
            Node *new_node = new Node(val);

            // 2. 链接各节点：将新节点插入到 prev_node 和 cur 之间
            prev_node->next_ = new_node;
            cur->prev_ = new_node;
            new_node->prev_ = prev_node;
            new_node->next_ = cur;
            ++size_;

            return new_node;
        }

        // 在链表尾部添加新元素
        void push_back(const T &val)
        {
            insert(end(), val);
        }

        // 在链表头部添加新元素
        void push_front(const T &val)
        {
            insert(begin(), val);
        }

        // 删除迭代器 pos 指向的节点，并返回下一个节点的迭代器
        iterator erase(iterator pos)
        {
            // 1. 删除前检查，确保 pos 不是尾节点（头节点）
            assert(pos != end());
            // 获取被删除节点前后的节点
            Node *prev_node = pos.node_->prev_;
            Node *next_node = pos.node_->next_;
            // 删除当前节点
            delete pos.node_;

            // 2. 重新链接前后节点
            prev_node->next_ = next_node;
            next_node->prev_ = prev_node;
            --size_;
            return next_node;
        }

        // 返回链表中存储的元素数量
        size_t size() const
        {
            return size_;
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
            // 循环删除所有非头节点
            for (iterator iter = begin(); iter != end();)
            {
                // erase 返回下一个节点，不需要再递增迭代器
                iter = erase(iter);
            }
        }

        // 交换两个链表的内部数据
        void swap(list<T> &it)
        {
            std::swap(head_, it.head_);
            std::swap(size_, it.size_);
        }

        // 析构函数，清空链表并释放头节点内存
        ~list()
        {
            clear();
            delete head_;
            head_ = nullptr;
        }

    private:
        Node *head_;  // 链表的头节点（不存储有效数据）
        size_t size_; // 链表有效节点的数量
    };
};
