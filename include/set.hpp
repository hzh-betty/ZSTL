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

        const K& operator()(const K &key1)
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

        std::pair<iterator, bool> insert(const K &val)
        {
            return rb_tree_.insert_unique(val);
        }

        size_t erase(const K &key)
        {
            return rb_tree_.erase(key) ? 1 : 0;
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

        void erase(iterator pos)
        {
            rb_tree_.erase(pos);
        }

        // 删除全部值为Key的元素
        size_t erase(const K &key)
        {
            size_t count = 0;
            iterator iter = find(key);
            while (iter != end() && *iter == key)
            {
                iter = rb_tree_.erase(iter);
                ++count;
            }
            return count;
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