#pragma once
#include <iostream>
#include <cassert>
namespace zstl
{
    template <typename T>
    class vector
    {
    public:
        // 用原始指针模拟迭代器
        using iterator = T *;
        using const_iterator = const T *;

        // 返回指向起始元素的迭代器
        iterator begin()
        {
            return start_;
        }

        // 返回指向最后一个元素之后的迭代器
        iterator end()
        {
            return finish_;
        }

        // 常量版本的 begin()，用于只读访问
        const_iterator begin() const
        {
            return start_;
        }

        // 常量版本的 end()
        const_iterator end() const
        {
            return finish_;
        }

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
            {
                push_back(e);
            }
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
        vector(int n, const T &val = T())
        {
            reserve(n);
            for (int i = 0; i < n; i++)
            {
                push_back(val);
            }
        }

        // 重载构造函数：支持 size_t 类型的 n
        vector(size_t n, const T &val = T())
        {
            reserve(n);
            for (int i = 0; i < n; i++)
            {
                push_back(val);
            }
        }

        // 返回当前 vector 中存储的元素个数
        size_t size() const
        {
            return finish_ - start_;
        }

        // 返回当前分配的内存容量（单位：元素个数）
        size_t capacity() const
        {
            return end_of_storage_ - start_;
        }

        // 判断 vector 是否为空
        bool empty() const
        {
            return size() == 0;
        }

        // 为 vector 分配至少 n 个元素的存储空间（扩容操作）
        void reserve(size_t n)
        {
            // 当请求的容量大于当前容量时才扩容
            if (n > capacity())
            {
                size_t old_size = size();
                T *tmp = new T[n]; // 申请新的内存块

                // 将原有元素复制到新的内存中
                for (size_t i = 0; i < old_size; i++)
                {
                    tmp[i] = start_[i];
                }
                // 释放旧的内存块
                delete[] start_;

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
                // 缩容时，直接将结束指针移到新位置（不释放内存）
                finish_ = start_ + n;
            }
            else
            {
                // 若扩容，则先确保内存足够，然后依次赋值初始化新区域
                reserve(n);
                while (finish_ != start_ + n)
                {
                    *finish_ = val;
                    ++finish_;
                }
            }
        }

        // 在 vector 尾部添加一个新元素
        void push_back(const T &val)
        {
            // 若内存已满则先扩容
            if (finish_ == end_of_storage_)
            {
                size_t new_size = capacity() == 0 ? 4 : capacity() * 2;
                reserve(new_size);
            }

            // 将新元素写入尾部，并将结束指针后移
            *finish_ = val;
            ++finish_;
        }

        // 移除 vector 尾部的元素
        void pop_back()
        {
            // 先确保 vector 非空，再移动结束指针
            assert(!empty());
            --finish_;
        }

        // 在指定位置 pos 插入元素 val
        iterator insert(iterator pos, const T &val)
        {
            assert(pos >= start_);
            assert(pos <= finish_);

            // 如果当前内存不足，则先扩容并重新定位 pos
            if (finish_ == end_of_storage_)
            {
                size_t len = pos - start_;
                size_t new_size = capacity() == 0 ? 4 : capacity() * 2;
                reserve(new_size);
                pos = start_ + len;
            }

            // 将 pos 后的元素全部向后移动一位，空出插入位置
            iterator end = finish_;
            while (end > pos)
            {
                *end = *(end - 1);
                --end;
            }
            // 插入新元素
            *end = val;
            ++finish_;
            return pos;
        }

        // 删除指定位置 pos 的元素，并返回删除位置
        iterator erase(iterator pos)
        {
            assert(pos >= start_ && pos < finish_);
            iterator end = pos + 1;
            // 将 pos 后面的所有元素依次前移覆盖被删除的元素
            while (end != finish_)
            {
                *(end - 1) = *end;
                ++end;
            }
            --finish_;
            return pos;
        }

        // 重载 [] 运算符，提供对元素的可读写访问
        T &operator[](size_t pos)
        {
            assert(pos < size());
            return start_[pos];
        }

        // 常量版本的 [] 运算符，仅提供只读访问
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
            finish_ = start_;
        }

        // 析构函数，释放 vector 内部申请的内存空间
        ~vector()
        {
            if (start_)
            {
                delete[] start_;
                start_ = finish_ = end_of_storage_ = nullptr;
            }
        }

    private:
        // 指向容器第一个元素的指针
        iterator start_ = nullptr;
        // 指向容器最后一个元素之后的位置的指针
        iterator finish_ = nullptr;
        // 指向已分配内存结束位置的指针
        iterator end_of_storage_ = nullptr;
    };
};
