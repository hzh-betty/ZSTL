#include "rb_tree.hpp"
namespace zstl
{
    template <typename K>
    class set
    {
        // 比较仿函数
        struct SetKeyOfT
        {
            const K &operator()(const K &key)
            {
                return key;
            }
        };

    public:
        using iterator = typename RBTree<K, const K, SetKeyOfT>::iterator;
        using const_iterator = typename RBTree<K, const K, SetKeyOfT>::iterator;
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

    private:
        RBTree<K, const K, SetKeyOfT> rb_tree_; // K不能修改
    };
};