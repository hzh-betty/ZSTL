#pragma once
#include <iostream>
#include <cassert>
#include <initializer_list>
#include "../iterator/reverse_iterator.hpp"
#include "../allocator/alloc.hpp"
#include "../allocator/memory.hpp"

namespace zstl
{
    template <typename T, typename Alloc = alloc<T>>
    class vector
    {
        static_assert(!std::is_same_v<bool, T>, "vector<bool> is abandoned in mystl");

    public:
        using allocator_type = Alloc;
        using traits_allocator = allocator_traits<allocator_type>;
        using value_type = T;
        using reference = T &;
        using const_reference = const T &;
        using pointer = typename traits_allocator::pointer;
        using const_pointer = typename traits_allocator::const_pointer;
        using size_type = typename traits_allocator::size_type;
        using difference_type = typename traits_allocator::difference_type;

        // 用原始指针模拟迭代器
        using iterator = pointer;
        using const_iterator = const_pointer;
        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<const_iterator>;

        // 反向迭代器
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // 返回指向起始元素的迭代器
        iterator begin() { return start_; }
        iterator end() { return finish_; }
        const_iterator begin() const { return start_; }
        const_iterator end() const { return finish_; }

    public:
        // 返回当前使用的分配器实例
        allocator_type get_allocator() const noexcept { return alloc_; }

        // 默认构造函数，不做任何初始化
        explicit vector(const allocator_type &alloc = allocator_type())
            : alloc_(alloc), start_(nullptr), finish_(nullptr), end_of_storage_(nullptr) {}

        // 拷贝构造函数：复制另一个 vector 对象
        vector(const vector &v, const allocator_type &alloc)
            : alloc_(alloc)
        {
            // 分配足够的内存存储 v 的所有元素
            reserve(v.size());
            // 逐个调用 push_back 将 v 中的元素复制进来
            for (auto &e : v)
                push_back(e);
        }

        // 拷贝构造函数
        vector(const vector &v)
            : vector(v, v.get_allocator()) {}

        // 初始化列表构造
        vector(std::initializer_list<T> lt, const allocator_type &alloc = allocator_type())
            : alloc_(alloc)
        {
            reserve(lt.size());
            for (auto &e : lt)
                push_back(e);
        }

        // 范围构造函数：根据迭代器 [first, last) 内的元素构造 vector
        template <typename InputIterator>
        vector(InputIterator first, InputIterator last, const allocator_type &alloc = allocator_type())
            : alloc_(alloc)
        {
            while (first != last)
            {
                push_back(*first);
                ++first;
            }
        }

        // 指定大小构造，并用 val 初始化每个元素
        explicit vector(size_type n, const value_type &val = value_type(),
                        const allocator_type &alloc = allocator_type())
            : alloc_(alloc)
        {
            reserve(n);
            for (size_type i = 0; i < n; ++i)
                traits_allocator::construct(alloc_, start_ + i, val);
            finish_ = start_ + n;
        }

        // 移动构造
        vector(vector &&v) noexcept
            : vector(std::move(v), v.get_allocator()) {}

        // 移动构造（带分配器）
        vector(vector &&v, const allocator_type &alloc) noexcept
            : alloc_(alloc), start_(nullptr), finish_(nullptr), end_of_storage_(nullptr)
        {
            // 简化：无状态分配器直接窃取资源
            start_ = v.start_;
            finish_ = v.finish_;
            end_of_storage_ = v.end_of_storage_;
            v.start_ = v.finish_ = v.end_of_storage_ = nullptr;
        }

        // 析构函数，释放 vector 内部申请的内存空间
        ~vector()
        {
            clear();
            if (start_)
                traits_allocator::deallocate(alloc_, start_, capacity());
        }

        // 赋值重载
        vector &operator=(const vector &v)
        {
            if (this != &v)
            {
                vector tmp(v, get_allocator());
                swap(tmp);
            }
            return *this;
        }

        // 移动赋值
        vector &operator=(vector &&v) noexcept
        {
            if (this != &v)
            {
                clear();
                if (start_)
                    traits_allocator::deallocate(alloc_, start_, capacity());
                // 窃取资源
                start_ = v.start_;
                finish_ = v.finish_;
                end_of_storage_ = v.end_of_storage_;
                v.start_ = v.finish_ = v.end_of_storage_ = nullptr;
            }
            return *this;
        }

        // 返回当前 vector 中存储的元素个数
        size_type size() const { return finish_ - start_; }
        // 返回当前分配的内存容量（单位：元素个数）
        size_type capacity() const { return end_of_storage_ - start_; }
        // 判断 vector 是否为空
        bool empty() const { return size() == 0; }

        // 为 vector 分配至少 n 个元素的存储空间（扩容操作）
        void reserve(size_type n)
        {
            if (n > capacity())
            {
                size_type old_size = size();
                // 用 allocator_traits 申请原始内存
                pointer tmp = traits_allocator::allocate(alloc_, n);
                // 将原有元素复制到新的内存中
                for (size_type i = 0; i < old_size; ++i)
                {
                    traits_allocator::construct(alloc_, tmp + i, std::move(start_[i]));
                }
                // 释放旧内存
                traits_allocator::deallocate(alloc_, start_, capacity());
                // 更新内部指针
                start_ = tmp;
                finish_ = start_ + old_size;
                end_of_storage_ = start_ + n;
            }
        }

        // 改变 vector 的大小为 n，若扩容则新增元素使用 val 初始化
        void resize(size_type n, const value_type &val = value_type())
        {
            if (n < size())
            {
                // 缩容时，销毁多余元素
                for (auto it = start_ + n; it != finish_; ++it)
                    traits_allocator::destroy(alloc_, it);
                finish_ = start_ + n;
            }
            else
            {
                reserve(n);
                // 构造新增元素
                while (finish_ != start_ + n)
                {
                    traits_allocator::construct(alloc_, finish_, val);
                    ++finish_;
                }
            }
        }

        // 在 vector 尾部添加一个新元素
        void push_back(const value_type &val)
        {
            insert(end(), val);
        }

        // 移除 vector 尾部的元素
        void pop_back()
        {
            assert(!empty());
            --finish_;
            traits_allocator::destroy(alloc_, finish_);
        }

        // emplace_back 接口
        template <typename... Args>
        void emplace_back(Args &&...args)
        {
            if (finish_ == end_of_storage_)
            {
                size_type new_cap = capacity() ? capacity() * 2 : 4;
                reserve(new_cap);
            }
            traits_allocator::construct(alloc_, finish_, std::forward<Args>(args)...);
            ++finish_;
        }

        // 在指定位置 pos 插入元素 val
        iterator insert(iterator pos, const value_type &val)
        {
            assert(pos >= start_ && pos <= finish_);
            size_type idx = pos - start_;
            if (finish_ == end_of_storage_)
            {
                size_type new_cap = capacity() ? capacity() * 2 : 4;
                reserve(new_cap);
            }
            pos = start_ + idx;
            // 移动元素
            for (iterator it = finish_; it > pos; --it)
            {
                traits_allocator::construct(alloc_, it, std::move(*(it - 1)));
            }
            // 插入新元素
            traits_allocator::construct(alloc_, pos, val);
            ++finish_;
            return pos;
        }

        // 删除指定位置 pos 的元素，并返回删除位置
        iterator erase(iterator pos)
        {
            assert(pos >= start_ && pos < finish_);
            // 销毁 pos
            traits_allocator::destroy(alloc_, pos);
            // 移动后续元素覆盖
            for (iterator it = pos; it < finish_ - 1; ++it)
            {
                traits_allocator::construct(alloc_, it, std::move(*(it + 1)));
            }
            --finish_;
            return pos;
        }

        // 获取首尾元素
        reference front() { return start_[0]; }
        reference back() { return *(finish_ - 1); }
        const_reference front() const { return start_[0]; }
        const_reference back() const { return *(finish_ - 1); }

        // 重载 [] 运算符，提供对元素的可读写访问
        reference operator[](size_type pos)
        {
            assert(pos < size());
            return start_[pos];
        }
        const_reference operator[](size_type pos) const
        {
            assert(pos < size());
            return start_[pos];
        }

        // 交换两个 vector 内部数据的指针，效率高，不需要复制元素
        void swap(vector &v)
        {
            std::swap(start_, v.start_);
            std::swap(finish_, v.finish_);
            std::swap(end_of_storage_, v.end_of_storage_);
        }

        // 清空 vector 中的所有元素（不释放内存，仅重置结束指针）
        void clear()
        {
            // 销毁所有元素
            for (iterator it = start_; it != finish_; ++it)
                traits_allocator::destroy(alloc_, it);
            finish_ = start_;
        }

    private:
        allocator_type alloc_;
        iterator start_ = nullptr;
        iterator finish_ = nullptr;
        iterator end_of_storage_ = nullptr;
    };

}; // namespace zstl
