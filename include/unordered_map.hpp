#pragma once
#include "assoc_hash.hpp"
#include "string.hpp"
namespace zstl
{
    template <typename K>
    struct UMapCompare
    {
        bool operator()(const K &k1, const K &k2)const
        {
            return k1 == k2;
        }
    };

    template <typename K>
    struct MapHashFunc
    {
        size_t operator()(const K &key)const
        {
            return key;
        }
    };
    template <>
    struct MapHashFunc<string>
    {
        size_t operator()(const string &s)const
        {
            size_t value = 0;
            for (auto ch : s)
            {
                value = value * 131 + ch;
            }
            return value;
        }
    };
    template <typename K, typename V, typename Hash = MapHashFunc<K>, typename Compare = UMapCompare<K>>
    using unordered_map = assoc_hash<K, V, Hash, Compare, true>;

    template <typename K, typename V, typename Hash = MapHashFunc<K>, typename Compare = UMapCompare<K>>
    using unordered_multimap = assoc_hash<K, V, Hash, Compare, false>;
};