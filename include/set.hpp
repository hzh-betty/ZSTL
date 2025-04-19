#include "rb_tree.hpp"
namespace zstl
{
    template <typename K>
    class set
    {
        struct SetKeyOfT
        {
            const K &operator()(const K &key)
            {
                return key;
            }
        };

    public:
    private:
        RBTree<K, K, SetKeyOfT> rb_tree_;
    };
};