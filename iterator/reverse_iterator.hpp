#pragma once
#include "iterator.hpp"
namespace zstl
{
    // 所有迭代器都支持的操作* -> ++ == !=
    template <typename It>
    class input_reverse_iterator
    {
    public:
        using self = input_reverse_iterator<It>;

        using value_type = typename iterator_traits<It>::value_type;
        using iterator_category = typename iterator_traits<It>::iterator_category;
        using difference_type = typename iterator_traits<It>::difference_type;
        using pointer = typename iterator_traits<It>::pointer;
        using reference = typename iterator_traits<It>::reference;

    public:
        input_reverse_iterator() = default;
        explicit input_reverse_iterator(It x) : current_(x) {}
        // 普通迭代器构造const迭代器
        template <typename U, typename = std::enable_if_t<std::is_convertible_v<U, It>>>
        explicit input_reverse_iterator(const input_reverse_iterator<U> &other)
            : current_(other.base()) {}

        It base() const
        {
            return current_;
        }

        reference operator*() const
        {
            It tmp = current_;
            return *(--tmp);
        }

        pointer operator->() const
        {
            return &(operator*());
        }

        self &operator++()
        {
            --current_;
            return *this;
        }

        self operator++(int)
        {
            self tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const self &a) const
        {
            return current_ == a.current_;
        }

        bool operator!=(const self &a) const
        {
            return current_ != a.current_;
        }

    protected:
        It current_;
    };

    // 仅当底层迭代器是双向迭代器时支持-- > >= < <=
    template <typename It>
    class bidirectional_reverse_iterator
        : public input_reverse_iterator<It>
    {
        using Base = input_reverse_iterator<It>;

    public:
        using self = bidirectional_reverse_iterator<It>;
        using value_type = typename iterator_traits<It>::value_type;
        using iterator_category = typename iterator_traits<It>::iterator_category;
        using difference_type = typename iterator_traits<It>::difference_type;
        using pointer = typename iterator_traits<It>::pointer;
        using reference = typename iterator_traits<It>::reference;

    public:
        bidirectional_reverse_iterator() = default;
        explicit bidirectional_reverse_iterator(It x) : Base(x) {}
        // 普通迭代器构造const迭代器
        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, It>>>
        explicit bidirectional_reverse_iterator(const bidirectional_reverse_iterator<U>& other)
            : Base(other.base()) {}

        self &operator--()
        {
            ++this->current_;
            return *this;
        }
        self operator--(int)
        {
            self tmp = *this;
            ++this->current_;
            return tmp;
        }
    };

    // 仅当底层迭代器是随机迭代器时启用 +=, +, -=, -, []
    template <typename It>
    class random_access_reverse_iterator : public bidirectional_reverse_iterator<It>
    {
        using Base = bidirectional_reverse_iterator<It>;

    public:
        // 类型定义
        using self = random_access_reverse_iterator<It>;

        using value_type = typename iterator_traits<It>::value_type;
        using iterator_category = typename iterator_traits<It>::iterator_category;
        using difference_type = typename iterator_traits<It>::difference_type;
        using pointer = typename iterator_traits<It>::pointer;
        using reference = typename iterator_traits<It>::reference;

    public:
        random_access_reverse_iterator() = default;
        explicit random_access_reverse_iterator(It x) : Base(x) {}

        // 普通迭代器构造const迭代器
        template<typename U, typename = std::enable_if_t<std::is_convertible_v<U, It>>>
        explicit random_access_reverse_iterator(const random_access_reverse_iterator<U>& other)
            : Base(other.base()) {}

        bool operator<(const self &b) const
        {
            return this->current_ > b.current_;
        }

        // 比较操作
        bool operator>(const self &b) const
        {
            return this->current_ < b.current_;
        }

        bool operator<=(const self &b) const
        {
            return this->current_ >= b.current_;
        }

        bool operator>=(const self &b) const
        {
            return this->current_ <= b.current_;
        }

        self &operator+=(difference_type n)
        {
            this->current_ -= n;
            return *this;
        }

        self operator+(difference_type n) const
        {
            return self(this->current_ - n);
        }

        self &operator-=(difference_type n)
        {
            this->current_ += n;
            return *this;
        }

        self operator-(difference_type n) const
        {
            return self(this->current_ + n);
        }

        reference operator[](difference_type n) const
        {
            return *(*this + n);
        }
    };

    template <typename It>
    using basic_reverse_iterator = std::conditional_t<is_random_access_iterator_v<It>,
                                                      random_access_reverse_iterator<It>,
                                                      std::conditional_t<is_bidirectional_iterator_v<It>, bidirectional_reverse_iterator<It>, input_reverse_iterator<It>>>;
}