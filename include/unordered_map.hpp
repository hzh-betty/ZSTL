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
};