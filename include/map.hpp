#pragma once
#include "rb_tree.hpp"
namespace zstl
{
    template <typename K, typename V>
    struct MapCompare
    {
        bool operator()(const std::pair<K, V> &kv1, const std::pair<K, V> &kv2)
        {
            return kv1.first < kv2.first;
        }

        bool operator()(const K &key1, const std::pair<K, V> &kv2)
        {
            return key1 < kv2.first;
        }

        bool operator()(const std::pair<K, V> &kv1, const K &key2)
        {
            return kv1.first < key2;
        }

        const K &operator()(const std::pair<K, V> &kv1)
        {
            return kv1.first;
        }
    };

    template <typename K, typename V, typename Compare = MapCompare<K, V>>
    class map
    {
    public:
        using iterator = typename RBTree<K, std::pair<const K, V>, Compare>::iterator;
        using const_iterator = typename RBTree<K, std::pair<const K, V>, Compare>::const_iterator;
        iterator begin()
        {
            return rb_tree_.begin();
        }

        const_iterator begin() const
        {
            return rb_tree_.begin();
        }

        iterator end()
        {
            return rb_tree_.end();
        }

        const_iterator end() const
        {
            return rb_tree_.end();
        }

        std::pair<iterator, bool> insert(const std::pair<K, V> &val)
        {
            return rb_tree_.insert_unique(val);
        }

        size_t erase(const K &key)
        {
            return rb_tree_.erase(key);
        }

        iterator find(const K &key)
        {
            return rb_tree_.find(key);
        }

        V &operator[](const K &key)
        {
            // 没有则插入，有则返回V
            auto ret = rb_tree_.insert_unique(std::make_pair(key, V()));
            return ret.first->second;
        }

        bool empty() const
        {
            return rb_tree_.empty();
        }

        size_t size() const
        {
            return rb_tree_.size();
        }

    private:
        RBTree<K, std::pair<const K, V>, Compare> rb_tree_;
    };
};
