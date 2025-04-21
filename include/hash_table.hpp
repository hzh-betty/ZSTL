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
    template <typename K, typename T, typename Ref, typename Ptr, typename Hash, typename Compare>
    struct HashTableIterator
    {
        using Node = HashNode<T>;
        using HT = HashTable<K, T, Hash, Compre>;
        using Self = HashTableIterator<K, T, Ref, Ptr, Hash, Compare>;

        Node *node_;
        const HT *ht_;

        HashTableIterator(Node *node, const HT *ht)
            : node_(node), ht_(ht)
        {
        }

        HashTableIterator(const HashTableIterator<K, T, T &, T *, Hash, Compare> &it)
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

        Self& operator++()
        {
            if(node_->next != nullptr)
            {
                node_ = node_->next_;
            }
            else
            {
                // 寻找下一个不为空的桶
                Hash hash;
                size_t index = hash(node_->data_) % ht_->size();
                ++index;
                while(index < ht_->size())
                {
                    if(ht_->tables_[index]!=nullptr)
                    {
                        node_ = ht_->tables_[index];
                        break;
                    }
                    ++index;
                }
                
                if(index == ht_->size())
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
            return node_ != node_;
        }

        bool operator ==(const Self &s) const
        {
            return node_ == node_;
        }
    };

    /* 哈希表 */
    template <typename K, typename T, typename Hash, typename Compare>
    class HashTable
    {
        using Node = HashNode<T>;
        using iterator = 
    public:
        Node *find(const K &key)
        {
            // 1. 为空就直接返回
            if (tables_.size() == 0)
                return nullptr;

            // 2. 计算哈希索引
            size_t index = key % tables_.size();
            Node *cur = tables_[index];
            while (cur)
            {
                if (cur->kv_.first == key)
                {
                    return cur;
                }
                cur = cur->next_;
            }

            // 3. 没找到
            return nullptr;
        }

        bool insert(const std::pair<K, V> kv)
        {
            // 1. 首先判断是否存在
            Node *ret = find(kv.first);
            if (ret != nullptr)
                return false;

            // 2. 判断是否需要扩容，如果负载因子等于1则扩容
            size_t table_size = tables_.size();
            if (size_ == table_size)
            {
                // 计算新的哈希表大小
                vector<Node *> new_tables;
                size_t new_size = table_size == 0 ? 10 : table_size * 2;
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
                            size_t index = cur->kv_.first % new_size;

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
            size_t index = kv.first % table_size;
            Node *new_node = new Node(kv);
            new_node->next_ = tables_[index];
            tables_[index] = new_node;
            ++size_;
            return true;
        }

        bool erase(const K &key)
        {
            // 1. 首先先找到对应的节点
            if (tables_.size() == 0)
                return false;

            size_t index = key % tables_.size();
            Node *prev = nullptr;
            Node *cur = tables_[index];

            // 2. 删除对应节点
            while (cur)
            {
                if (cur->kv_.first == key)
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

        bool empty() const
        {
            return size_ == 0;
        }

        size_t size() const
        {
            return size_;
        }

        ~HashTable()
        {
            for (auto &cur : _tables)
            {
                while (cur)
                {
                    Node *next = cur->_next;
                    delete cur;
                    cur = next;
                }
                cur = nullptr;
            }

            size_ = 0;
        }

    private:
        vector<Node *> tables_; // 哈希表
        size_t size_ = 0;       // 有效数据个数
    };

};