#pragma once
#include "assoc_tree.hpp"
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
    using map = assoc_tree<K, V, Compare, true>;

    template <typename K, typename V, typename Compare = MapCompare<K, V>>
    using multimap = assoc_tree<K, V, Compare, false>;

};
