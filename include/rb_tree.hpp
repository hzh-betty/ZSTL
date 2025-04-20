#pragma once
#include <utility>
#include <iostream>
namespace zstl
{
    enum class Color
    {
        RED,  // 红色
        BLACK // 黑色
    };

    template <typename T>
    struct RBNode
    {
        RBNode(const T &data = T(), Color col = Color::RED)
            : left_(nullptr), right_(nullptr), parent_(nullptr),
              data_(data), col_(col) // 默认为红节点
        {
        }
        RBNode<T> *left_;   // 左子树
        RBNode<T> *right_;  // 右子树
        RBNode<T> *parent_; // 父节点
        T data_;            // 键值
        Color col_;         // 颜色
    };

    template <typename T, typename Ref, typename Ptr>
    struct RBTreeIterator
    {
        using Node = RBNode<T>;
        using Self = RBTreeIterator<T, Ref, Ptr>;
        RBTreeIterator(Node *node)
            : node_(node)
        {
        }
        // 普通迭代器构造const迭代器
        RBTreeIterator(const RBTreeIterator<T, T &, T *> &it)
            : node_(it.node_)
        {
        }
        Ref operator*()
        {
            return node_->data_;
        }

        Ptr operator->()
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
        Self &operator++(int)
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
        Self &operator--(int)
        {
            Self tmp(*this);
            --(*this);
            return tmp;
        }

        Node *node_; // 节点
    };

    template <typename K, typename T, typename KeyOfT>
    class RBTree
    {
        using Node = RBNode<T>;

    public:
        using iterator = RBTreeIterator<T, T &, T *>;
        using const_iterator = RBTreeIterator<T, const T &, const T *>;

        iterator begin()
        {
            return header_->left_;
        }
        const_iterator begin() const
        {
            return header_->left_;
        }

        iterator end()
        {
            return header_;
        }
        const_iterator end() const
        {
            return header_;
        }

    public:
        RBTree()
        {
            // 初始化header__节点
            init_header();
        }

        RBTree(const RBTree<K, T, KeyOfT> &t)
        {
            // 拷贝构造时重建header结构
            init_header();
            header_->parent_ = copy(t.header_->parent_);
            adjust_header_pointers(header_->parent_);
        }

        RBTree<K, T, KeyOfT> &operator=(const RBTree<K, T, KeyOfT> &t)
        {
            if (this != &t)
            {
                clear();
                header_->parent_ = copy(t.header_->parent_);
                adjust_header_pointers(header_->parent_);
            }
            return *this;
        }
        std::pair<iterator, bool> insert(const T &data)
        {
            // 情况一：如果是根节点
            Node *newnode = nullptr;
            if (header_->parent_ == nullptr)
            {
                header_->parent_ = new Node(data);
                header_->parent_->col_ = Color::BLACK;
                header_->parent_->parent_ = header_;
                return std::make_pair(iterator(header_->parent_), true);
            }
            else
            {
                KeyOfT kot;
                Node *cur = header_->parent_;
                Node *parent = nullptr;
                while (cur)
                {
                    if (kot(cur->data_) < kot(data))
                    {
                        parent = cur;
                        cur = cur->right_;
                    }
                    else if (kot(cur->data_) > kot(data))
                    {
                        parent = cur;
                        cur = cur->left_;
                    }
                    else
                    {
                        return std::make_pair(iterator(cur), false);
                    }
                }
                // 找到插入位置
                cur = new Node(data);
                newnode = cur;
                if (kot(parent->data_) < kot(data))
                {
                    parent->right_ = cur;
                }
                else
                {
                    parent->left_ = cur;
                }
                cur->parent_ = parent;

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
            adjust_header_pointers(header_->parent_);
            // 防止情况三改到根节点变为红色
            if (header_->parent_)
                header_->parent_->col_ = Color::BLACK;
            return std::make_pair(iterator(newnode), true);
        }

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
        void RotateLR(Node *parent)
        {
            RotateL(parent->left_);
            RotateR(parent);
        }
        void RotateRL(Node *parent)
        {
            RotateR(parent->right_);
            RotateL(parent);
        }

        iterator find(const K &val)
        {
            KeyOfT kot;
            Node *cur = header_->parent_;
            while (cur)
            {
                if (kot(cur->data_) > val)
                {
                    // 左子树中查找
                    cur = cur->left_;
                }
                else if (kot(cur->data_) < val)
                {
                    // 右子树中查找
                    cur = cur->right_;
                }
                else
                {
                    // 找到了
                    return iterator(cur);
                }
            }
            return end();
        }

        // 删除函数
        bool erase(const K &key)
        {
            KeyOfT kot;
            Node *cur = header_->parent_;
            Node *parent = nullptr;
            // 用于标记实际的待删除结点及其父结点
            Node *delParent = nullptr;
            Node *delCur = nullptr;
            // 找到删除节点
            while (cur)
            {
                if (key < kot(cur->data_)) // 所给key值小于当前结点的key值
                {
                    // 往该结点的左子树走
                    parent = cur;
                    cur = cur->left_;
                }
                else if (key > kot(cur->data_)) // 所给key值大于当前结点的key值
                {
                    // 往该结点的右子树走
                    parent = cur;
                    cur = cur->right_;
                }
                else
                {

                    if (cur->left_ == nullptr) // 待删除结点的左子树为空
                    {
                        if (cur == header_->parent_) // 待删除结点是根结点
                        {
                            header_->parent_ = header_->parent_->right_; // 让根结点的右子树作为新的根结点
                            if (header_->parent_)
                            {
                                header_->parent_->parent_ = header_;
                                header_->parent_->col_ = Color::BLACK; // 根结点为黑色
                            }
                            delete cur; // 删除原根结点
                            adjust_header_pointers(header_->parent_);
                            return true;
                        }
                        else
                        {
                            delParent = parent; // 标记实际删除结点的父结点
                            delCur = cur;       // 标记实际删除的结点
                        }
                        break;
                    }
                    else if (cur->right_ == nullptr) // 待删除结点的右子树为空
                    {
                        if (cur == header_->parent_) // 待删除结点是根结点
                        {
                            header_->parent_ = header_->parent_->left_; // 让根结点的左子树作为新的根结点
                            if (header_->parent_)
                            {
                                header_->parent_->parent_ = header_;
                                header_->parent_->col_ = Color::BLACK; // 根结点为黑色
                            }
                            delete cur; // 删除原根结点
                            adjust_header_pointers(header_->parent_);
                            return true;
                        }
                        else
                        {
                            delParent = parent; // 标记实际删除结点的父结点
                            delCur = cur;       // 标记实际删除的结点
                        }
                        break;
                    }
                    else // 待删除结点的左右子树均不为空
                    {
                        // 替换法删除
                        // 寻找待删除结点右子树当中key值最小的结点作为实际删除结点
                        Node *minParent = cur;
                        Node *minRight = cur->right_;
                        while (minRight->left_)
                        {
                            minParent = minRight;
                            minRight = minRight->left_;
                        }

                        // 原本直接可以赋值
                        // cur->_value = minRight->_value
                        // 将待删除结点的键值改为minRight的键值
                        Node *newnode = new Node(minRight->data_, cur->col_);
                        Node *parent = cur->parent_;

                        // 重新链接祖父孙三代节点关系
                        cur->left_->parent_ = newnode;
                        cur->right_->parent_ = newnode;
                        if (parent)
                        {
                            if (parent->left_ == cur)
                            {
                                parent->left_ = newnode;
                            }
                            else
                            {
                                parent->right_ = newnode;
                            }
                        }
                        else
                        {
                            // 如果是根节点
                            header_->parent_ = newnode;
                        }
                        newnode->parent_ = parent;
                        newnode->left_ = cur->left_;
                        newnode->right_ = cur->right_;

                        // 如果minParent是cur
                        if (minParent == cur)
                        {
                            minParent = newnode;
                        }

                        delete cur;
                        delParent = minParent; // 标记实际删除的父节点
                        delCur = minRight;     // 标记实际删除的结点
                        break;
                    }
                }
            }

            if (delCur == nullptr)
            {
                return false;
            }

            // 记录待删除结点及其父结点，便于后面删除
            Node *del = delCur;
            Node *delP = delParent;

            //(二)调整红黑树
            adjust_erase(delCur, delParent);

            //(三)进行实际删除
            delete_node(del, delP);

            adjust_header_pointers(header_->parent_);
            return true;
        }

        void delete_node(Node *del, Node *delP)
        {
            if (del->left_ == nullptr) // 实际删除结点的左子树为空
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
            delete del; // 实际删除结点
        }
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

        ~RBTree()
        {
            clear();
            delete header_;
        }

    private:
        void clear()
        {
            destroy(header_->parent_);
            header_->parent_ = nullptr;
            header_->left_ = header_;
            header_->right_ = header_;
        }

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
            delete root;
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
            Node *newnode = new Node(root->data_);
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
            header_ = new Node();
            header_->col_ = Color::RED;
            header_->left_ = header_;
            header_->right_ = header_;
            header_->parent_ = nullptr;
        }

        // 辅助函数：调整header_的极值指针
        void adjust_header_pointers(Node *root)
        {
            if (root == nullptr)
            {
                header_->left_ = header_;
                header_->right_ = header_;
                return;
            }
            // 查找最小节点（最左节点）
            Node *min_node = root;
            while (min_node->left_)
            {
                min_node = min_node->left_;
            }
            header_->left_ = min_node;

            // 查找最大节点（最右节点）
            Node *max_node = root;
            while (max_node->right_)
            {
                max_node = max_node->right_;
            }
            header_->right_ = max_node;
            header_->parent_->parent_ = header_;
        }

    private:
        Node *header_; // 头节点
    };
};
