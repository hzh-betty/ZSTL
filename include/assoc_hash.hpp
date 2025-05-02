#pragma once
#include "hash_table.hpp"
namespace zstl
{
    // 空类型标记，用于模板元编程中区分unordered_set和map
    struct hash_null_type
    {
    };

    /**
     * @brief 通用关联容器模板，基于哈希表实现
     *
     * @tparam Key        键类型
     * @tparam Mapped     映射值类型。设为null_type时表示unordered_set容器
     * @tparam Compare    键比较函数对象类型
     * @tparam Unique     是否强制键唯一。true为类似std::unordered_set/map，false为类似unordered_multiset/map
     */
    template <typename Key, typename Mapped, typename Hash, typename Compare, bool Unique>
    class assoc_hash
    {
    public:
        // 根据Mapped类型决定值类型：unordered_set时为Key，map时为pair<const Key, Mapped>
        using value_type = std::conditional_t<
            std::is_same_v<Mapped, hash_null_type>,
            Key,
            std::pair<const Key, Mapped>>;

        // 底层哈希表类型
        using hash_type = HashTable<Key, value_type, Hash, Compare>;

        // unordered_set容器使用const_iterator禁止修改键值，map使用普通iterator允许修改value部分
        using const_iterator = typename hash_type::const_iterator;
        using iterator = std::conditional_t<
            std::is_same_v<Mapped, hash_null_type>,
            const_iterator,
            typename hash_type::iterator>;

    public:
        /* 迭代器访问 */
        iterator begin() noexcept { return hash_.begin(); }
        const_iterator begin() const noexcept { return hash_.begin(); }
        iterator end() noexcept { return hash_.end(); }
        const_iterator end() const noexcept { return hash_.end(); }

    public:
        // 默认构造/拷贝/移动构造函数
        assoc_hash() = default;
        assoc_hash(const assoc_hash &) = default;
        assoc_hash &operator=(const assoc_hash &) = default;
        assoc_hash(assoc_hash &&h) = default;
        assoc_hash &operator=(assoc_hash &&h) = default;
        ~assoc_hash() = default;

        /**
         * @brief 初始化列表构造函数
         * @param il 包含初始元素的初始化列表
         * @details 逐个元素构造，允许重复元素的插入（取决于Unique参数）
         */
        assoc_hash(std::initializer_list<value_type> il)
        {
            for (auto &e : il)
            {
                emplace(std::move(e));
            }
        }

        /* 容量查询 */
        bool empty() const noexcept { return hash_.empty(); }
        size_t size() const noexcept { return hash_.size(); }

        /* 查找操作 */
        iterator find(const Key &k) const { return hash_.find(k); }

        // 获取键值范围
        std::pair<iterator, iterator> equal_range(const Key &k)
        {
            return hash_.equal_range(k);
        }
        std::pair<const_iterator, const_iterator> equal_range(const Key &k) const
        {
            return hash_.equal_range(k);
        }

        /**
         * @brief 统计键出现的次数
         * @param key 要统计的键值
         * @return size_t 键在容器中的出现次数
         * @note Unique=true时返回0或1，Unique=false时返回实际数量
         */
        size_t count(const Key &key) const
        {
            auto p = equal_range(key);
            size_t cnt = 0;
            for (auto it = p.first; it != p.second; ++it)
                ++cnt;
            return cnt;
        }

        /* 删除操作 */
        iterator erase(const_iterator pos) { return hash_.erase(pos); }
        iterator erase(const_iterator first, const_iterator last) { return hash_.erase(first, last); }

        /**
         * @brief 删除所有匹配键的元素
         * @param k 要删除的键
         * @return size_t 被删除的元素数量
         * @note Unique=true时最多删除1个元素，Unique=false时删除所有匹配元素
         */
        size_t erase(const Key &k)
        {
            if constexpr (Unique)
            {
                return hash_.erase(k); // 唯一键直接删除
            }
            else
            { // 允许多个相同键，删除区间
                auto [l, r] = equal_range(k);
                size_t old_size = size();
                hash_.erase(l, r);
                return old_size - size();
            }
        }

        /* 插入操作 */

        /**
         * @brief 原位构造元素
         * @tparam Args 构造参数类型
         * @param args 构造参数
         * @return 对于Unique容器返回pair<iterator, bool>，表示插入位置和是否成功插入
         *         对于非Unique容器返回iterator，指向插入位置
         * @note Unique容器插入失败时返回已存在元素的位置和false
         */
        template <typename... Args>
        auto emplace(Args &&...args)
        {
            if constexpr (Unique)
            {
                return hash_.emplace_unique(std::forward<Args>(args)...);
            }
            else
            {
                return hash_.emplace_duplicate(std::forward<Args>(args)...);
            }
        }

        // 传统插入方法
        auto insert(const value_type &v)
        {
            return emplace(v);
        }

        // 只在 value_type 可由 P&& 构造时才参与重载，等价于 emplace 的完美转发
        template <typename P,
                  typename = std::enable_if_t<std::is_constructible_v<value_type, P &&>>>
        auto insert(P &&x)
        {
            return emplace(std::forward<P>(x));
        }

        /**
         * @brief 下标访问运算符（仅适用于map且键唯一的情况）
         * @param key 要访问的键
         * @return Mapped& 对应的映射值的引用
         * @note 如果键不存在，插入一个值初始化的元素
         */
        template <typename M = Mapped, bool U = Unique>
        std::enable_if_t<!std::is_same_v<M, hash_null_type> && U, M &>
        operator[](const Key &key)
        {
            auto [it, inserted] = hash_.emplace_unique(key, M());
            return it->second;
        }

        /* 其他操作 */
        void clear() noexcept { hash_.clear(); }
        void swap(assoc_hash &o) noexcept { hash_.swap(o.hash_); }

    private:
        hash_type hash_; // 底层红黑树实现
    };
};