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

        iterator lower_bound(const K &k)
        {
            return rb_tree_.lower_bound(k);
        }
        const_iterator lower_bound(const K &k) const
        {
            return rb_tree_.lower_bound(k);
        }
        iterator upper_bound(const K &k)
        {
            return rb_tree_.upper_bound(k);
        }
        const_iterator upper_bound(const K &k) const
        {
            return rb_tree_.upper_bound(k);
        }

        std::pair<iterator, iterator> equal_range(const K &k)
        {
            return {lower_bound(k), upper_bound(k)};
        }

        std::pair<const_iterator, const_iterator> equal_range(const K &k) const
        {
            return {lower_bound(k), upper_bound(k)};
        }

        iterator erase(const_iterator begin, const_iterator end)
        {
            return rb_tree_.erase(begin, end);
        }

        iterator erase(const_iterator pos)
        {
            return rb_tree_.erase(pos);
        }

        size_t erase(const K &key)
        {
            return rb_tree_.erase(key);
        }

        std::pair<iterator, bool> insert(const std::pair<K, V> &val)
        {
            return rb_tree_.insert_unique(val);
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

        void clear()
        {
            return rb_tree_.clear();
        }

    private:
        RBTree<K, std::pair<const K, V>, Compare> rb_tree_;
    };

    template <typename K, typename V, typename Compare = MapCompare<K, V>>
    class mutimap
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

        iterator lower_bound(const K &k)
        {
            return rb_tree_.lower_bound(k);
        }
        const_iterator lower_bound(const K &k) const
        {
            return rb_tree_.lower_bound(k);
        }
        iterator upper_bound(const K &k)
        {
            return rb_tree_.upper_bound(k);
        }
        const_iterator upper_bound(const K &k) const
        {
            return rb_tree_.upper_bound(k);
        }

        std::pair<iterator, iterator> equal_range(const K &k)
        {
            return {lower_bound(k), upper_bound(k)};
        }

        std::pair<const_iterator, const_iterator> equal_range(const K &k) const
        {
            return {lower_bound(k), upper_bound(k)};
        }

        iterator erase(const_iterator begin, const_iterator end)
        {
            return rb_tree_.erase(begin, end);
        }

        iterator erase(const_iterator pos)
        {
            return rb_tree_.erase(pos);
        }

        size_t erase(const K &key)
        {
            auto p = equal_range(key);
            const size_t old_size = size();
            erase(p.first, p.second);
            return old_size - size();
        }

        iterator insert(const std::pair<K, V> &val)
        {
            return rb_tree_.insert_duplicate(val).first;
        }

        iterator find(const K &key)
        {
            return rb_tree_.find(key);
        }

        bool empty() const
        {
            return rb_tree_.empty();
        }

        size_t size() const
        {
            return rb_tree_.size();
        }

        void clear()
        {
            return rb_tree_.clear();
        }

    private:
        RBTree<K, std::pair<const K, V>, Compare> rb_tree_;
    };
};
