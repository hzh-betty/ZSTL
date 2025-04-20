#include "rb_tree.hpp"
namespace zstl
{
    template <typename K, typename V>
    class map
    {
        struct MapKeyOfT
        {
            const K &operator()(const std::pair<K, V> &kv)
            {
                return kv.first;
            }
        };

    public:
        using iterator = typename RBTree<K, std::pair<const K, V>, MapKeyOfT>::iterator;
        using const_iterator = typename RBTree<K, std::pair<const K, V>, MapKeyOfT>::const_iterator;
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

        V &operator[](const K &key)
        {
            // 没有则插入，有则返回V
            auto ret = insert(std::make_pair(key, V()));
            return ret.first->second;
        }

    private:
        RBTree<K, std::pair<const K, V>, MapKeyOfT> rb_tree_;
    };
};
