// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#pragma once
#include <iterator>
#include "intrusive.hpp"

namespace uit {

template <typename T, stag N = "">
class idlist {
   public:
    using node_t = idnode<T, N>;

    idlist() noexcept {
        head.left = head.right = mock_head();
    }

    idlist(const idlist &) = delete;

    idlist &operator=(const idlist &) = delete;

    idlist(idlist &&other) noexcept {
        move_from(std::move(other));
    }

    idlist &operator=(idlist &&other) noexcept {
        if (this != &other) [[likely]] {
            move_from(std::move(other));
        }
        return *this;
    }

    [[nodiscard]]
    bool empty() const noexcept {
        const T *mhead = const_mock_head();
        return mhead == mhead->node_t::right;
    }

    void clear() noexcept {
        head.left = head.right = mock_head();
    }

    [[nodiscard]]
    T &front() const noexcept {
        return *head.right;
    }

    [[nodiscard]]
    T &back() const noexcept {
        return *head.left;
    }

    static void insert(T *node, T *left, T *right) noexcept {
        node->node_t::right = right;
        node->node_t::left = left;

        left->node_t::right = node;
        right->node_t::left = node;
    }

    static void remove(T *left, T *right) noexcept {
        left->node_t::right = right;
        right->node_t::left = left;
    }

    static void remove(T *node) noexcept {
        remove(node->node_t::left, node->node_t::right);
    }

    void push_front(T *node) noexcept {
        T *mhead = mock_head();
        insert(node, mhead, mhead->node_t::right);
    }

    void push_back(T *node) noexcept {
        T *mhead = mock_head();
        insert(node, mhead->node_t::left, mhead);
    }

    T *pop_front() noexcept {
        T *mhead = mock_head();
        T *right = mhead->node_t::right;
        if (right == mhead) [[unlikely]] {
            return nullptr;
        }
        remove(mhead, right->node_t::right);
        return right;
    }

    T *pop_back() noexcept {
        T *mhead = mock_head();
        T *left = mhead->node_t::left;
        if (left == mhead) [[unlikely]] {
            return nullptr;
        }
        remove(left->node_t::left, mhead);
        return left;
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
                current = current->node_t::right;
            } else {
                current = current->node_t::left;
            }
            return *this;
        }

        iterator_t operator++(int) noexcept {
            pointer old = current;
            if constexpr (!is_reverse) {
                current = current->node_t::right;
            } else {
                current = current->node_t::left;
            }
            return iterator_t{old};
        }

        iterator_t &operator--() noexcept {
            if constexpr (!is_reverse) {
                current = current->node_t::left;
            } else {
                current = current->node_t::right;
            }
            return *this;
        }

        iterator_t operator--(int) noexcept {
            pointer old = current;
            if constexpr (!is_reverse) {
                current = current->node_t::left;
            } else {
                current = current->node_t::right;
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
        return const_iterator{head.right};
    }

    iterator begin() noexcept {
        return iterator{head.right};
    }

    const_iterator end() const noexcept {
        return const_iterator{const_mock_head()};
    }

    iterator end() noexcept {
        return iterator{mock_head()};
    }

    const_iterator cbegin() const noexcept {
        return const_iterator{head.right};
    }

    const_iterator cend() const noexcept {
        return const_iterator{const_mock_head()};
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{head.left};
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator{head.left};
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{const_mock_head()};
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator{mock_head()};
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{head.left};
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{const_mock_head()};
    }
   private:
    void move_from(idlist &&other) noexcept {
        if (other.empty()) [[unlikely]] {
            clear();
        } else {
            head = other.head;

            T *mhead = mock_head();
            head.right->node_t::left = mhead;
            head.left->node_t::right = mhead;

            other.clear();
        }
    }

    [[nodiscard]]
    T *mock_head() noexcept {
        return static_cast<T *>(&head);
    }

    [[nodiscard]]
    const T *const_mock_head() const noexcept {
        return static_cast<const T *>(&head);
    }

    idnode<T> head;
};

} // namespace uit