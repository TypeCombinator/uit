#ifndef IDLIST_PROXY_6E544103_E9F9_4A88_BA0D_A8D7150303BF
#define IDLIST_PROXY_6E544103_E9F9_4A88_BA0D_A8D7150303BF
#include <iterator>

namespace uit {

template <auto Right, auto Left>
class idlist_proxy;

template <typename T, typename MT, MT T::*Right, MT T::*Left>
class idlist_proxy<Right, Left> {
   public:
    T &m_sentinel;

    [[nodiscard]]
    constexpr bool empty() const noexcept {
        return &m_sentinel == m_sentinel.*Right;
    }

    void clear() noexcept {
        m_sentinel.*Left = m_sentinel.*Right = &m_sentinel;
    }

    [[nodiscard]]
    T &front() const noexcept {
        return *(m_sentinel.*Right);
    }

    [[nodiscard]]
    T &back() const noexcept {
        return *(m_sentinel.*Left);
    }

    [[nodiscard]]
    constexpr T *sentinel() noexcept {
        return &m_sentinel;
    }

    [[nodiscard]]
    constexpr const T *sentinel() const noexcept {
        return &m_sentinel;
    }

    static constexpr void insert(T *node, T *left, T *right) noexcept {
        node->*Right = right;
        node->*Left = left;

        left->*Right = node;
        right->*Left = node;
    }

    static constexpr void remove(T *left, T *right) noexcept {
        left->*Right = right;
        right->*Left = left;
    }

    static constexpr void remove(T *node) noexcept {
        remove(node->*Left, node->*Right);
    }

    constexpr void push_front(T *node) noexcept {
        insert(node, &m_sentinel, m_sentinel.*Right);
    }

    constexpr void push_back(T *node) noexcept {
        insert(node, m_sentinel.*Left, &m_sentinel);
    }

    void pop_front() noexcept {
        T *right = m_sentinel.*Right;
        remove(&m_sentinel, right->*Right);
    }

    void pop_back() noexcept {
        T *left = m_sentinel.*Left;
        remove(left->*Left, &m_sentinel);
    }

    template <typename T_CV, bool is_reverse = false>
    struct iterator_t {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T_CV;
        using difference_type = std::ptrdiff_t;
        using pointer = T_CV *;
        using reference = T_CV &;

        explicit iterator_t(pointer item) {
            current = item;
        }

        [[nodiscard]]
        reference operator*() const noexcept {
            return *current;
        }

        [[nodiscard]]
        reference operator*() noexcept {
            return *current;
        }

        [[nodiscard]]
        pointer operator->() const noexcept {
            return current;
        }

        [[nodiscard]]
        pointer operator->() noexcept {
            return current;
        }

        iterator_t &operator++() noexcept {
            if constexpr (!is_reverse) {
                current = current->*Right;
            } else {
                current = current->*Left;
            }
            return *this;
        }

        iterator_t operator++(int) noexcept {
            pointer old = current;
            if constexpr (!is_reverse) {
                current = current->*Right;
            } else {
                current = current->*Left;
            }
            return iterator_t{old};
        }

        iterator_t &operator--() noexcept {
            if constexpr (!is_reverse) {
                current = current->*Left;
            } else {
                current = current->*Right;
            }
            return *this;
        }

        iterator_t operator--(int) noexcept {
            pointer old = current;
            if constexpr (!is_reverse) {
                current = current->*Left;
            } else {
                current = current->*Right;
            }
            return iterator_t{old};
        }

        bool operator==(const iterator_t &other) const noexcept {
            return current == other.current;
        }

        bool operator!=(const iterator_t &other) const noexcept {
            return current != other.current;
        }
       private:
        pointer current{nullptr};
    };

    using iterator = iterator_t<T>;
    using const_iterator = iterator_t<const T>;
    using reverse_iterator = iterator_t<T, true>;
    using const_reverse_iterator = iterator_t<const T, true>;

    const_iterator begin() const noexcept {
        return const_iterator{m_sentinel.*Right};
    }

    iterator begin() noexcept {
        return iterator{m_sentinel.*Right};
    }

    const_iterator end() const noexcept {
        return const_iterator{&m_sentinel};
    }

    iterator end() noexcept {
        return iterator{&m_sentinel};
    }

    const_iterator cbegin() const noexcept {
        return const_iterator{m_sentinel.*Right};
    }

    const_iterator cend() const noexcept {
        return const_iterator{&m_sentinel};
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{m_sentinel.*Left};
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator{m_sentinel.*Left};
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{&m_sentinel};
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator{&m_sentinel};
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{m_sentinel.*Left};
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{&m_sentinel};
    }
};
} // namespace uit
#endif // idlist_proxy.hpp