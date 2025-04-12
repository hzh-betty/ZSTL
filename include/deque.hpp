#pragma once
#include <iostream>
namespace zstl
{
    template <typename T, typename Ptr, typename Ref, size_t BufferSize>
    struct DequeIterator
    {
    };

    template <typename T, size_t BufferSize = 25>
    class deque
    {
    private:
        using pointer = T *;
        using map_pointer = pointer *;
    public:
        using iteraotr = DequeIterator<T, T *, T &, BufferSize>;
        using const_iteraotr = DequeIterator<T, const T *, const T &, BufferSize>;

    private:
        iteraotr start_;  // 开始元素
        iteraotr end_;    // 最后一个元素的下一个
        map_pointer map_; // 中控数组
        size_t map_size_; // 数组个数
    };
};
