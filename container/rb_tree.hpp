#pragma once
#include <utility>
#include "../iterator/reverse_iterator.hpp"
#include "../allocator/alloc.hpp"
#include "../allocator/memory.hpp"
namespace zstl
{
    // 红黑树的颜色
    enum class Color
    {
        RED,  // 红色
        BLACK // 黑色
    };

    // 键提取器：从 value_type 中获取 key
    struct KeyOfValue
    {
        template <typename K, typename V>
        const K &operator()(const std::pair<const K, V> &kv) const
        {
            return kv.first;
        }

        template <typename K>
        const K &operator()(const K &k) const
        {
            return k;
        }
    };

    // 红黑树的节点
    template <typename T>
    struct RBNode
    {
        // 完美转发构造函数
        template <typename... Args>
        RBNode(Args &&...args) : data_(std::forward<Args>(args)...)
        {
        }

        RBNode<T> *left_ = nullptr;   // 左子树
        RBNode<T> *right_ = nullptr;  // 右子树
        RBNode<T> *parent_ = nullptr; // 父节点
        T data_;                      // 键值
        Color col_ = Color::RED;      // 颜色
    };

    // 红黑树的迭代器
    template <typename T, typename Ref, typename Ptr>
    struct RBTreeIterator
    {
        using Node = RBNode<T>;
        using Self = RBTreeIterator<T, Ref, Ptr>;

        // 迭代器萃取必需的五个类型
        using iterator_category = bidirectional_iterator_tag;
        using value_type = T;
        using difference_type = ptrdiff_t;
        using pointer = Ptr;
        using reference = Ref;

        explicit RBTreeIterator(Node *node)
            : node_(node)
        {
        }

        Self &operator=(const Self &it)
        {
            node_ = it.node_;
            return *this;
        }

        // 普通迭代器构造const迭代器
        RBTreeIterator(const RBTreeIterator<T, T &, T *> &it)
            : node_(it.node_)
        {
        }

        Ref operator*() const
        {
            return node_->data_;
        }

        Ptr operator->() const
        {
            return &node_->data_;
        }

        // 判断是否相同
        bool operator==(const Self &self) const
        {
            return node_ == self.node_;
        }

        // 判断是否不相同
        bool operator!=(const Self &self) const
        {
            return node_ != self.node_;
        }

        // 前置++
        Self &operator++()
        {
            // 如果右子树不为空
            if (node_->right_)
            {
                // 找到右子树的最小值
                Node *left = node_->right_;
                while (left->left_)
                {
                    left = left->left_;
                }
                node_ = left;
            }
            else
            {
                // 找到孩子不在右的祖先
                Node *cur = node_;
                Node *parent = node_->parent_;
                while (cur != parent->parent_ && cur == parent->right_)
                {
                    cur = parent;
                    parent = parent->parent_;
                }
                node_ = parent;
            }
            return *this;
        }

        // 后置++
        Self operator++(int)
        {
            Self tmp(*this);
            ++(*this);
            return tmp;
        }

        // 前置--
        Self &operator--()
        {
            // 如果是header
            if (node_->parent_->parent_ == node_ && node_->col_ == Color::RED)
            {
                node_ = node_->right_;
            }
            // 左子树不为空
            else if (node_->left_)
            {
                // 找到左子树的最大值
                Node *right = node_->left_;
                while (right->right_)
                {
                    right = right->right_;
                }
                node_ = right;
            }
            else
            {
                // 找到孩子不在左的祖先
                Node *cur = node_;
                Node *parent = cur->parent_;
                while (cur != parent->parent_ && cur == parent->left_)
                {
                    cur = parent;
                    parent = parent->parent_;
                }
                node_ = parent;
            }

            return *this;
        }

        // 后置++
        Self operator--(int)
        {
            Self tmp(*this);
            --(*this);
            return tmp;
        }

        Node *node_; // 节点
    };

    // 红黑树的平衡操作
    template <typename T>
    class RBBalance
    {
        using Node = RBNode<T>;

    protected:
        // 右旋
        void RotateR(Node *parent)
        {
            Node *cur = parent;
            Node *subL = parent->left_;
            Node *subLR = subL->right_;
            parent->left_ = subLR;
            if (subLR)
                subLR->parent_ = parent;
            subL->right_ = parent;
            Node *ppNode = parent->parent_;
            parent->parent_ = subL;
            if (parent == header_->parent_)
            {
                header_->parent_ = subL;
                header_->parent_->parent_ = header_;
            }
            else
            {
                if (ppNode->left_ == parent)
                {
                    ppNode->left_ = subL;
                }
                else
                {
                    ppNode->right_ = subL;
                }
                subL->parent_ = ppNode;
            }
        }

        // 左旋
        void RotateL(Node *parent)
        {
            Node *subR = parent->right_;
            Node *subRL = subR->left_;
            parent->right_ = subRL;
            if (subRL)
                subRL->parent_ = parent;

            subR->left_ = parent;
            Node *ppNode = parent->parent_;

            parent->parent_ = subR;

            if (parent == header_->parent_)
            {
                header_->parent_ = subR;
                header_->parent_->parent_ = header_;
            }
            else
            {
                if (ppNode->right_ == parent)
                {
                    ppNode->right_ = subR;
                }
                else
                {
                    ppNode->left_ = subR;
                }
                subR->parent_ = ppNode;
            }
        }

        // 左右双旋
        void RotateLR(Node *parent)
        {
            RotateL(parent->left_);
            RotateR(parent);
        }

        // 右左双旋
        void RotateRL(Node *parent)
        {
            RotateR(parent->right_);
            RotateL(parent);
        }

        // 插入调整
        void adjust_insert(Node *cur, Node *parent)
        {
            while (parent != header_ && parent->col_ == Color::RED)
            {
                Node *grandfather = parent->parent_;
                if (parent == grandfather->left_)
                {
                    Node *uncle = grandfather->right_;
                    // 情况三：如果叔叔存在且为红
                    if (uncle && uncle->col_ == Color::RED)
                    {
                        parent->col_ = uncle->col_ = Color::BLACK;
                        grandfather->col_ = Color::RED;
                        cur = grandfather;
                        parent = cur->parent_;
                    }
                    else
                    {
                        // 情况四：叔叔不存在/存在且为黑，且cur在parent的左侧
                        if (cur == parent->left_)
                        {
                            //     g
                            //   p   u
                            // c
                            RotateR(grandfather);
                            parent->col_ = Color::BLACK;
                            grandfather->col_ = Color::RED;
                        }
                        else // 情况五：叔叔不存在 / 存在且为黑，cur在parent的右侧
                        {
                            //     g
                            //   p   u
                            //     c
                            RotateLR(grandfather);
                            cur->col_ = Color::BLACK;
                            grandfather->col_ = Color::RED;
                        }
                        // 这时该子树的根节点变为黑色，不需要继续调整
                        break;
                    }
                }
                else
                {
                    Node *uncle = grandfather->left_;
                    // 情况三：如果叔叔存在且为红
                    if (uncle && uncle->col_ == Color::RED)
                    {
                        parent->col_ = uncle->col_ = Color::BLACK;
                        grandfather->col_ = Color::RED;
                        // 继续调整
                        cur = grandfather;
                        parent = cur->parent_;
                    }
                    else
                    {
                        // 情况四：叔叔不存在/存在且为黑，且cur在parent的左侧
                        if (cur == parent->right_)
                        {
                            //    g
                            //  u   p
                            //        c
                            RotateL(grandfather);
                            parent->col_ = Color::BLACK;
                            grandfather->col_ = Color::RED;
                        }
                        else // 情况五：叔叔不存在 / 存在且为黑，cur在parent的右侧
                        {
                            //    g
                            //  u   p
                            //    c
                            RotateRL(grandfather);
                            cur->col_ = Color::BLACK;
                            grandfather->col_ = Color::RED;
                        }
                        // 这时该子树的根节点变为黑色，不需要继续调整
                        break;
                    }
                }
            }
        }

        // 删除调整
        void adjust_erase(Node *delCur, Node *delParent)
        {
            if (delCur->col_ == Color::BLACK) // 删除的是黑色结点
            {
                if (delCur->left_) // 待删除结点有一个红色的左孩子（不可能是黑色）
                {
                    delCur->left_->col_ = Color::BLACK; // 将这个红色的左孩子变黑即可
                }
                else if (delCur->right_) // 待删除结点有一个红色的右孩子（不可能是黑色）
                {
                    delCur->right_->col_ = Color::BLACK; // 将这个红色的右孩子变黑即可
                }
                else // 待删除结点的左右均为空
                {
                    while (delCur != header_->parent_) // 可能一直调整到根结点
                    {
                        if (delCur == delParent->left_) // 待删除结点是其父结点的左孩子
                        {
                            Node *brother = delParent->right_; // 兄弟结点是其父结点的右孩子
                                                               // 新增：若兄弟为空，视作黑色哨兵，向上继续调整
                            // 情况一：brother为红色
                            if (brother->col_ == Color::RED)
                            {
                                delParent->col_ = Color::RED;
                                brother->col_ = Color::BLACK;
                                RotateL(delParent);
                                // 需要继续处理
                                brother = delParent->right_; // 更新brother
                            }
                            // 情况二：brother为黑色，且其左右孩子都是黑色结点或为空
                            if (((brother->left_ == nullptr) || (brother->left_->col_ == Color::BLACK)) &&
                                ((brother->right_ == nullptr) || (brother->right_->col_ == Color::BLACK)))
                            {
                                brother->col_ = Color::RED;
                                if (delParent->col_ == Color::RED)
                                {
                                    delParent->col_ = Color::BLACK;
                                    break;
                                }
                                // 需要继续处理
                                delCur = delParent;
                                delParent = delCur->parent_;
                            }
                            else
                            {
                                // 情况三：brother为黑色，且其左孩子是红色结点，右孩子是黑色结点或为空
                                if ((brother->right_ == nullptr) || (brother->right_->col_ == Color::BLACK))
                                {
                                    brother->left_->col_ = Color::BLACK;
                                    brother->col_ = Color::RED;
                                    RotateR(brother);
                                    // 需要继续处理
                                    brother = delParent->right_; // 更新brother
                                }
                                // 情况四：brother为黑色，且其右孩子是红色结点
                                brother->col_ = delParent->col_;
                                delParent->col_ = Color::BLACK;
                                brother->right_->col_ = Color::BLACK;
                                RotateL(delParent);
                                break; // 情况四执行完毕后调整一定结束
                            }
                        }
                        else // delCur == delParent->right_ //待删除结点是其父结点的右孩子
                        {
                            Node *brother = delParent->left_; // 兄弟结点是其父结点的左孩子
                            // 情况一：brother为红色
                            if (brother->col_ == Color::RED) // brother为红色
                            {
                                delParent->col_ = Color::RED;
                                brother->col_ = Color::BLACK;
                                RotateR(delParent);
                                // 需要继续处理
                                brother = delParent->left_; // 更新brother
                            }
                            // 情况二：brother为黑色，且其左右孩子都是黑色结点或为空
                            if (((brother->left_ == nullptr) || (brother->left_->col_ == Color::BLACK)) && ((brother->right_ == nullptr) || (brother->right_->col_ == Color::BLACK)))
                            {
                                brother->col_ = Color::RED;
                                if (delParent->col_ == Color::RED)
                                {
                                    delParent->col_ = Color::BLACK;
                                    break;
                                }
                                // 需要继续处理
                                delCur = delParent;
                                delParent = delCur->parent_;
                            }
                            else
                            {
                                // 情况三：brother为黑色，且其右孩子是红色结点，左孩子是黑色结点或为空
                                if ((brother->left_ == nullptr) || (brother->left_->col_ == Color::BLACK))
                                {
                                    brother->right_->col_ = Color::BLACK;
                                    brother->col_ = Color::RED;
                                    RotateL(brother);
                                    // 需要继续处理
                                    brother = delParent->left_; // 更新brother
                                }
                                // 情况四：brother为黑色，且其左孩子是红色结点
                                brother->col_ = delParent->col_;
                                delParent->col_ = Color::BLACK;
                                brother->left_->col_ = Color::BLACK;
                                RotateR(delParent);
                                break; // 情况四执行完毕后调整一定结束
                            }
                        }
                    }
                }
            }
        }

    protected:
        Node *header_ = nullptr;
    };

    // 红黑树的基础操作
    template <typename K, typename T, typename Compare, typename Alloc>
    class RBTreeBase : public RBBalance<T>
    {
        using Node = RBNode<T>;
        using iterator = RBTreeIterator<T, T &, T *>;

    public:
        using allocator_type = Alloc;
        using traits_allocator = allocator_traits<allocator_type>;
        using value_type = T;
        using size_type = size_t;
        using difference_type = typename iterator_traits<iterator>::difference_type;

        // 针对节点类型的分配器重绑定
        using node_allocator_type = typename traits_allocator::template rebind_alloc<Node>;
        using node_traits_alloc = allocator_traits<node_allocator_type>;

    protected:
        // 辅助插入节点
        template <bool Unique, typename... Args>
        auto insert_impl(Args &&...args)
        {
            Node *parent = this->header_;
            Node *cur = this->header_->parent_;
            Node *newnode = create_node(std::forward<Args>(args)...);

            // 定位插入点（重复时统一走右支）
            while (cur)
            {
                parent = cur;
                if (this->com_(this->kov_(newnode->data_), this->kov_(cur->data_)))
                    cur = cur->left_;
                else
                {
                    if constexpr (Unique) // —— 编译期分支
                    {
                        // 运行时判断是否重复
                        if (!this->com_(this->kov_(cur->data_), this->kov_(newnode->data_)))
                        {
                            this->destroy_node(newnode);
                            return std::pair<Node *, bool>{cur, false};
                        }
                    }
                    // 对于 Unique==false 或者 未重复，都走右支
                    cur = cur->right_;
                }
            }

            // 创建并挂接
            newnode->parent_ = parent;
            this->link_node(newnode, parent);

            // 平衡调整
            this->adjust_insert(newnode, parent);
            this->header_->parent_->col_ = Color::BLACK;

            if constexpr (Unique)
                return std::pair<Node *, bool>{newnode, true};
            else
                return newnode;
        }

        // 删除的辅助函数
        Node *erase_impl(Node *target, Node *successor)
        {
            // 1. 定位待真正删除的节点 del and its parent delP
            Node *del = nullptr, *delP = nullptr;
            if (!target->left_ || !target->right_)
            {
                // 度0或1：直接删除 target
                del = target;
                delP = target->parent_;
            }
            else
            {
                // 情况 B/双子：交换数据后，将后继节点（必为半叶）设为删除对象
                this->copy_delete_node(target->data_, successor->data_);
                del = successor;
                delP = successor->parent_;
                successor = target;
            }

            // 2. 红黑树删除调整
            Node *del_node = del;
            Node *del_node_parent = delP;
            this->adjust_erase(del, delP);                // 恢复红黑性质
            this->delete_node(del_node, del_node_parent); // 实际摘除节点并释放
            return successor;
        }

        // 查找节点
        Node *find_impl(const K &val) const
        {
            Node *cur = this->header_->parent_;
            while (cur)
            {
                if (this->com_(val, this->kov_(cur->data_)))
                {
                    // 左子树中查找
                    cur = cur->left_;
                }
                else if (this->com_(this->kov_(cur->data_), val))
                {
                    // 右子树中查找
                    cur = cur->right_;
                }
                else
                {
                    // 找到了
                    return cur;
                }
            }
            return this->header_;
        }

        // 插入链接节点
        void link_node(Node *newnode, Node *parent)
        {
            if (parent == this->header_)
            {
                // 树原为空，新节点即根
                this->header_->parent_ = newnode;
                this->header_->left_ = this->header_->right_ = newnode;
            }
            else if (this->com_(this->kov_(newnode->data_), this->kov_(parent->data_)))
            {
                parent->left_ = newnode;
                // 更新最小值指针
                if (this->header_->left_ == parent)
                    this->header_->left_ = newnode;
            }
            else
            {
                parent->right_ = newnode;
                // 更新最大值指针
                if (this->header_->right_ == parent)
                    this->header_->right_ = newnode;
            }
        }

        // 删除节点
        void delete_node(Node *del, Node *delP)
        {
            // 更新最大值与最小值
            if (del == this->header_->left_)
            {
                iterator next(del);
                ++next;
                this->header_->left_ = next.node_;
            }
            if (del == this->header_->right_)
            {
                iterator prev(del);
                --prev;
                this->header_->right_ = prev.node_;
            }

            // 根节点被删：直接将唯一子节点提到根
            if (delP == this->header_)
            {
                // 调整红黑树
                Node *child = del->left_ ? del->left_ : del->right_;
                this->header_->parent_ = child;
                if (child != nullptr)
                {
                    child->parent_ = this->header_;
                    child->col_ = Color::BLACK;
                }
            }
            else if (del->left_ == nullptr) // 实际删除结点的左子树为空
            {
                if (del == delP->left_) // 实际删除结点是其父结点的左孩子
                {
                    delP->left_ = del->right_;
                    // 指向父节点
                    if (del->right_)
                        del->right_->parent_ = delP;
                }
                else // 实际删除结点是其父结点的右孩子
                {
                    delP->right_ = del->right_;
                    if (del->right_)
                        del->right_->parent_ = delP;
                }
            }
            else // 实际删除结点的右子树为空
            {
                if (del == delP->left_) // 实际删除结点是其父结点的左孩子
                {
                    delP->left_ = del->left_;
                    if (del->left_)
                        del->left_->parent_ = delP;
                }
                else // 实际删除结点是其父结点的右孩子
                {
                    delP->right_ = del->left_;
                    if (del->left_)
                        del->left_->parent_ = delP;
                }
            }

            this->destroy_node(del); // 实际删除结点
        }

        // 复制节点
        template <typename L, typename R>
        void copy_delete_node(std::pair<const L, R> &a, std::pair<const L, R> &b)
        {
            // 赋值 const key —— const_cast 是常见 hack
            const_cast<L &>(a.first) = b.first;
            // 赋值 mapped value
            a.second = b.second;
        }

        template <typename L>
        void copy_delete_node(L &a, L &b)
        {
            const_cast<L &>(a) = b;
        }

        // 创建节点
        template <typename... Args>
        Node *create_node(Args &&...args)
        {
            Node *newnode = node_traits_alloc::allocate(node_alloc_, 1);
            node_traits_alloc::construct(node_alloc_, newnode, std::forward<Args>(args)...);
            newnode->left_ = newnode->right_ = newnode->parent_ = nullptr;
            return newnode;
        }

        // 销毁节点
        void destroy_node(Node *node)
        {
            node_traits_alloc::destroy(node_alloc_, node);
            node_traits_alloc::deallocate(node_alloc_, node, 1);
        }

    protected:
        allocator_type alloc_;           // 用户传入或默认分配器
        node_allocator_type node_alloc_; // 针对节点重绑定的分配器
        KeyOfValue kov_;                 // 键提取器：从 value_type 中获取 key
        Compare com_;                    // 比较函数
    };

    // 红黑树
    template <typename K, typename T, typename Compare, typename Alloc>
    class RBTree : public RBTreeBase<K, T, Compare, Alloc>
    {
        using Node = RBNode<T>;

    public:
        using allocator_type = Alloc;

        // 迭代器
        using iterator = RBTreeIterator<T, T &, T *>;
        using const_iterator = RBTreeIterator<T, const T &, const T *>;

        iterator begin()
        {
            return iterator(this->header_->left_);
        }
        const_iterator begin() const
        {
            return iterator(this->header_->left_);
        }

        iterator end()
        {
            return iterator(this->header_);
        }
        const_iterator end() const
        {
            return iterator(this->header_);
        }

        // 反向迭代器
        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<iterator>;

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    public:
        // 构造函数
        RBTree(const allocator_type &alloc = allocator_type())
        {
            // 初始化header节点
            this->alloc_ = alloc;
            this->node_alloc_ = this->alloc_;
            init_header();
        }

        // 拷贝构造
        RBTree(const RBTree &t)
        {
            // 拷贝构造时重建header结构
            this->alloc_ = t.alloc_;
            this->node_alloc_ = this->alloc_;
            init_header();
            this->header_->parent_ = copy(t.header_->parent_);
            adjust_header_pointers(this->header_->parent_);
            size_ = t.size_;
        }

        // 赋值重载
        RBTree &operator=(const RBTree &t)
        {
            if (this != &t)
            {
                RBTree tmp(t);
                swap(tmp);
            }
            return *this;
        }

        // 移动构造函数
        RBTree(RBTree &&other)
            : size_(other.size_)
        {
            this->alloc_ = std::move(other.alloc_);
            this->node_alloc_ = this->alloc_;
            this->header_ = other.header_;
            other.header_ = nullptr;
            other.size_ = 0;
        }

        // 移动赋值运算符
        RBTree &operator=(RBTree &&other) noexcept
        {
            if (this != &other)
            {
                clear();
                this->destroy_node(this->header_);
                this->header_ = other.header_;
                size_ = other.size_;
                this->alloc_ = std::move(other.alloc_);
                this->node_alloc_ = this->alloc_;
                other.header_ = nullptr;
                other.size_ = 0;
            }
            return *this;
        }

        // lower_bound：第一个 ≥ k
        iterator lower_bound(const K &k)
        {
            // 根节点
            Node *cur = this->header_->parent_;
            // 候选，初始为 header_（即 end()）
            Node *res = this->header_;
            while (cur)
            {
                if (this->com_(this->kov_(cur->data_), k))
                {
                    // cur->data_ < k: 下界不可能在左子树
                    cur = cur->right_;
                }
                else
                {
                    // cur->data_ >= k: 记录候选，继续往左找更小的 ≥ k
                    res = cur;
                    cur = cur->left_;
                }
            }
            return iterator(res);
        }

        const_iterator lower_bound(const K &k) const
        {
            Node *cur = this->header_->parent_;
            Node *res = this->header_;
            while (cur)
            {
                if (this->com_(this->kov_(cur->data_), k))
                {
                    cur = cur->right_;
                }
                else
                {
                    res = cur;
                    cur = cur->left_;
                }
            }
            return const_iterator(res);
        }

        // upper_bound：第一个 > k
        iterator upper_bound(const K &k)
        {
            Node *cur = this->header_->parent_;
            Node *res = this->header_;
            while (cur)
            {
                if (this->com_(k, this->kov_(cur->data_)))
                {
                    // k < cur->data_: cur 是一个 > k 的候选
                    res = cur;
                    cur = cur->left_;
                }
                else
                {
                    // cur->data_ ≤ k: 跳过左侧
                    cur = cur->right_;
                }
            }
            return iterator(res);
        }

        const_iterator upper_bound(const K &k) const
        {
            Node *cur = this->header_->parent_;
            Node *res = this->header_;
            while (cur)
            {
                if (this->com_(k, this->kov_(cur->data_)))
                {
                    res = cur;
                    cur = cur->left_;
                }
                else
                {
                    cur = cur->right_;
                }
            }
            return const_iterator(res);
        }

        // emplace 接口（不支持重复插入）
        template <typename... Args>
        std::pair<iterator, bool> emplace_unique(Args &&...args)
        {
            auto p = this->template insert_impl<true>(std::forward<Args>(args)...);
            if (p.second == true)
            {
                ++size_;
            }
            return {iterator(p.first), p.second};
        }

        // emplace 接口（支持重复插入）
        template <typename... Args>
        iterator emplace_duplicate(Args &&...args)
        {
            ++size_;
            return iterator(this->template insert_impl<false>(std::forward<Args>(args)...));
        }

        // 查找
        iterator find(const K &val) const
        {
            return iterator(this->find_impl(val));
        }

        // 删除接口
        iterator erase(const_iterator pos)
        {
            iterator next = iterator(pos.node_);
            ++next;
            // 计算中序后继，作为返回值
            Node *target = pos.node_; // 待删除节点
            Node *successor = next.node_;
            successor = this->erase_impl(target, successor);
            --size_;
            return iterator(successor);
        }
        iterator erase(const_iterator first, const_iterator last)
        {
            // 如果删除整个树，直接 clear
            if (first == begin() && last == end())
            {
                clear();
                return end();
            }

            // 否则逐节点删除，因为删除存在替换删除逻辑
            // 如果last被替换，就会出现一直first!=last的情况
            const_iterator ret = const_iterator(last.node_);
            while (first != last)
            {
                const_iterator cur = first++;
                ret = erase(cur);
                if (ret != first && first != last)
                {
                    first = ret;
                }
            }
            return iterator(ret.node_); // 返回下一个有效迭代器
        }
        size_t erase(const K &key)
        {
            const_iterator iter = find(key);
            if (iter == end())
                return 0;
            erase(iter);
            return 1;
        }

        // 获取有效数据个数
        size_t size() const
        {
            return size_;
        }

        // 判断是否为空
        bool empty() const
        {
            return size_ == 0;
        }

        void swap(RBTree &rb_tree)
        {
            zstl::swap(this->header_, rb_tree.header_);
            zstl::swap(size_, rb_tree.size_);
            zstl::swap(this->alloc_, rb_tree.alloc_);
            zstl::swap(this->node_alloc_, rb_tree.node_alloc_);
        }

        // 清空节点
        void clear()
        {
            destroy(this->header_->parent_);
            this->header_->parent_ = nullptr;
            this->header_->left_ = this->header_;
            this->header_->right_ = this->header_;
            size_ = 0;
        }

        ~RBTree()
        {
            if (this->header_)
            {
                clear();
                this->destroy_node(this->header_);
            }
        }

    private:
        // 销毁除header之外的节点
        void destroy(Node *&root)
        {
            if (root == nullptr)
            {
                return;
            }
            // 递归销毁左子树
            destroy(root->left_);
            // 递归销毁右子树
            destroy(root->right_);
            // 销毁根节点
            this->destroy_node(root);
            root = nullptr;
        }

        Node *copy(Node *root)
        {
            // 如果原始节点为空，直接返回空指针
            if (root == nullptr)
            {
                return nullptr;
            }
            // 为新节点分配内存并拷贝原始节点的值
            Node *newnode = this->create_node(root->data_);
            // 递归拷贝左子树
            newnode->left_ = copy(root->left_);
            // 递归拷贝右子树
            newnode->right_ = copy(root->right_);
            // 将新节点的父节点指针置为空
            newnode->parent_ = nullptr;
            // 拷贝原始节点的颜色信息
            newnode->col_ = root->col_;

            // 如果新节点的左子节点存在，设置其父节点为新节点
            if (newnode->left_)
            {
                newnode->left_->parent_ = newnode;
            }
            // 如果新节点的右子节点存在，设置其父节点为新节点
            if (newnode->right_)
            {
                newnode->right_->parent_ = newnode;
            }
            return newnode;
        }

        // 初始化头节点
        void init_header()
        {
            this->header_ = this->create_node(T());
            this->header_->col_ = Color::RED;
            this->header_->left_ = this->header_;
            this->header_->right_ = this->header_;
            this->header_->parent_ = nullptr;
            size_ = 0;
        }

        // 辅助函数：调整header_的极值指针
        void adjust_header_pointers(Node *root)
        {
            if (root == nullptr)
            {
                this->header_->left_ = this->header_;
                this->header_->right_ = this->header_;
                return;
            }
            // 查找最小节点（最左节点）
            Node *min_node = root;
            while (min_node->left_)
            {
                min_node = min_node->left_;
            }
            this->header_->left_ = min_node;

            // 查找最大节点（最右节点）
            Node *max_node = root;
            while (max_node->right_)
            {
                max_node = max_node->right_;
            }
            this->header_->right_ = max_node;
            this->header_->parent_->parent_ = this->header_;
        }

    private:
        size_t size_; // 节点数量
    };
};
