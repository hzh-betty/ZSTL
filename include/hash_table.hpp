#pragma once
#include <iostream>
#include <utility>
#include "vector.hpp"
namespace zstl
{

    /* 哈希节点 */
    template <typename T>
    struct HashNode
    {
        HashNode(const T &data)
            : data_(data), next_(nullptr)
        {
        }
        T data_;         // 键值
        HashNode *next_; // 下一个节点指针
    };

    template <typename K, typename T, typename Hash, typename Compare>
    class HashTable;

    /* 哈希表的迭代器 */
    template <typename Key, typename Value, typename Ref, typename Ptr, typename HashFunc, typename CompareFunc>
    struct HashTableIterator
    {
        using Node = HashNode<Value>;
        using HT = HashTable<Key, Value, HashFunc, CompareFunc>;
        using Self = HashTableIterator<Key, Value, Ref, Ptr, HashFunc, CompareFunc>;

        Node *node_;
        const HT *ht_;

        HashTableIterator(Node *node, const HT *ht)
            : node_(node), ht_(ht)
        {
        }

        HashTableIterator(const HashTableIterator<Key, Value, Value &, Value *, HashFunc, CompareFunc> &it)
            : node_(it.node_), ht_(it.ht_)
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

        Self &operator++()
        {
            HashFunc hash;
            CompareFunc com;
            if (node_->next_ != nullptr)
            {
                node_ = node_->next_;
            }
            else
            {
                // 寻找下一个不为空的桶
                size_t table_size = ht_->tables_.size();
                size_t index = hash(com(node_->data_)) % table_size;
                ++index;
                while (index < table_size)
                {
                    if (ht_->tables_[index] != nullptr)
                    {
                        node_ = ht_->tables_[index];
                        break;
                    }
                    ++index;
                }

                if (index == table_size)
                {
                    node_ = nullptr;
                }
            }

            return *this;
        }

        Self operator++(int)
        {
            Self tmp(*this);
            ++(*this);
            return tmp;
        }

        bool operator!=(const Self &s) const
        {
            return node_ != s.node_;
        }

        bool operator==(const Self &s) const
        {
            return node_ == s.node_;
        }
    };

    /* 哈希表 */
    template <typename K, typename T, typename Hash, typename Compare>
    class HashTable
    {
        template <typename Key, typename Value, typename Ref, typename Ptr, typename HashFunc, typename CompareFunc>
        friend struct HashTableIterator;
        using Node = HashNode<T>;

    public:
        using iterator = HashTableIterator<K, T, T &, T *, Hash, Compare>;
        using const_iterator = HashTableIterator<K, T, const T &, const T *, Hash, Compare>;
        iterator begin()
        {
            Node *cur = nullptr;
            for (int i = 0; i < tables_.size(); ++i)
            {
                if (tables_[i] != nullptr)
                {
                    cur = tables_[i];
                    break;
                }
            }
            return iterator(cur, this);
        }

        iterator end()
        {
            return iterator(nullptr, this);
        }

        const_iterator begin() const
        {
            Node *cur = nullptr;
            for (int i = 0; i < tables_.size(); ++i)
            {
                cur = tables_[i];
                if (cur != nullptr)
                    break;
            }
            return const_iterator(cur, this);
        }

        const_iterator end() const
        {
            return const_iterator(nullptr, this);
        }

    public:
        // 构造
        HashTable() = default;

        // 拷贝
        HashTable(const HashTable &ht)
        {
            // 1. 先扩容
            tables_.resize(ht.tables_.size());

            // 2. 再拷贝
            for (size_t i = 0; i < tables_.size(); i++)
            {
                if (ht.tables_[i] != nullptr)
                {
                    Node *cur = ht.tables_[i];
                    while (cur)
                    {
                        // 进行头插
                        Node *copy = new Node(cur->data_);
                        copy->next = tables_[i];
                        tables_[i] = copy;
                        cur = cur->next_;
                    }
                }
            }

            size_ = ht.size_;
        }

        // 赋值重载
        HashTable &operator=(HashTable &ht)
        {
            if (this != &ht)
            {
                swap(ht);
            }
            return *this;
        }

        size_t get_next_prime(size_t prime)
        {
            static constexpr size_t PRIMECOUNT = 28;
            // 素数序列
            static const size_t primeList[PRIMECOUNT] =
                {
                    53ul, 97ul, 193ul, 389ul, 769ul,
                    1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
                    49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
                    1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
                    50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
                    1610612741ul, 3221225473ul, 4294967291ul};
            size_t i = 0;
            for (i = 0; i < PRIMECOUNT; i++)
            {
                if (primeList[i] > prime)
                    return primeList[i];
            }
            return primeList[i];
        }

        iterator find(const K &key)const
        {
            Hash hash;
            Compare com;
            // 1. 为空就直接返回
            if (tables_.size() == 0)
                return iterator(nullptr, this);

            // 2. 计算哈希索引
            size_t index = hash(key) % tables_.size();
            Node *cur = tables_[index];
            while (cur)
            {
                if (com(cur->data_, key))
                {
                    return iterator(cur, this);
                }
                cur = cur->next_;
            }

            // 3. 没找到
            return iterator(nullptr, this);
        }

        std::pair<iterator, bool> insert(const T &data)
        {
            Hash hash;
            Compare com;
            // 1. 首先判断是否存在
            iterator iter = find(com(data));
            if (iter != end())
            {
                return std::make_pair(iter, false);
            }

            // 2. 判断是否需要扩容，如果负载因子等于1则扩容
            size_t table_size = tables_.size();
            if (size_ == table_size)
            {
                // 计算新的哈希表大小
                vector<Node *> new_tables;
                size_t new_size = get_next_prime(table_size);
                new_tables.resize(new_size);

                // 转移原哈希表数据
                for (int i = 0; i < table_size; ++i)
                {
                    if (tables_[i] != nullptr)
                    {
                        Node *cur = tables_[i];
                        while (cur)
                        {
                            // 记录下一个节点
                            Node *next = cur->next_;
                            size_t index = hash(com(cur->data_)) % new_size;

                            // 头插
                            cur->next_ = new_tables[index];
                            new_tables[index] = cur;

                            cur = next;
                        }

                        // 将原哈希表置空
                        tables_[i] = nullptr;
                    }
                }
                tables_.swap(new_tables);
                table_size = new_size;
            }

            // 3. 新插入节点
            size_t index = hash(com(data)) % table_size;
            Node *new_node = new Node(data);
            new_node->next_ = tables_[index];
            tables_[index] = new_node;
            ++size_;
            iter = iterator(new_node, this);
            return std::make_pair(iter, true);
        }

        bool erase(const K &key)
        {
            Hash hash;
            Compare com;
            // 1. 先判断是否有该节点
            if (tables_.size() == 0)
                return false;

            size_t index = hash(key) % tables_.size();
            Node *prev = nullptr;
            Node *cur = tables_[index];

            // 2. 删除对应节点
            while (cur)
            {
                if (com(cur->data_, key))
                {
                    if (prev == nullptr)
                    {
                        tables_[index] = cur->next_;
                    }
                    else
                    {
                        prev->next_ = cur->next_;
                    }
                    delete cur;
                    --size_;
                    return true;
                }
                prev = cur;
                cur = cur->next_;
            }

            // 没有该节点
            return false;
        }

        // 交换
        void swap(HashTable &ht)
        {
            tables_.swap(ht.tables_);
            std::swap(size_, ht.size_);
        }

        bool empty() const
        {
            return size_ == 0;
        }

        size_t size() const
        {
            return size_;
        }

        void clear()
        {
            for (auto &cur : tables_)
            {
                while (cur)
                {
                    Node *next = cur->next_;
                    delete cur;
                    cur = next;
                }
                cur = nullptr;
            }
            size_ = 0;
        }

        ~HashTable()
        {
            clear();
        }

    private:
        vector<Node *> tables_; // 哈希表
        size_t size_ = 0;       // 有效数据个数
    };

};