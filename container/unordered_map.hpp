#pragma once
#include "assoc_hash.hpp"
#include <functional>
#include "../allocator/alloc.hpp"
namespace zstl
{
    template <typename K, typename V, typename Hash = std::hash<K>, typename Compare = std::equal_to<K>, typename Alloc = alloc<std::pair<const K, V>>>
    using unordered_map = assoc_hash<K, V, Hash, Compare, Alloc, true>;

    template <typename K, typename V, typename Hash = std::hash<K>, typename Compare = std::equal_to<K>, typename Alloc = alloc<std::pair<const K, V>>>
    using unordered_multimap = assoc_hash<K, V, Hash, Compare, Alloc, false>;
};