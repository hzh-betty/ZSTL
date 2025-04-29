#pragma once
#include "assoc_tree.hpp"
namespace zstl
{
    // 比较仿函数
    template <typename K>
    struct SetCompare
    {
        bool operator()(const K &key1, const K &key2)
        {
            return key1 < key2;
        }

        const K &operator()(const K &key1)
        {
            return key1;
        }
    };

    template <typename K, typename Compare = SetCompare<K>>
    using set = assoc_tree<K, null_type, Compare, true>;
    
    template <typename K, typename Compare = SetCompare<K>>
    using multiset = assoc_tree<K, null_type, Compare, false>;
};