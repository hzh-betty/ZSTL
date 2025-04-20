#include "rb_tree.hpp"
namespace zstl
{
    // 比较仿函数
    template <typename K>
    struct SetCompare
    {
        bool operator()(const K &key1,const K &key2)
        {
            return key1 < key2;
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
            return rb_tree_.insert(val);
        }

        bool erase(const K &key)
        {
            return rb_tree_.erase(key);
        }

        iterator find(const K &key)
        {
            return rb_tree_.find(key);
        }

        bool empty()const
        {
            return rb_tree_.empty();
        }

        size_t size()const
        {
            return rb_tree_.size();
        }
    private:
        RBTree<K, K, Compare> rb_tree_; // K不能修改
    };
};