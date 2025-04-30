#pragma once
#include <iostream>
#include <utility>
#include "vector.hpp"

namespace zstl
{

    // 哈希节点，用于链表存储冲突的元素
    template <typename T>
    struct HashNode
    {
        T data_;         // 存储的数据
        HashNode *next_; // 指向下一个节点

        HashNode(const T &data)
            : data_(data), next_(nullptr) {}
        HashNode(T &&data)
            : data_(std::move(data)), next_(nullptr) {}
    };

    // 前向声明哈希表模板
    template <typename K, typename T, typename Hash, typename Compare>
    class HashTable;

    // 哈希表迭代器，用于遍历所有元素
    template <typename Key, typename Value, typename Ref, typename Ptr,
              typename HashFunc, typename CompareFunc>
    struct HashTableIterator
    {
        using Node = HashNode<Value>;
        using HT = HashTable<Key, Value, HashFunc, CompareFunc>;
        using Self = HashTableIterator<Key, Value, Ref, Ptr, HashFunc, CompareFunc>;

        Node *node_;   // 当前节点
        const HT *ht_; // 所属哈希表

        HashTableIterator(Node *node, const HT *ht)
            : node_(node), ht_(ht) {}

        // 允许不同类型的迭代器转换
        HashTableIterator(const HashTableIterator<Key, Value, Value &, Value *, HashFunc, CompareFunc> &it)
            : node_(it.node_), ht_(it.ht_) {}

        // 获取节点数据
        Ref operator*() { return node_->data_; }
        Ptr operator->() { return &node_->data_; }

        // 前置++：移动到下一个节点或下一个非空桶
        Self &operator++()
        {
            HashFunc hash;
            CompareFunc com;
            if (node_ && node_->next_)
            {
                node_ = node_->next_;
            }
            else if (node_)
            {
                // 当前链表结束，查找下一个非空桶
                size_t bucket_num = ht_->tables_.size();
                size_t index = hash(com(node_->data_)) % bucket_num;
                ++index;
                while (index < bucket_num)
                {
                    if (ht_->tables_[index])
                    {
                        node_ = ht_->tables_[index];
                        break;
                    }
                    ++index;
                }
                if (index == bucket_num)
                    node_ = nullptr; // 结束
            }
            return *this;
        }

        // 后置++：保留旧状态
        Self operator++(int)
        {
            Self tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator!=(const Self &s) const { return node_ != s.node_; }
        bool operator==(const Self &s) const { return node_ == s.node_; }
    };

    // 哈希表主体
    template <typename K, typename T, typename Hash, typename Compare>
    class HashTable
    {
        template <typename Key, typename Value, typename Ref, typename Ptr,
                  typename HashFunc, typename CompareFunc>
        friend struct HashTableIterator;
        using Node = HashNode<T>;

    public:
        using iterator = HashTableIterator<K, T, T &, T *, Hash, Compare>;
        using const_iterator = HashTableIterator<K, T, const T &, const T *, Hash, Compare>;

        // 返回第一个元素与末尾的迭代器
        iterator begin()
        {
            Node *cur = nullptr;
            for (int i = 0; i < tables_.size(); ++i)
            {
                if (tables_[i])
                {
                    cur = tables_[i];
                    break;
                }
            }
            return iterator(cur, this);
        }
        iterator end() { return iterator(nullptr, this); }

        // 返回第一个元素与末尾的迭代器
        const_iterator begin() const
        {
            Node *cur = nullptr;
            for (int i = 0; i < tables_.size(); ++i)
            {
                if (tables_[i])
                {
                    cur = tables_[i];
                    break;
                }
            }
            return const_iterator(cur, this);
        }
        const_iterator end() const { return const_iterator(nullptr, this); }

        // 默认构造与析构
        HashTable() = default;
        ~HashTable() { clear(); }
        // 拷贝构造：按桶复制节点
        HashTable(const HashTable &ht)
        {
            tables_.resize(ht.tables_.size());
            for (size_t i = 0; i < tables_.size(); i++)
            {
                Node *cur = ht.tables_[i];
                while (cur)
                {
                    Node *copy = new Node(cur->data_);
                    copy->next_ = tables_[i];
                    tables_[i] = copy;
                    cur = cur->next_;
                }
            }
            size_ = ht.size_;
        }
        // 赋值：交换
        HashTable &operator=(const HashTable& ht)
        {
            if(this != &ht)
            {
                swap(ht);
            }
            return *this;
        }

        // 移动构造函数
        HashTable(HashTable &&ht) noexcept
            : tables_(std::move(ht.tables_)), size_(ht.size_)
        {
            ht.size_ = 0;
        }
        // 移动赋值运算符
        HashTable &operator=(HashTable &&ht) noexcept
        {
            if (this != &ht)
            {
                clear();
                tables_ = std::move(ht.tables_);
                size_ = ht.size_;
                ht.size_ = 0;
            }
            return *this;
        }
        // 查找元素
        iterator find(const K &key) const
        {
            if (tables_.empty())
                return iterator(nullptr, this);

            Hash hash;
            Compare com;
            size_t index = hash(key) % tables_.size();
            Node *cur = tables_[index];
            while (cur)
            {
                if (com(cur->data_, key))
                    return iterator(cur, this);
                cur = cur->next_;
            }
            return iterator(nullptr, this);
        }

        // 插入元素，返回迭代器与是否成功
        std::pair<iterator, bool> insert_unique(const T &data)
        {
            Hash hash;
            Compare com;
            // 已存在则不插入
            auto it_pair = find(com(data));
            if (it_pair != end())
                return {it_pair, false};

            // 负载因子 >= 1 时扩容
            if (size_ == tables_.size())
            {
                rehash();
            }
            // 插入到头部
            size_t index = hash(com(data)) % tables_.size();
            Node *new_node = new Node(data);
            new_node->next_ = tables_[index];
            tables_[index] = new_node;
            ++size_;
            return {iterator(new_node, this), true};
        }
        iterator insert_duplicate(const T &data)
        {
            Hash hash;
            Compare com;
            // 负载因子 >= 1 时扩容
            if (size_ == tables_.size())
            {
                rehash();
            }
            // 插入到头部
            size_t index = hash(com(data)) % tables_.size();
            Node *new_node = new Node(data);
            Node *cur = tables_[index];
            Node *prev = nullptr; // 尾插
            while (cur)
            {
                if (com(cur->data_, data))
                {
                    break;
                }
                prev = cur;
                cur = cur->next_;
            }

            if (prev == nullptr)
            {
                new_node->next_ = tables_[index];
                tables_[index] = new_node;
            }
            else
            {
                prev->next_ = new_node;
                new_node->next_ = cur;
            }

            ++size_;
            return iterator(new_node, this);
        }

        // emplace 接口（唯一插入）
        template <typename... Args>
        std::pair<iterator, bool> emplace_unique(Args &&...args)
        {
            T data(std::forward<Args>(args)...);
            Hash hash;
            Compare com;
            // 已存在则不插入
            auto it_pair = find(com(data));
            if (it_pair != end())
                return {it_pair, false};

            // 负载因子 >= 1 时扩容
            if (size_ == tables_.size())
            {
                rehash();
            }
            // 插入到头部
            size_t index = hash(com(data)) % tables_.size();
            Node *new_node = new Node(data);
            new_node->next_ = tables_[index];
            tables_[index] = new_node;
            ++size_;
            return {iterator(new_node, this), true};
        }

        // emplace 接口（重复插入）
        template <typename... Args>
        iterator emplace_duplicate(Args &&...args)
        {
            T data(std::forward<Args>(args)...);
            Hash hash;
            Compare com;
            // 负载因子 >= 1 时扩容
            if (size_ == tables_.size())
            {
                rehash();
            }
            // 插入到头部
            size_t index = hash(com(data)) % tables_.size();
            Node *new_node = new Node(data);
            Node *cur = tables_[index];
            Node *prev = nullptr; // 尾插
            while (cur)
            {
                if (com(cur->data_, data))
                {
                    break;
                }
                prev = cur;
                cur = cur->next_;
            }

            if (prev == nullptr)
            {
                new_node->next_ = tables_[index];
                tables_[index] = new_node;
            }
            else
            {
                prev->next_ = new_node;
                new_node->next_ = cur;
            }

            ++size_;
            return iterator(new_node, this);
        }

        // 删除元素，返回是否成功
        iterator erase(const_iterator pos)
        {
            // 1. 如果不存在
            if (pos.node_ == nullptr || tables_.empty())
            {
                return iterator(nullptr, this);
            }

            // 2. 如果存在
            Hash hash;
            Compare com;
            size_t index = hash(com(*pos)) % tables_.size();
            const_iterator tmp = pos;
            ++tmp;
            iterator next_it(tmp.node_, this); // 找到下一个迭代器
            Node *prev = nullptr;
            Node *cur = tables_[index];
            while (cur)
            {
                if (cur == pos.node_)
                {
                    if (prev != nullptr)
                    {
                        prev->next_ = cur->next_;
                    }
                    else
                    {
                        tables_[index] = cur->next_;
                    }
                    delete cur;
                    --size_;
                    break;
                }
                prev = cur;
                cur = cur->next_;
            }
            return next_it;
        }
        iterator erase(const_iterator first, const_iterator last)
        {
            // 如果删除整个树，直接 clear
            if (first == begin() && last == end())
            {
                clear();
                return end();
            }
            const_iterator it = first;
            const_iterator it_end = last;
            while (it != it_end)
            {
                it = erase(it);
            }
            return iterator(it_end.node_, this);
        }
        size_t erase(const K &key)
        {
            const_iterator iter = find(key);
            if (iter == end())
                return 0;
            erase(iter);
            return 1;
        }

        // equal_range: 返回所有与 key 相等的元素范围
        std::pair<iterator, iterator> equal_range(const K &key) const
        {
            // 为空直接返回
            if (tables_.empty())
                return {iterator(nullptr, this), iterator(nullptr, this)};
            Hash hash;
            Compare com;
            size_t index = hash(key) % tables_.size();
            Node *cur = tables_[index];
            Node *first = nullptr;
            Node *last = nullptr;

            // 在链表中查找所有匹配节点
            while (cur)
            {
                if (com(cur->data_, key))
                {
                    if (first == nullptr)
                    {
                        first = cur;
                    }
                    last = cur;
                }
                cur = cur->next_;
            }

            if (first == nullptr)
            {
                return {iterator(nullptr, this), iterator(nullptr, this)};
            }

            iterator it_first(first, this);
            iterator it_last(last, this);
            ++it_last;
            return {it_first, it_last};
        }

        bool empty() const { return size_ == 0; }
        size_t size() const { return size_; }

        // 清空所有节点
        void clear()
        {
            for (auto &head : tables_)
            {
                while (head)
                {
                    Node *next = head->next_;
                    delete head;
                    head = next;
                }
            }
            size_ = 0;
        }

        // 交换两表数据
        void swap(HashTable &ht)
        {
            tables_.swap(ht.tables_);
            std::swap(size_, ht.size_);
        }

        // 扩容并重新哈希所有元素
        void rehash()
        {
            size_t old_bucket = tables_.size();
            size_t new_bucket = get_next_prime(old_bucket);
            vector<Node *> new_tables;
            new_tables.resize(new_bucket);

            Hash hash;
            Compare com;
            // 转移节点
            for (size_t i = 0; i < old_bucket; ++i)
            {
                Node *cur = tables_[i];
                while (cur)
                {
                    Node *next = cur->next_;
                    size_t idx = hash(com(cur->data_)) % new_bucket;
                    cur->next_ = new_tables[idx];
                    new_tables[idx] = cur;
                    cur = next;
                }
            }
            tables_.swap(new_tables);
        }

    private:
        // 查找下一个素数，用于扩容
        size_t get_next_prime(size_t prime)
        {
            static constexpr size_t PRIMECOUNT = 28;
            static const size_t primeList[PRIMECOUNT] = {53ul, 97ul, 193ul, 389ul, 769ul,
                                                         1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
                                                         49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
                                                         1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
                                                         50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
                                                         1610612741ul, 3221225473ul, 4294967291ul};
            for (size_t p : primeList)
            {
                if (p > prime)
                    return p;
            }
            return primeList[PRIMECOUNT - 1];
        }

    private:
        vector<Node *> tables_; // 桶数组
        size_t size_ = 0;       // 元素计数
    };

} // namespace zstl
