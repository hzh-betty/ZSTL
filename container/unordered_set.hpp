#pragma once
#include "assoc_hash.hpp"
#include <functional>
#include "../allocator/alloc.hpp"

namespace zstl
{
    template <typename K, typename Hash = std::hash<K>, typename Compare = std::equal_to<K>, typename Alloc = alloc<K>>
    using unordered_set = assoc_hash<K, hash_null_type, Hash, Compare, Alloc, true>;

    template <typename K, typename Hash = std::hash<K>, typename Compare = std::equal_to<K>, typename Alloc = alloc<K>>
    using unordered_multiset = assoc_hash<K, hash_null_type, Hash, Compare, Alloc, false>;
};