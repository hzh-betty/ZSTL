#pragma once
#include "assoc_tree.hpp"
#include "../allocator/alloc.hpp"
#include <functional>
namespace zstl
{

    template <typename K, typename V, typename Compare = std::less<K>, typename Alloc = alloc<std::pair<const K, V>>>
    using map = assoc_tree<K, V, Compare, Alloc, true>;

    template <typename K, typename V, typename Compare = std::less<K>, typename Alloc = alloc<std::pair<const K, V>>>
    using multimap = assoc_tree<K, V, Compare, Alloc, false>;

};
