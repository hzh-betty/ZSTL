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

        set() = default;
        set(const set &) = default;
        set &operator=(const set &) = default;
        ~set() = default;

        // 移动构造函数
        set(set &&other) noexcept
            : rb_tree_(std::move(other.rb_tree_))
        {
        }

        // 移动赋值运算符
        set &operator=(set &&other) noexcept
        {
            if (this != &other)
            {
                swap(other);
                other.clear();
            }
            return *this;
        }

        // emplace 接口
        template <typename... Args>
        std::pair<iterator, bool> emplace(Args &&...args)
        {
            return rb_tree_.emplace_unique(std::forward<Args>(args)...);
        }

        // initializer_list 构造函数
        set(std::initializer_list<K> il)
        {
            for (auto &e : il)
            {
                emplace(std::move(e));
            }
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

        void clear()
        {
            rb_tree_.clear();
        }

        bool empty() const
        {
            return rb_tree_.empty();
        }

        size_t size() const
        {
            return rb_tree_.size();
        }

        void swap(set &s)
        {
            rb_tree_.swap(s.rb_tree_);
        }

        size_t count(const K &key) const
        {
            return find(key) != end() ? 1 : 0;
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
            return rb_tree_.insert_duplicate(val);
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

        multiset() = default;
        multiset(const multiset &) = default;
        multiset &operator=(const multiset &) = default;
        ~multiset() = default;

        // 移动构造函数
        multiset(multiset &&other) noexcept
            : rb_tree_(std::move(other.rb_tree_))
        {
        }

        // initializer_list 构造函数
        multiset(std::initializer_list<K> il)
        {
            for (auto &e : il)
            {
                emplace(std::move(e));
            }
        }

        // 移动赋值运算符
        multiset &operator=(multiset &&other) noexcept
        {
            if (this != &other)
            {
                swap(other);
                other.clear();
            }
            return *this;
        }

        // emplace 接口
        template <typename... Args>
        iterator emplace(Args &&...args)
        {
            return rb_tree_.emplace_duplicate(std::forward<Args>(args)...);
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

        size_t count(const K &key) const
        {
            auto p = equal_range(key);
            const_iterator it_first = p.first;
            const_iterator it_second = p.second;
            size_t cnt = 0;
            while (it_first != it_second)
            {
                ++it_first;
                ++cnt;
            }
            return cnt;
        }
        
        void clear()
        {
            rb_tree_.clear();
        }

        bool empty() const
        {
            return rb_tree_.empty();
        }

        size_t size() const
        {
            return rb_tree_.size();
        }

        void swap(multiset &s)
        {
            rb_tree_.swap(s.rb_tree_);
        }

    private:
        RBTree<K, K, Compare> rb_tree_; // K不能修改
    };
};