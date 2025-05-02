#pragma once
#include "assoc_tree.hpp"
namespace zstl
{
    template <typename K>
    struct MapCompare
    {
        bool operator()(const K &k1, const K &k2)const
        {
            return k1 < k2;
        }
    };

    template <typename K, typename V, typename Compare = MapCompare<K>>
    using map = assoc_tree<K, V, Compare, true>;

    template <typename K, typename V, typename Compare = MapCompare<K>>
    using multimap = assoc_tree<K, V, Compare, false>;

};
