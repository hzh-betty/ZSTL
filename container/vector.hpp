#pragma once
#include <iostream>
#include <cassert>
#include <initializer_list>
#include "../iterator/reverse_iterator.hpp"
#include "../allocator/alloc.hpp"

namespace zstl
{
    template <typename T, typename Alloc = alloc<T>>
    class vector
    {
        static_assert(!std::is_same_v<bool, T>, "vector<bool> is abandoned in mystl");

    public:
        using allocator_type = Alloc;

        // 用原始指针模拟迭代器
        using iterator = T *;
        using const_iterator = const T *;
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
        // 默认构造函数，不做任何初始化
        vector() = default;

        // 拷贝构造函数：复制另一个 vector 对象
        vector(const vector<T> &v)
        {
            // 分配足够的内存存储 v 的所有元素
            reserve(v.size());
            // 逐个调用 push_back 将 v 中的元素复制进来
            for (auto &e : v)
                push_back(e);
        }

        vector(std::initializer_list<T> lt)
        {
            reserve(lt.size());
            for (auto &e : lt)
                push_back(e);
        }

        // 赋值重载
        vector<T> &operator=(const vector<T> &v)
        {
            if (this != &v)
            {
                vector<T> tmp(v);
                swap(tmp);
            }
            return *this;
        }

        // 范围构造函数：根据迭代器 [first, last) 内的元素构造 vector
        template <typename InputIterator>
        vector(InputIterator first, InputIterator last)
        {
            while (first != last)
            {
                push_back(*first);
                ++first;
            }
        }

        // 构造函数：创建包含 n 个元素，并用 val 初始化每个元素
        vector(size_t n, const T &val = T())
        {
            reserve(n);
            for (size_t i = 0; i < n; ++i)
                push_back(val);
        }

        // 移动构造
        vector(vector &&v) noexcept
            : start_(v.start_), finish_(v.finish_), end_of_storage_(v.end_of_storage_)
        {
            v.start_ = v.finish_ = v.end_of_storage_ = nullptr;
        }

        // 移动赋值
        vector &operator=(vector &&v) noexcept
        {
            if (this != &v)
            {
                clear();
                allocator_type::deallocate(start_, capacity());
                start_ = v.start_;
                finish_ = v.finish_;
                end_of_storage_ = v.end_of_storage_;
                v.start_ = v.finish_ = v.end_of_storage_ = nullptr;
            }
            return *this;
        }

        // 返回当前 vector 中存储的元素个数
        size_t size() const { return finish_ - start_; }
        // 返回当前分配的内存容量（单位：元素个数）
        size_t capacity() const { return end_of_storage_ - start_; }
        // 判断 vector 是否为空
        bool empty() const { return size() == 0; }

        // 为 vector 分配至少 n 个元素的存储空间（扩容操作）
        void reserve(size_t n)
        {
            if (n > capacity())
            {
                size_t old_size = size();
                // 用 allocator 申请原始内存
                T *tmp = allocator_type::allocate(n);
                // 将原有元素复制到新的内存中
                for (size_t i = 0; i < old_size; ++i)
                    allocator_type::construct(&tmp[i], std::move(start_[i]));
                // 销毁旧元素并释放旧内存
                for (size_t i = 0; i < old_size; ++i)
                    allocator_type::destroy(&start_[i]);
                allocator_type::deallocate(start_, capacity());

                // 更新内部指针
                start_ = tmp;
                finish_ = start_ + old_size;
                end_of_storage_ = start_ + n;
            }
        }

        // 改变 vector 的大小为 n，若扩容则新增元素使用 val 初始化
        void resize(size_t n, const T &val = T())
        {
            if (n < size())
            {
                // 缩容时，销毁多余元素
                for (auto it = start_ + n; it != finish_; ++it)
                    allocator_type::destroy(it);
                finish_ = start_ + n;
            }
            else
            {
                reserve(n);
                // 构造新增元素
                while (finish_ != start_ + n)
                {
                    allocator_type::construct(finish_, val);
                    ++finish_;
                }
            }
        }

        // 在 vector 尾部添加一个新元素
        void push_back(const T &val)
        {
            insert(end(), val);
        }

        // 移除 vector 尾部的元素
        void pop_back()
        {
            assert(!empty());
            --finish_;
            allocator_type::destroy(finish_);
        }

        // emplace_back 接口
        template <typename... Args>
        void emplace_back(Args &&...args)
        {
            if (finish_ == end_of_storage_)
            {
                size_t new_cap = capacity() == 0 ? 4 : capacity() * 2;
                reserve(new_cap);
            }
            allocator_type::construct(finish_, std::forward<Args>(args)...);
            ++finish_;
        }

        // 在指定位置 pos 插入元素 val
        iterator insert(iterator pos, const T &val)
        {
            assert(pos >= start_ && pos <= finish_);
            size_t idx = pos - start_;
            if (finish_ == end_of_storage_)
            {
                size_t new_cap = capacity() == 0 ? 4 : capacity() * 2;
                reserve(new_cap);
            }
            pos = start_ + idx;
            // 移动元素
            for (iterator it = finish_; it > pos; --it)
            {
                allocator_type::construct(it, std::move(*(it - 1)));
                allocator_type::destroy(it - 1);
            }
            // 插入新元素
            allocator_type::construct(pos, val);
            ++finish_;
            return pos;
        }

        // 删除指定位置 pos 的元素，并返回删除位置
        iterator erase(iterator pos)
        {
            assert(pos >= start_ && pos < finish_);
            // 销毁 pos
            allocator_type::destroy(pos);
            // 移动后续元素覆盖
            for (iterator it = pos; it < finish_ - 1; ++it)
            {
                allocator_type::construct(it, std::move(*(it + 1)));
                allocator_type::destroy(it + 1);
            }
            --finish_;
            return pos;
        }

        // 获取首尾元素
        T &front() { return start_[0]; }
        T &back() { return *(finish_ - 1); }
        const T &front() const { return start_[0]; }
        const T &back() const { return *(finish_ - 1); }

        // 重载 [] 运算符，提供对元素的可读写访问
        T &operator[](size_t pos)
        {
            assert(pos < size());
            return start_[pos];
        }
        const T &operator[](size_t pos) const
        {
            assert(pos < size());
            return start_[pos];
        }

        // 交换两个 vector 内部数据的指针，效率高，不需要复制元素
        void swap(vector<T> &v)
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
                allocator_type::destroy(it);
            finish_ = start_;
        }

        // 析构函数，释放 vector 内部申请的内存空间
        ~vector()
        {
            clear();
            allocator_type::deallocate(start_, capacity());
            start_ = finish_ = end_of_storage_ = nullptr;
        }

    private:
        iterator start_ = nullptr;
        iterator finish_ = nullptr;
        iterator end_of_storage_ = nullptr;
    };
}; // namespace zstl
