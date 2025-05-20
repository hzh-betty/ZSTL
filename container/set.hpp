#pragma once
#include "assoc_tree.hpp"
#include "../functor/functional.hpp"
#include "../allocator/alloc.hpp"
namespace zstl
{
    template <typename K, typename Compare = zstl::less<K>, typename Alloc = alloc<K>>
    using set = assoc_tree<K, tree_null_type, Compare, Alloc, true>;

    template <typename K, typename Compare = zstl::less<K>, typename Alloc = alloc<K>>
    using multiset = assoc_tree<K, tree_null_type, Compare, Alloc, false>;
}