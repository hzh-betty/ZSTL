#pragma once
#include "rb_tree.hpp"
#include <type_traits>
#include "../iterator/reverse_iterator.hpp"
namespace zstl
{
    // 空类型标记，用于模板元编程中区分set和map
    struct tree_null_type
    {
    };

    /**
     * @brief 通用关联容器模板，基于红黑树实现，支持set和map两种容器
     *
     * @tparam key_type        键类型
     * @tparam mapped_type     映射值类型。设为null_type时表示set容器
     * @tparam Compare    键比较函数对象类型
     * @tparam Unique     是否强制键唯一。true为类似std::set/map，false为类似multiset/map
     */
    template <typename Key, typename Mapped, typename Compare, typename Alloc, bool Unique>
    class assoc_tree
    {
    public:
        using key_type = Key;
        using mapped_type = Mapped;
        using allocator_type = Alloc;
        using traits_allocator = allocator_traits<allocator_type>;
        using pointer = typename traits_allocator::pointer;
        using const_pointer = typename traits_allocator::const_pointer;
        using size_type = size_t;

        // 根据Mapped类型决定值类型：set时为Key，map时为pair<const key_type, mapped_type>
        using value_type = std::conditional_t<
            std::is_same_v<mapped_type, tree_null_type>,
            key_type,
            std::pair<const key_type, mapped_type>>;

        // 底层红黑树类型
        using tree_type = RBTree<key_type, value_type, Compare, Alloc>;

        // set容器使用const_iterator禁止修改键值，map使用普通iterator允许修改value部分
        using const_iterator = typename tree_type::const_iterator;
        using iterator = std::conditional_t<
            std::is_same_v<mapped_type, tree_null_type>,
            const_iterator,
            typename tree_type::iterator>;

        using reverse_iterator = basic_reverse_iterator<iterator>;
        using const_reverse_iterator = basic_reverse_iterator<iterator>;

    public:
        /* 迭代器访问 */
        iterator begin() noexcept { return tree_.begin(); }
        const_iterator begin() const noexcept { return tree_.begin(); }
        iterator end() noexcept { return tree_.end(); }
        const_iterator end() const noexcept { return tree_.end(); }

        // 反向迭代器
        reverse_iterator rbegin() { return tree_.rbegin(); }
        reverse_iterator rend() { return tree_.rend(); }
        const_reverse_iterator rbegin() const { return tree_.rbegin(); }
        const_reverse_iterator rend() const { return tree_.rend(); }

    public:
        // 默认构造/拷贝/移动构造函数
        assoc_tree(const allocator_type&alloc = allocator_type())
        :tree_(alloc)
        {

        }
        assoc_tree(const assoc_tree &) = default;
        assoc_tree &operator=(const assoc_tree &) = default;
        assoc_tree(assoc_tree &&) = default;
        assoc_tree &operator=(assoc_tree &&) = default;
        ~assoc_tree() = default;

        /**
         * @brief 初始化列表构造函数
         * @param il 包含初始元素的初始化列表
         * @details 逐个元素构造，允许重复元素的插入（取决于Unique参数）
         */
        assoc_tree(std::initializer_list<value_type> il,const allocator_type&alloc = allocator_type())
        :assoc_tree(alloc)
        {
            for (auto &e : il)
            {
                emplace(std::move(e));
            }
        }

        /* 容量查询 */
        bool empty() const noexcept { return tree_.empty(); }
        size_t size() const noexcept { return tree_.size(); }

        /* 查找操作 */
        iterator find(const key_type &k) const { return tree_.find(k); }

        // 范围查找
        iterator lower_bound(const key_type &k) { return tree_.lower_bound(k); }
        iterator upper_bound(const key_type &k) { return tree_.upper_bound(k); }
        const_iterator lower_bound(const key_type &k) const { return tree_.lower_bound(k); }
        const_iterator upper_bound(const key_type &k) const { return tree_.upper_bound(k); }

        // 获取键值范围
        std::pair<iterator, iterator> equal_range(const key_type &k)
        {
            return {lower_bound(k), upper_bound(k)};
        }
        std::pair<const_iterator, const_iterator> equal_range(const key_type &k) const
        {
            return {lower_bound(k), upper_bound(k)};
        }

        /**
         * @brief 统计键出现的次数
         * @param key 要统计的键值
         * @return size_t 键在容器中的出现次数
         * @note Unique=true时返回0或1，Unique=false时返回实际数量
         */
        size_t count(const key_type &key) const
        {
            auto p = equal_range(key);
            size_t cnt = 0;
            for (auto it = p.first; it != p.second; ++it)
                ++cnt;
            return cnt;
        }

        /* 删除操作 */
        iterator erase(const_iterator pos) { return tree_.erase(pos); }
        iterator erase(const_iterator first, const_iterator last) { return tree_.erase(first, last); }

        /**
         * @brief 删除所有匹配键的元素
         * @param k 要删除的键
         * @return size_t 被删除的元素数量
         * @note Unique=true时最多删除1个元素，Unique=false时删除所有匹配元素
         */
        size_t erase(const key_type &k)
        {
            if constexpr (Unique)
            {
                return tree_.erase(k); // 唯一键直接删除
            }
            else
            { // 允许多个相同键，删除区间
                auto [l, r] = equal_range(k);
                size_t old_size = size();
                tree_.erase(l, r);
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
                return tree_.emplace_unique(std::forward<Args>(args)...);
            }
            else
            {
                return tree_.emplace_duplicate(std::forward<Args>(args)...);
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
         * @return mapped_type& 对应的映射值的引用
         * @note 如果键不存在，插入一个值初始化的元素
         */
        template <typename M = mapped_type, bool U = Unique>
        std::enable_if_t<!std::is_same_v<M, tree_null_type> && U, M &>
        operator[](const key_type &key)
        {
            auto [it, inserted] = tree_.emplace_unique(key, M());
            return it->second;
        }

        /* 其他操作 */
        void clear() noexcept { tree_.clear(); }
        void swap(assoc_tree &o) noexcept { tree_.swap(o.tree_); }

    private:
        tree_type tree_; // 底层红黑树实现
    };
};