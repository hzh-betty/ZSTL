#pragma once
#include "assoc_hash.hpp"
#include"../functor/functional.hpp"
#include "../allocator/alloc.hpp"
namespace zstl
{
    template <typename K, typename V, typename Hash = zstl::hash<K>, typename Compare = zstl::equal_to<K>, typename Alloc = alloc<std::pair<const K, V>>>
    using unordered_map = assoc_hash<K, V, Hash, Compare, Alloc, true>;

    template <typename K, typename V, typename Hash = zstl::hash<K>, typename Compare = zstl::equal_to<K>, typename Alloc = alloc<std::pair<const K, V>>>
    using unordered_multimap = assoc_hash<K, V, Hash, Compare, Alloc, false>;
};