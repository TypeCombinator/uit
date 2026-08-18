// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef IDLIST_ECA1754E_A213_45ED_BC83_5B6AD39C7669
#define IDLIST_ECA1754E_A213_45ED_BC83_5B6AD39C7669
#include <iterator>

namespace uit {
template <auto Right, auto Left>
class idlist;

template <typename T, typename MT, MT T::*Right, MT T::*Left>
class idlist<Right, Left> {
   public:
    T *m_right;
    T *m_left;

    // constexpr idlist() noexcept {
    //     m_left = m_right = nullptr;
    // }

    [[nodiscard]]
    constexpr bool empty() const noexcept {
        return m_right == nullptr;
    }

    constexpr void clear() noexcept {
        m_left = m_right = nullptr;
    }

    [[nodiscard]]
    constexpr T &front() const noexcept {
        return *m_right;
    }

    [[nodiscard]]
    constexpr T &back() const noexcept {
        return *m_left;
    }

    [[nodiscard]]
    static constexpr T *sentinel() noexcept {
        return nullptr;
    }

    constexpr void remove(T *node) noexcept {
        auto right = node->*Right;
        auto left = node->*Left;
        if (right != nullptr) [[likely]] {
            right->*Left = left;
        } else {
            m_left = left;
        }
        if (left != nullptr) [[likely]] {
            left->*Right = right;
        } else {
            m_right = right;
        }
    }

    constexpr void push_front(T *node) noexcept {
        T *first = m_right;
        node->*Right = first;
        node->*Left = nullptr;
        m_right = node;
        if (first != nullptr) [[likely]] {
            first->*Left = node;
        } else {
            m_left = node;
        }
    }

    constexpr void push_back(T *node) noexcept {
        T *last = m_left;
        node->*Right = nullptr;
        node->*Left = last;
        m_left = node;
        if (last != nullptr) [[likely]] {
            last->*Right = node;
        } else {
            m_right = node;
        }
    }

    constexpr void pop_front() noexcept {
        if (m_right == nullptr) [[unlikely]] {
            return;
        }
        T *first_right = m_right->*Right;
        m_right = first_right;
        if (first_right != nullptr) [[likely]] {
            first_right->*Left = nullptr;
        } else {
            m_left = nullptr;
        }
    }

    constexpr void pop_back() noexcept {
        if (m_left == nullptr) [[unlikely]] {
            return;
        }
        T *last_left = m_left->*Left;
        m_left = last_left;
        if (last_left != nullptr) [[likely]] {
            last_left->*Right = nullptr;
        } else {
            m_right = nullptr;
        }
    }

    template <typename T_CV, bool is_reverse = false>
    struct iterator_t {
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = T_CV;
        using difference_type = std::ptrdiff_t;
        using pointer = T_CV *;
        using reference = T_CV &;

        explicit constexpr iterator_t(pointer item) {
            current = item;
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
            if constexpr (!is_reverse) {
                current = current->*Right;
            } else {
                current = current->*Left;
            }
            return *this;
        }

        constexpr iterator_t operator++(int) noexcept {
            pointer old = current;
            if constexpr (!is_reverse) {
                current = current->*Right;
            } else {
                current = current->*Left;
            }
            return iterator_t{old};
        }

        constexpr iterator_t &operator--() noexcept {
            if constexpr (!is_reverse) {
                current = current->*Left;
            } else {
                current = current->*Right;
            }
            return *this;
        }

        constexpr iterator_t operator--(int) noexcept {
            pointer old = current;
            if constexpr (!is_reverse) {
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

    constexpr const_iterator begin() const noexcept {
        return const_iterator{m_right};
    }

    constexpr iterator begin() noexcept {
        return iterator{m_right};
    }

    constexpr const_iterator end() const noexcept {
        return const_iterator{nullptr};
    }

    constexpr iterator end() noexcept {
        return iterator{nullptr};
    }

    constexpr const_iterator cbegin() const noexcept {
        return const_iterator{m_right};
    }

    constexpr const_iterator cend() const noexcept {
        return const_iterator{nullptr};
    }

    constexpr const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{m_left};
    }

    constexpr reverse_iterator rbegin() noexcept {
        return reverse_iterator{m_left};
    }

    constexpr const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{nullptr};
    }

    constexpr reverse_iterator rend() noexcept {
        return reverse_iterator{nullptr};
    }

    constexpr const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{m_left};
    }

    constexpr const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{nullptr};
    }
};
} // namespace uit
#endif // idlist.hpp