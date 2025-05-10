#pragma once
#include "assoc_tree.hpp"
#include <functional>
namespace zstl
{
    template <typename K, typename Compare = std::less<K>>
    using set = assoc_tree<K, tree_null_type, Compare, true>;

    template <typename K, typename Compare = std::less<K>>
    using multiset = assoc_tree<K, tree_null_type, Compare, false>;
};