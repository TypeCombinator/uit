// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
// SPDX-License-Identifier: MPL-2.0
//
// This project is licensed under the Mozilla Public License 2.0.
// Please see the accompanying LICENSE file in this project root
// or https://spdx.org/licenses/MPL-2.0.html for details.

#ifndef ISDLIST_91B4F713_47E1_4184_AAC2_D26F17F436DE
#define ISDLIST_91B4F713_47E1_4184_AAC2_D26F17F436DE
#include <uit/detail/islist_iterator.hpp>

namespace uit {
// It is better for the right member to be a pointer to a pointer, but it's not uniform.
template <auto Right, auto Left>
class isdlist;

template <typename T, typename MT, MT T::* Right, MT T::* Left>
class isdlist<Right, Left> {
   public:
    T* m_right;

    [[nodiscard]]
    constexpr bool empty() const noexcept {
        return m_right == nullptr;
    }

    constexpr void clear() noexcept {
        m_right = nullptr;
    }

    [[nodiscard]]
    constexpr T& front() const noexcept {
        return *m_right;
    }

    [[nodiscard]]
    static constexpr T* sentinel() noexcept {
        return nullptr;
    }

    constexpr void push_front(T* node) noexcept {
        T* first = m_right;

        node->*Right = first;
        node->*Left = nullptr;

        m_right = node;
        if (first != nullptr) [[likely]] {
            first->*Left = node;
        }
    }

    constexpr void remove(T* node) noexcept {
        T* right = node->*Right;
        T* left = node->*Left;

        if (right != nullptr) [[likely]] {
            right->*Left = left;
        }
        if (left != nullptr) [[likely]] {
            left->*Right = right;
        } else {
            m_right = right;
        }
    }

    constexpr void pop_front() noexcept {
        if (m_right == nullptr) [[unlikely]] {
            return;
        }
        T* first_right = m_right->*Right;
        m_right = first_right;
        if (first_right != nullptr) [[likely]] {
            first_right->*Left = nullptr;
        }
    }

    using iterator = islist_iterator<T, Right>;
    using const_iterator = islist_iterator<const T, Right>;

    [[nodiscard]]
    constexpr iterator begin() const noexcept {
        return iterator{m_right};
    }

    [[nodiscard]]
    constexpr iterator begin() noexcept {
        return iterator{m_right};
    }

    [[nodiscard]]
    constexpr iterator end() const noexcept {
        return iterator{nullptr};
    }

    [[nodiscard]]
    constexpr iterator end() noexcept {
        return iterator{nullptr};
    }

    [[nodiscard]]
    constexpr const_iterator cbegin() const noexcept {
        return const_iterator{m_right};
    }

    [[nodiscard]]
    constexpr const_iterator cend() const noexcept {
        return const_iterator{nullptr};
    }
};
} // namespace uit
#endif // isdlist.hpp