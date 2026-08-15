// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef IDSLIST_CC34206A_3577_4178_B59C_4923C34A71A6
#define IDSLIST_CC34206A_3577_4178_B59C_4923C34A71A6
#include <iterator>
#include <uit/intrusive.hpp>

namespace uit {

template <auto Right>
class idslist;

template <typename T, typename MT, MT T::*Right>
class idslist<Right> {
   public:
    constexpr idslist() noexcept
        : m_right{nullptr}
        , m_left{nullptr} {
    }

    // constexpr idslist(const idslist &other) noexcept {
    //     m_right = other.m_right;
    //     m_left = other.m_left;
    // }

    // constexpr idslist &operator=(const idslist &other) noexcept {
    //     m_right = other.m_right;
    //     m_left = other.m_left;
    //     return *this;
    // }

    // constexpr idslist(idslist &&other) noexcept {
    //     move_from(std::move(other));
    // }

    // constexpr idslist &operator=(idslist &&other) noexcept {
    //     if (this != &other) [[likely]] {
    //         move_from(std::move(other));
    //     }
    //     return *this;
    // }

    [[nodiscard]]
    constexpr bool empty() const noexcept {
        return m_right == nullptr;
    }

    constexpr void clear() noexcept {
        m_right = nullptr;
        m_left = nullptr;
    }

    [[nodiscard]]
    constexpr T &front() const noexcept {
        return *m_right;
    }

    [[nodiscard]]
    constexpr T &back() const noexcept {
        return *m_left;
    }

    constexpr void push_front(T *node) noexcept {
        node->*Right = m_right;
        if (m_right == nullptr) [[unlikely]] {
            m_left = node;
        }
        m_right = node;
    }

    constexpr void push_back(T *node) noexcept {
        node->*Right = nullptr;
        if (m_right != nullptr) [[likely]] {
            m_left->*Right = node;
        } else {
            m_right = node;
        }
        m_left = node;
    }

    void pop_front() noexcept {
        T *first = m_right;
        m_right = first->*Right;
        if (first == m_left) [[unlikely]] {
            m_left = nullptr;
        }
    }

    constexpr bool remove(T *node) noexcept {
#if 0 // TODO: Need benchmark
        T **left_ptr = &m_right;
        T *left = nullptr;
        for (T *right = *left_ptr; right != nullptr;) {
            if (node == right) {
                if (node == m_left) [[unlikely]] { // Last?
                    m_left = left;
                }
                *left_ptr = node->*Right;
                return true;
            }
            left = right;
            left_ptr = &(right->*Right);
            right = *left_ptr;
        }
#else
        T *left = nullptr;
        for (T *right = m_right; right != nullptr;) {
            if (node == right) {
                if (left == nullptr) [[unlikely]] { // First?
                    m_right = node->*Right;
                } else {
                    left->*Right = node->*Right;
                }
                if (node == m_left) [[unlikely]] { // Last?
                    m_left = left;
                }
                return true;
            }
            left = right;
            right = right->*Right;
        }
#endif
        return false;
    }

    // TODO; need a more generic iterator
    template <typename T_CV>
    struct iterator_t {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T_CV;
        using difference_type = std::ptrdiff_t;
        using pointer = T_CV *;
        using reference = T_CV &;

        explicit constexpr iterator_t(pointer item) {
            current = item;
        }

        [[nodiscard]]
        constexpr reference operator*() noexcept {
            return *current;
        }

        [[nodiscard]]
        constexpr reference operator*() const noexcept {
            return *current;
        }

        [[nodiscard]]
        constexpr pointer operator->() noexcept {
            return current;
        }

        [[nodiscard]]
        constexpr pointer operator->() const noexcept {
            return current;
        }

        constexpr iterator_t &operator++() noexcept {
            current = current->*Right;
            return *this;
        }

        constexpr iterator_t operator++(int) noexcept {
            pointer old = current;
            current = current->*Right;
            return iterator_t{old};
        }

        constexpr bool operator==(const iterator_t &other) const noexcept {
            return current == other.current;
        }

       private:
        pointer current{nullptr};
    };

    using iterator = iterator_t<T>;
    using const_iterator = iterator_t<const T>;

    constexpr iterator begin() const noexcept {
        return iterator{m_right};
    }

    constexpr iterator begin() noexcept {
        return iterator{m_right};
    }

    constexpr iterator end() const noexcept {
        return iterator{nullptr};
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

   private:
    constexpr void move_from(idslist &&other) noexcept {
        *this = other;
        other.clear();
    }

    T *m_right;
    T *m_left;
};
} // namespace uit
#endif // idslist.hpp