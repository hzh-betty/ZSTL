#pragma once
#include "assoc_tree.hpp"
#include<functional>
namespace zstl
{

    template <typename K, typename V, typename Compare = std::less<K>>
    using map = assoc_tree<K, V, Compare, true>;

    template <typename K, typename V, typename Compare = std::less<K>>
    using multimap = assoc_tree<K, V, Compare, false>;

};
