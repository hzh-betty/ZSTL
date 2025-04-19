#include "rb_tree.hpp"
namespace zstl
{
    template <typename K, typename V>
    class map
    {
        struct MapKetOfT
        {
            const K &operator()(const std::pair<K, V> &kv)
            {
                return kv.first;
            }
        };

    public:
    private:
        RBTree<K, V, MapKetOfT> rb_tree_;
    };
};
