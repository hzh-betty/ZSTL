#pragma once
#include "assoc_hash.hpp"
#include <functional>

namespace zstl
{
    template <typename K, typename V, typename Hash = std::hash<K>, typename Compare = std::equal_to<K>>
    using unordered_map = assoc_hash<K, V, Hash, Compare, true>;

    template <typename K, typename V, typename Hash = std::hash<K>, typename Compare = std::equal_to<K>>
    using unordered_multimap = assoc_hash<K, V, Hash, Compare, false>;
};