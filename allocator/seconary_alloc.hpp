#pragma once
#include "primary_alloc.hpp"
namespace zstl
{
    // 对齐边界与最大“小块”尺寸
    inline constexpr size_t ALIGN = 8;                      // 小块上调至 8 字节对齐
    inline constexpr size_t MAX_BYTES = 128;                // 二级配置器只管理 ≤128 字节
    inline constexpr size_t NFREELISTS = MAX_BYTES / ALIGN; // 共 16 个 free_list

    /* 二级空间配置器,当申请内存下于128bytes时,
    采用内存池的方式实现*/
    template <typename T>
    class SeconaryAlloc
    {
        // 空闲链表节点
        union Obj
        {
            Obj *next;           // 链表指针
            char client_data[1]; // 客户可用内存起点
        };

    public:
        using value_type = T;
        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;

    public:
        static pointer allocate(size_type n)
        {
            if (n == 0)
                return nullptr;
            size_type bytes = n * sizeof(value_type);
            size_type idx = freelist_index(bytes);
            size_type size = round_up(bytes);
            // 如果对应 free list 有可用块，直接返回
            if (free_list_[idx])
            {
                Obj *obj = free_list_[idx];
                free_list_[idx] = obj->next;
                return reinterpret_cast<pointer>(obj);
            }
            // 否则 refill
            return reinterpret_cast<pointer>(refill(size));
        }

        static void deallocate(pointer ptr, size_type n)
        {
            if (ptr == nullptr)
                return;
            size_type bytes = n * sizeof(value_type);
            size_type idx = freelist_index(bytes);
            reinterpret_cast<Obj *>(ptr)->next = free_list_[idx];
            free_list_[idx] = reinterpret_cast<Obj *>(ptr);
        }

    private:
        // 向上取 ALIGN 的倍数
        static size_type round_up(size_type bytes)
        {
            return (bytes + ALIGN - 1) & ~(ALIGN - 1);
        }
        // 计算 bytes 应进哪个 free_list
        static size_type freelist_index(size_type bytes)
        {
            return round_up(bytes) / ALIGN - 1;
        }

        // 向内存池要 “nobjs” 个 size 大小的块，返回首块地址
        static char *chunk_alloc(size_type size, int &nobjs)
        {
            char *result = nullptr;
            size_type total_bytes = size * nobjs;
            size_type bytes_left = end_free_ - start_free_; // 池中剩余字节数

            if (bytes_left >= total_bytes)
            {
                // 情形 2：内存池足够整个请求
                result = start_free_;
                start_free_ += total_bytes;
                return result;
            }
            else if (bytes_left >= size)
            {
                // 情形 3：池中不足整个请求，但至少一个块
                nobjs = bytes_left / size; // 调整可分配块数
                total_bytes = size * nobjs;
                result = start_free_;
                start_free_ += total_bytes;
                return result;
            }
            else
            {
                // 池中连一个块都不够（情形 4 或 1 的补充）
                if (bytes_left > 0)
                {
                    // 池中残余 < size，但 >0，将这段零头回收至相应 free_list
                    size_type idx = freelist_index(bytes_left);
                    reinterpret_cast<Obj *>(start_free_)->next = free_list_[idx];
                    free_list_[idx] = reinterpret_cast<Obj *>(start_free_);
                }

                // 向系统申请新内存：申请 2×total_bytes + heap_size_/16（增长策略）
                size_type bytes_to_get = 2 * total_bytes + round_up(heap_size_ >> 4);
                start_free_ = static_cast<char *>(std::malloc(bytes_to_get));
                if (!start_free_)
                {
                    // 情形 4：系统 malloc 失败，尝试从 larger free_list 拿块
                    for (size_type i = size + ALIGN; i <= MAX_BYTES; i += ALIGN)
                    {
                        Obj *&fl = free_list_[freelist_index(i)];
                        if (fl)
                        {
                            // 拿下第一个大块，调整池边界，递归重试
                            Obj *p = fl;
                            fl = p->next;
                            start_free_ = reinterpret_cast<char *>(p);
                            end_free_ = start_free_ + i;
                            return chunk_alloc(size, nobjs);
                        }
                    }
                    // 最后退回一级配置器（抛异常或循环重试）
                    start_free_ = zstl::PrimaryAlloc<char>::allocate(bytes_to_get);
                    if (!start_free_)
                        throw std::bad_alloc();
                }
                heap_size_ += bytes_to_get;
                end_free_ = start_free_ + bytes_to_get;
                // 递归，为了按新的池状态分配
                return chunk_alloc(size, nobjs);
            }
        }

        static void *refill(size_type size)
        {
            int nobjs = 20; // 默认一次批量申请 20 块
            char *chunk = chunk_alloc(size, nobjs);
            if (nobjs == 1)
                return chunk; // 仅够 1 块，直接返回

            // 否则将 [1..nobjs-1] 块串入 free_list
            Obj *result = reinterpret_cast<Obj *>(chunk);
            Obj *current = nullptr;
            Obj *next = nullptr;
            size_type idx = freelist_index(size);

            // 第一个块留给调用者，其余链接
            free_list_[idx] = next = reinterpret_cast<Obj *>(chunk + size);
            for (int i = 1; i < nobjs - 1; ++i)
            {
                current = next;
                next = reinterpret_cast<Obj *>(reinterpret_cast<char *>(next) + size);
                current->next = next;
            }
            next->next = nullptr;
            return result;
        }

    private:
        inline static Obj *free_list_[NFREELISTS] = {nullptr}; // 自由链表
        inline static char *start_free_ = nullptr;             // 内存池起始
        inline static char *end_free_ = nullptr;               // 内存池终点
        inline static size_type heap_size_ = 0;                 // 从系统（malloc）累计获取的字节数
    };
};