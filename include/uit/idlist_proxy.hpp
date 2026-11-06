// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
// SPDX-License-Identifier: MPL-2.0
//
// This project is licensed under the Mozilla Public License 2.0.
// Please see the accompanying LICENSE file in this project root
// or https://spdx.org/licenses/MPL-2.0.html for details.

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

    constexpr void clear() noexcept {
        m_sentinel.*Left = m_sentinel.*Right = &m_sentinel;
    }

    [[nodiscard]]
    constexpr T &front() const noexcept {
        return *(m_sentinel.*Right);
    }

    [[nodiscard]]
    constexpr T &back() const noexcept {
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

    constexpr void pop_front() noexcept {
        const T *right = m_sentinel.*Right;
        remove(&m_sentinel, right->*Right);
    }

    constexpr void pop_back() noexcept {
        const T *left = m_sentinel.*Left;
        remove(left->*Left, &m_sentinel);
    }

    template <typename TCV, bool IsReverse = false>
    struct iterator_t {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = TCV;
        using difference_type = std::ptrdiff_t;
        using pointer = TCV *;
        using reference = TCV &;

        explicit constexpr iterator_t(pointer item) noexcept
            : current{item} {
        }

        [[nodiscard]]
        constexpr reference operator*() const noexcept {
            return *current;
        }

        [[nodiscard]]
        constexpr reference operator*() noexcept {
            return *current;
        }

        [[nodiscard]]
        constexpr pointer operator->() const noexcept {
            return current;
        }

        [[nodiscard]]
        constexpr pointer operator->() noexcept {
            return current;
        }

        constexpr iterator_t &operator++() noexcept {
            if constexpr (!IsReverse) {
                current = current->*Right;
            } else {
                current = current->*Left;
            }
            return *this;
        }

        constexpr iterator_t operator++(int) noexcept {
            pointer old = current;
            if constexpr (!IsReverse) {
                current = current->*Right;
            } else {
                current = current->*Left;
            }
            return iterator_t{old};
        }

        constexpr iterator_t &operator--() noexcept {
            if constexpr (!IsReverse) {
                current = current->*Left;
            } else {
                current = current->*Right;
            }
            return *this;
        }

        constexpr iterator_t operator--(int) noexcept {
            pointer old = current;
            if constexpr (!IsReverse) {
                current = current->*Left;
            } else {
                current = current->*Right;
            }
            return iterator_t{old};
        }

        constexpr bool operator==(const iterator_t &other) const noexcept {
            return current == other.current;
        }

        constexpr bool operator!=(const iterator_t &other) const noexcept {
            return current != other.current;
        }
       private:
        pointer current{nullptr};
    };

    using iterator = iterator_t<T>;
    using const_iterator = iterator_t<const T>;
    using reverse_iterator = iterator_t<T, true>;
    using const_reverse_iterator = iterator_t<const T, true>;

    [[nodiscard]]
    constexpr const_iterator begin() const noexcept {
        return const_iterator{m_sentinel.*Right};
    }

    [[nodiscard]]
    constexpr iterator begin() noexcept {
        return iterator{m_sentinel.*Right};
    }

    [[nodiscard]]
    constexpr const_iterator end() const noexcept {
        return const_iterator{&m_sentinel};
    }

    [[nodiscard]]
    constexpr iterator end() noexcept {
        return iterator{&m_sentinel};
    }

    [[nodiscard]]
    constexpr const_iterator cbegin() const noexcept {
        return const_iterator{m_sentinel.*Right};
    }

    [[nodiscard]]
    constexpr const_iterator cend() const noexcept {
        return const_iterator{&m_sentinel};
    }

    [[nodiscard]]
    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{m_sentinel.*Left};
    }

    [[nodiscard]]
    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator{m_sentinel.*Left};
    }

    [[nodiscard]]
    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{&m_sentinel};
    }

    [[nodiscard]]
    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator{&m_sentinel};
    }

    [[nodiscard]]
    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{m_sentinel.*Left};
    }

    [[nodiscard]]
    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{&m_sentinel};
    }
};
} // namespace uit
#endif // idlist_proxy.hpp