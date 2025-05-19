#pragma once
#include "../iterator/reverse_iterator.hpp"
#include "../allocator/alloc.hpp"
#include "../allocator/memory.hpp"
namespace zstl
{
    // 单向链表节点定义
    template <typename T>
    struct ForwardListNode
    {
        T data_;                          // 节点存储的数据
        ForwardListNode *next_ = nullptr; // 指向下一个节点的指针

        // 完美转发构造函数
        template <typename... Args>
        ForwardListNode(Args &&...args) : data_(std::forward<Args>(args)...)
        {
        }
    };

    // 迭代器模板：支持可读写与只读两种模式
    template <typename T, typename Ref, typename Ptr>
    struct ForwardListIterator
    {
        using Node = ForwardListNode<T>;
        using Self = ForwardListIterator<T, Ref, Ptr>;

        // 迭代器萃取必需的五种类型
        using iterator_category = forward_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = Ptr;
        using reference = Ref;

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
    template <typename T, typename Alloc = alloc<T>>
    class forward_list
    {
    public:
        using iterator = ForwardListIterator<T, T &, T *>;
        using const_iterator = ForwardListIterator<T, const T &, const T *>;

        using allocator_type = Alloc;
        using traits_allocator = allocator_traits<allocator_type>;
        using value_type = T;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = typename traits_allocator::pointer;
        using const_pointer = typename traits_allocator::const_pointer;
        using size_type = size_t;
        using difference_type = typename iterator_traits<iterator>::difference_type;

        // 针对节点类型的分配器重绑定
        using Node = ForwardListNode<value_type>;
        using node_allocator_type = typename traits_allocator::template rebind_alloc<Node>;
        using node_traits_alloc = allocator_traits<node_allocator_type>;

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
        forward_list(const allocator_type &alloc = allocator_type())
            : alloc_(alloc), node_alloc_(alloc_) // 同步节点分配器
        {
            header_ = create_node(T());
        }

        // 析构：清空所有节点并删除头节点
        ~forward_list()
        {
            if (header_)
            {
                clear();
                destroy_node(header_);

                header_ = nullptr;
            }
        }

        // 复制构造：深拷贝链表数据
        forward_list(const forward_list &other)
            : alloc_(other.alloc_),
              node_alloc_(alloc_)
        {
            header_ = create_node(T());
            Node *cur = other.header_->next_;
            Node *prev = header_;
            while (cur)
            {
                Node *new_node = create_node(cur->data_);
                prev->next_ = new_node;
                prev = new_node;
                cur = cur->next_;
            }
        }

        // 赋值运算：拷贝并交换
        forward_list &operator=(const forward_list &other)
        {
            if (this != &other)
            {
                forward_list<T> tmp(other);
                swap(tmp);
            }
            return *this;
        }

        // 移动构造函数
        forward_list(forward_list &&other) noexcept
            : alloc_(std::move(other.alloc_)), node_alloc_(alloc_), header_(other.header_)
        {
            other.header_ = nullptr;
        }

        // 移动赋值运算符
        forward_list &operator=(forward_list &&other) noexcept
        {
            if (this != &other)
            {
                clear();
                destroy_node(header_);
                alloc_ = std::move(other.alloc_);
                node_alloc_ = node_allocator_type(alloc_);
                header_ = other.header_;
                other.header_ = nullptr;
            }
            return *this;
        }

        // initializer_list 构造函数
        forward_list(std::initializer_list<T> il)
        {
            header_ = create_node(T());
            Node *tail = header_;
            for (const auto &val : il)
            {
                Node *new_node = create_node(val);
                tail->next_ = new_node;
                tail = new_node;
            }
        }

        // emplace_after 接口
        template <typename... Args>
        iterator emplace_after(iterator pos, Args &&...args)
        {
            Node *new_node = create_node(std::forward<Args>(args)...);
            new_node->next_ = pos.node_->next_;
            pos.node_->next_ = new_node;
            return iterator(new_node);
        }

        // 访问首元素
        T &front() { return header_->next_->data_; }
        const T &front() const { return header_->next_->data_; }

        // 在头部插入元素
        void push_front(const T &val)
        {
            Node *new_node = create_node(val);
            new_node->next_ = header_->next_;
            header_->next_ = new_node;
        }

        // 删除头部元素
        void pop_front()
        {
            if (header_->next_)
            {
                Node *tmp = header_->next_;
                header_->next_ = tmp->next_;
                destroy_node(tmp);
            }
        }

        // 在指定位置之后插入元素，返回新节点迭代器
        iterator insert_after(iterator pos, const T &val)
        {
            Node *new_node = create_node(val);
            new_node->next_ = pos.node_->next_;
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
                destroy_node(tmp);

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
            zstl::swap(header_->next_, other.header_->next_);
            zstl::swap(alloc_, other.alloc_);
            zstl::swap(node_alloc_, other.node_alloc_);
        }

        // 判断是否为空（仅检查首节点是否存在）
        bool empty() const { return header_->next_ == nullptr; }

    private:
        // 创建节点：分配内存并调用构造
        template <typename... Args>
        Node *create_node(Args &&...args)
        {
            Node *p = node_traits_alloc::allocate(node_alloc_, 1);
            node_traits_alloc::construct(node_alloc_, p, std::forward<Args>(args)...);
            return p;
        }

        // 销毁节点：调用析构并释放内存
        void destroy_node(Node *p)
        {
            node_traits_alloc::destroy(node_alloc_, p);
            node_traits_alloc::deallocate(node_alloc_, p, 1);
        }

    private:
        allocator_type alloc_;           // 用户传入或默认分配器
        node_allocator_type node_alloc_; // 针对节点重绑定的分配器
        Node *header_;                   // 哨兵头节点，不存储有效数据
    };
}; // namespace zstl