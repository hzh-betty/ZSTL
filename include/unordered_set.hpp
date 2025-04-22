#pragma once
#include "hash_table.hpp"
#include "string.hpp"
namespace zstl
{
    template <typename K>
    struct USetCompare
    {
        const K &operator()(const K &key)
        {
            return key;
        }

        bool operator()(const K &key1, const K &key2)
        {
            return key1 == key2;
        }
    };

    template <typename K>
    struct HashFunc
    {
        size_t operator()(const K &key)
        {
            return key;
        }
    };
    template <>
    struct HashFunc<string>
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

    template <typename K, typename Hash = HashFunc<K>, typename Compare = USetCompare<K>>
    class unordered_set
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

        std::pair<iterator, bool> insert(const K &key)
        {
            return hash_table_.insert(key);
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
            return find(key) != end() ? 1 : 0;
        }

        size_t erase(const K &key)
        {
            return hash_table_.erase(key) ? 1 : 0;
        }

        void clear()
        {
            hash_table_.clear();
        }

        void swap(unordered_set &s)
        {
            hash_table_.swap(s.hash_table_);
        }

        size_t size()const
        {
            return hash_table_.size();
        }

        bool empty()const
        {
            return hash_table_.empty();
        }
    private:
        HashTable<K, K, Hash, Compare> hash_table_;
    };
};