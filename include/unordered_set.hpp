#pragma once
#include "assoc_hash.hpp"
namespace zstl
{
    template <typename K>
    struct USetCompare
    {
        const K &operator()(const K &key)
        {
            return key;
        }

        bool operator()(const K &key1, const K &key2)
        {
            return key1 == key2;
        }
    };

    template <typename K>
    struct SetHashFunc
    {
        size_t operator()(const K &key)
        {
            return key;
        }
    };
    template <>
    struct SetHashFunc<string>
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
    template <typename K, typename Hash = SetHashFunc<K>, typename Compare = USetCompare<K>>
    using unordered_set = assoc_hash<K, hash_null_type, Hash, Compare, true>;

    template <typename K, typename Hash = SetHashFunc<K>, typename Compare = USetCompare<K>>
    using unordered_multiset = assoc_hash<K, hash_null_type, Hash, Compare, false>;
};