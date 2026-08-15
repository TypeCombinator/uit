// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef ISDLIST_91B4F713_47E1_4184_AAC2_D26F17F436DE
#define ISDLIST_91B4F713_47E1_4184_AAC2_D26F17F436DE
#include <iterator>
#include <uit/intrusive.hpp>

namespace uit {
// It is better for the right member to be a pointer to a pointer, but it's not uniform.
template <auto Right, auto Left>
class isdlist;

template <typename T, typename MT, MT T::* Right, MT T::* Left>
class isdlist<Right, Left> {
   public:
    isdlist() noexcept {
        m_right = nullptr;
    }

    // isdlist(const isdlist&) = delete;

    // isdlist& operator=(const isdlist&) = delete;

    // isdlist(isdlist&& other) noexcept {
    //     move_from(std::move(other));
    // }

    // isdlist& operator=(isdlist&& other) noexcept {
    //     if (this != &other) {
    //         move_from(std::move(other));
    //     }
    //     return *this;
    // }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_right == nullptr;
    }

    void clear() noexcept {
        m_right = nullptr;
    }

    [[nodiscard]]
    T& front() const noexcept {
        return *m_right;
    }

    [[nodiscard]]
    static constexpr T* sentinel() noexcept {
        return nullptr;
    }

    void push_front(T* node) noexcept {
        T* first = m_right;

        node->*Right = first;
        node->*Left = nullptr;

        m_right = node;
        if (first != nullptr) [[likely]] {
            first->*Left = node;
        }
    }

    void remove(T* node) noexcept {
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

    void pop_front() noexcept {
        if (m_right == nullptr) [[unlikely]] {
            return;
        }
        T* first_right = m_right->*Right;
        m_right = first_right;
        if (first_right != nullptr) [[likely]] {
            first_right->*Left = nullptr;
        }
    }

    template <typename T_CV>
    struct iterator_t {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T_CV;
        using difference_type = std::ptrdiff_t;
        using pointer = T_CV*;
        using reference = T_CV&;

        explicit iterator_t(pointer item) {
            current = item;
        }

        [[nodiscard]]
        reference operator*() noexcept {
            return *current;
        }

        [[nodiscard]]
        reference operator*() const noexcept {
            return *current;
        }

        [[nodiscard]]
        pointer operator->() noexcept {
            return current;
        }

        [[nodiscard]]
        pointer operator->() const noexcept {
            return current;
        }

        iterator_t& operator++() noexcept {
            current = current->*Right;
            return *this;
        }

        iterator_t operator++(int) noexcept {
            pointer old = current;
            current = current->*Right;
            return iterator_t{old};
        }

        bool operator==(const iterator_t& other) const noexcept {
            return current == other.current;
        }

        bool operator!=(const iterator_t& other) const noexcept {
            return current != other.current;
        }
       private:
        pointer current{nullptr};
    };

    using iterator = iterator_t<T>;
    using const_iterator = iterator_t<const T>;

    iterator begin() const noexcept {
        return iterator{m_right};
    }

    iterator begin() noexcept {
        return iterator{m_right};
    }

    iterator end() const noexcept {
        return iterator{nullptr};
    }

    iterator end() noexcept {
        return iterator{nullptr};
    }

    const_iterator cbegin() const noexcept {
        return const_iterator{m_right};
    }

    const_iterator cend() const noexcept {
        return const_iterator{nullptr};
    }

   private:
    T* m_right;
};
} // namespace uit
#endif // isdlist.hpp