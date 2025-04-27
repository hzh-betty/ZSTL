#pragma once
#include "hash_table.hpp"
#include "string.hpp"
namespace zstl
{
    template <typename K, typename V>
    struct UMapCompare
    {
        bool operator()(const std::pair<K, V> &kv1, const std::pair<K, V> &kv2)
        {
            return kv1.first == kv2.first;
        }

        bool operator()(const std::pair<K, V> &kv1, const K&key)
        {
            return kv1.first == key;
        }

        bool operator()(const K&key, const std::pair<K, V> &kv2)
        {
            return key == kv2.first;
        }
        const K &operator()(const std::pair<K, V> &kv)
        {
            return kv.first;
        }
    };

    template <typename K>
    struct MapHashFunc
    {
        size_t operator()(const K &key)
        {
            return key;
        }
    };
    template <>
    struct MapHashFunc<string>
    {
        size_t operator()(const string &s)
        {
            size_t value = 0;
            for (auto ch : s)
            {
                value = value * 131 + ch;
            }
            return value;
        }
    };

    template <typename K, typename V, typename Hash = MapHashFunc
    <K>, typename Compare = UMapCompare<K, V>>
    class unordered_map
    {
    public:
        using iterator = typename HashTable<K, std::pair<const K, V>, Hash, Compare>::iterator;
        using const_iterator = typename HashTable<K, std::pair<const K, V>, Hash, Compare>::const_iterator;
        iterator begin()
        {
            return hash_table_.begin();
        }

        iterator end()
        {
            return hash_table_.end();
        }

        const_iterator begin() const
        {
            return hash_table_.begin();
        }

        const_iterator end() const
        {
            return hash_table_.end();
        }
        std::pair<iterator, iterator> equal_range(const K &k)
        {
            return hash_table_.equal_range(k);
        }

        std::pair<const_iterator, const_iterator> equal_range(const K &k) const
        {
            return hash_table_.equal_range(k);
        }

        std::pair<iterator, bool> insert(const std::pair<K, V> &data)
        {
            return hash_table_.insert_unique(data);
        }

        V &operator[](const K &key)
        {
            auto ret = insert(std::make_pair(key, V()));
            return ret.first->second;
        }

        iterator find(const K &key)
        {
            return hash_table_.find(key);
        }

        const_iterator find(const K &key) const
        {
            return hash_table_.find(key);
        }

        size_t count(const K &key) const
        {
            const_iterator iter = find(key);
            return iter != end() ? 1 : 0;
        }

        iterator erase(const_iterator pos)
        {
            return hash_table_.erase(pos);
        }
        iterator erase(const_iterator begin, const_iterator end)
        {
            return hash_table_.erase(begin, end);
        }
        size_t erase(const K &key)
        {
            return hash_table_.erase(key) ;
        }

        void clear()
        {
            hash_table_.clear();
        }

        void swap(unordered_map &s)
        {
            hash_table_.swap(s.hash_table_);
        }

        size_t size() const
        {
            return hash_table_.size();
        }

        bool empty() const
        {
            return hash_table_.empty();
        }

    private:
        HashTable<K, std::pair<const K, V>, Hash, Compare> hash_table_;
    };

    template <typename K, typename Hash = SetHashFunc<K>, typename Compare = USetCompare<K>>
    class unordered_multimap
    {
    public:
        using iterator = typename HashTable<K, K, Hash, Compare>::const_iterator;
        using const_iterator = typename HashTable<K, K, Hash, Compare>::const_iterator;
        iterator begin()
        {
            return hash_table_.begin();
        }

        iterator end()
        {
            return hash_table_.end();
        }

        const_iterator begin() const
        {
            return hash_table_.begin();
        }

        const_iterator end() const
        {
            return hash_table_.end();
        }

        std::pair<iterator, iterator> equal_range(const K &k)
        {
            return hash_table_.equal_range(k);
        }

        std::pair<const_iterator, const_iterator> equal_range(const K &k) const
        {
            return hash_table_.equal_range(k);
        }

        iterator insert(const K &key)
        {
            return hash_table_.insert_duplicate(key);
        }

        iterator find(const K &key)
        {
            return hash_table_.find(key);
        }

        const_iterator find(const K &key) const
        {
            return hash_table_.find(key);
        }

        size_t count(const K &key) const
        {
            auto p = hash_table_.equal_range(key);
            const_iterator it_first = p.first;
            const_iterator it_end = p.second;
            size_t cnt = 0;
            while (it_first != it_end)
            {
                cnt++;
                ++it_first;
            }
            return cnt;
        }

        // 删除相关接口
        iterator erase(const_iterator pos)
        {
            return hash_table_.erase(pos);
        }
        iterator erase(const_iterator begin, const_iterator end)
        {
            return hash_table_.erase(begin, end);
        }
        size_t erase(const K &key)
        {
            auto p = equal_range(key);
            const size_t old_size = size();
            erase(p.first, p.second);
            return old_size - size();
        }

        void clear()
        {
            hash_table_.clear();
        }

        void swap(unordered_multimap &s)
        {
            hash_table_.swap(s.hash_table_);
        }

        size_t size() const
        {
            return hash_table_.size();
        }

        bool empty() const
        {
            return hash_table_.empty();
        }

    private:
        HashTable<K, K, Hash, Compare> hash_table_;
    };

};