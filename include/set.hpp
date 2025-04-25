#pragma once
#include "rb_tree.hpp"
namespace zstl
{
    // 比较仿函数
    template <typename K>
    struct SetCompare
    {
        bool operator()(const K &key1, const K &key2)
        {
            return key1 < key2;
        }

        const K &operator()(const K &key1)
        {
            return key1;
        }
    };

    template <typename K, typename Compare = SetCompare<K>>
    class set
    {
    public:
        // 保证K都不能修改，此时就需要支持iterator转换为const_iterator
        using iterator = typename RBTree<K, K, Compare>::const_iterator;
        using const_iterator = typename RBTree<K, K, Compare>::const_iterator;

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

        std::pair<iterator, bool> insert(const K &val)
        {
            return rb_tree_.insert_unique(val);
        }

        iterator erase(const_iterator pos)
        {
            return rb_tree_.erase(pos);
        }
        
        size_t erase(const K &key)
        {
            return rb_tree_.erase(key);
        }

        iterator find(const K &key) const
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

    private:
        RBTree<K, K, Compare> rb_tree_; // K不能修改
    };

    /* 支持键值冗余的multiset */
    template <typename K, typename Compare = SetCompare<K>>
    class multiset
    {
    public:
        // 保证K都不能修改，此时就需要支持iterator转换为const_iterator
        using iterator = typename RBTree<K, K, Compare>::const_iterator;
        using const_iterator = typename RBTree<K, K, Compare>::const_iterator;

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

        iterator insert(const K &val)
        {
            return rb_tree_.insert_duplicate(val).first;
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

        iterator erase(const_iterator pos)
        {
            return rb_tree_.erase(pos);
        }

        iterator erase(const_iterator begin, const_iterator end)
        {
            return rb_tree_.erase(begin, end);
        }

        // 删除全部值为Key的元素
        size_t erase(const K &key)
        {
            auto p = equal_range(key);
            const size_t old_size = size();
            erase(p.first, p.second);
            return old_size - size();
        }

        iterator find(const K &key) const
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

    private:
        RBTree<K, K, Compare> rb_tree_; // K不能修改
    };

};