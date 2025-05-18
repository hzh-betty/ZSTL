#pragma once
#include <cassert>
#include <stdexcept>
#include <initializer_list>
#include "../iterator/reverse_iterator.hpp"

namespace zstl
{
    // 固定大小的数组容器，模拟 std::array
    template <typename T, size_t N>
    class array
    {
    public:
        using iterator = T *;             // 可写迭代器
        using const_iterator = const T *; // 只读迭代器

        // 反向迭代器
        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<iterator>;

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // 返回指向第一个元素的迭代器
        iterator begin() noexcept { return data_; }
        const_iterator begin() const noexcept { return data_; }

        // 返回指向末尾后位置的迭代器
        iterator end() noexcept { return data_ + N; }
        const_iterator end() const noexcept { return data_ + N; }

    public:
        // 访问第一个元素
        T &front() { return data_[0]; }
        const T &front() const { return data_[0]; }

        // 访问最后一个元素
        T &back() { return data_[N - 1]; }
        const T &back() const { return data_[N - 1]; }

        // 返回底层原始指针
        T *data() noexcept { return data_; }
        const T *data() const noexcept { return data_; }

        // 带范围检查的元素访问，越界抛出异常
        T &at(size_t pos)
        {
            if (pos >= N)
                throw std::out_of_range("array:at");
            return data_[pos];
        }
        const T &at(size_t pos) const
        {
            if (pos >= N)
                throw std::out_of_range("array:at");
            return data_[pos];
        }

        // 无范围检查的下标访问，调试模式下断言
        T &operator[](size_t pos)
        {
            assert(pos < N);
            return data_[pos];
        }
        const T &operator[](size_t pos) const
        {
            assert(pos < N);
            return data_[pos];
        }

        // 将所有元素设为相同值
        void fill(const T &val)
        {
            for (size_t i = 0; i < N; ++i)
            {
                data_[i] = val;
            }
        }

        array() = default;
        array(const array &) = default;
        array &operator=(const array &) = default;
        ~array() = default;

        array(std::initializer_list<T> il)
        {
            size_t i = 0;
            assert(il.size() <= N);
            for (auto &e : il)
            {
                data_[i++] = e;
            }
        }
        // 返回数组大小
        constexpr size_t max_size() const noexcept { return N; }
        // 判断数组是否为空（N==0）
        constexpr bool empty() const noexcept { return N == 0; }

    private:
        T data_[N]; // 静态分配的存储空间
    };

} // namespace zstl
