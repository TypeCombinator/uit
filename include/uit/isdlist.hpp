// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#pragma once
#include <iterator>
#include <uit/intrusive.hpp>

namespace uit {

template <auto Right, auto Left>
class isdlist;

template <typename T, typename MT, MT T::* Right, MT T::* Left>
class isdlist<Right, Left> {
   public:
    isdlist() noexcept {
        m_right = nullptr;
    }

    isdlist(const isdlist&) = delete;

    isdlist& operator=(const isdlist&) = delete;

    isdlist(isdlist&& other) noexcept {
        move_from(std::move(other));
    }

    isdlist& operator=(isdlist&& other) noexcept {
        if (this != &other) {
            move_from(std::move(other));
        }
        return *this;
    }

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

    void push_front(T* node) noexcept {
        T* first;
        first = m_right;

        node->*Right = first;
        node->*Left = mock_head();

        m_right = node;
        if (first != nullptr) {
            first->*Left = node;
        }
    }

    static void remove(T* node) noexcept {
        T* right = node->*Right;
        T* left = node->*Left;

        if (right != nullptr) {
            right->*Left = left;
        }
        left->*Right = right;
    }

    T* pop_front() noexcept {
        T* right = m_right;
        if (right == nullptr) {
            return nullptr;
        }
        remove(right);
        return right;
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
    void move_from(isdlist&& other) noexcept {
        if (other.empty()) [[unlikely]] {
            clear();
        } else {
            m_right = other.m_right;

            m_right->*Left = mock_head();

            other.clear();
        }
    }

    [[nodiscard]]
    T* mock_head() noexcept {
        // UB!!!
        return container_of(Right, &m_right);
    }

    [[nodiscard]]
    const T* const_mock_head() const noexcept {
        // UB!!!
        return const_container_of(Right, &m_right);
    }

    T* m_right;
};

} // namespace uit