// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef ISLIST_6CB2236F_CA9B_42B5_85CC_C16BF43C71DF
#define ISLIST_6CB2236F_CA9B_42B5_85CC_C16BF43C71DF
#include <uit/detail/islist_iterator.hpp>
#include <uit/intrusive.hpp>

namespace uit {

template <auto Right>
class islist;

template <typename T, typename MT, MT T::* Right>
class islist<Right> {
   public:
    constexpr islist() noexcept {
        m_right = nullptr;
    }

    // islist(const islist& other) noexcept = default;

    // islist& operator=(const islist& other) noexcept = default;

    // islist(islist&& other) noexcept {
    //     m_right = other.m_right;
    //     other.clear();
    // }

    // islist& operator=(islist&& other) noexcept {
    //     if (this != &other) {
    //         m_right = other.m_right;
    //         other.clear();
    //     }
    //     return *this;
    // }

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

    constexpr void push_front(T* node) noexcept {
        node->*Right = m_right;
        m_right = node;
    }

    constexpr T* pop_front() noexcept {
        T* first = m_right;
        if (first == nullptr) [[unlikely]] {
            return nullptr;
        }
        m_right = first->*Right;
        return first;
    }

    constexpr T* remove(T* node) noexcept {
        T** left = &m_right;
        for (T* right = m_right; right != nullptr;) {
            if (right == node) {
                *left = right->*Right;
                return node;
            }
            left = &(right->*Right);
            right = right->*Right;
        }
        return nullptr;
    }

    using iterator = islist_iterator<T, Right>;
    using const_iterator = islist_iterator<const T, Right>;

    constexpr const_iterator begin() const {
        return const_iterator{m_right};
    }

    constexpr iterator begin() {
        return iterator{m_right};
    }

    constexpr const_iterator end() const {
        return const_iterator{nullptr};
    }

    constexpr iterator end() {
        return iterator{nullptr};
    }

    constexpr const_iterator cbegin() const {
        return const_iterator{m_right};
    }

    constexpr const_iterator cend() const {
        return const_iterator{nullptr};
    }
   private:
    T* m_right;
};

} // namespace uit
#endif // islist.hpp