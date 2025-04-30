#pragma once
#include "assoc_hash.hpp"
#include "string.hpp"
namespace zstl
{
    template <typename K, typename V>
    struct UMapCompare
    {
        bool operator()(const std::pair<K, V> &kv1, const std::pair<K, V> &kv2)
        {
            return kv1.first == kv2.first;
        }

        bool operator()(const std::pair<K, V> &kv1, const K &key)
        {
            return kv1.first == key;
        }

        bool operator()(const K &key, const std::pair<K, V> &kv2)
        {
            return key == kv2.first;
        }
        const K &operator()(const std::pair<K, V> &kv)
        {
            return kv.first;
        }
    };

    template <typename K>
    struct MapHashFunc
    {
        size_t operator()(const K &key)
        {
            return key;
        }
    };
    template <>
    struct MapHashFunc<string>
    {
        size_t operator()(const string &s)
        {
            size_t value = 0;
            for (auto ch : s)
            {
                value = value * 131 + ch;
            }
            return value;
        }
    };
    template <typename K, typename V, typename Hash = MapHashFunc<K>, typename Compare = UMapCompare<K, V>>
    using unordered_map = assoc_hash<K, V, Hash, Compare, true>;

    template <typename K, typename V, typename Hash = MapHashFunc<K>, typename Compare = UMapCompare<K, V>>
    using unordered_multimap = assoc_hash<K, V, Hash, Compare, false>;
};