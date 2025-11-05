// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#pragma once
#include <iterator>
#include "intrusive.hpp"

namespace uit {

template <auto M>
class idslist;

template <typename T, typename MT, MT(T::*M)>
    requires std::is_base_of_v<isnode<T>, MT>
class idslist<M> {
   public:
    idslist() noexcept {
        head.right = nullptr;
        head.left = mock_head();
    }

    idslist(const idslist &other) noexcept {
        copy_from(other);
    }

    idslist &operator=(const idslist &other) noexcept {
        if (this != &other) [[likely]] {
            copy_from(other);
        }
        return *this;
    }

    idslist(idslist &&other) noexcept {
        move_from(std::move(other));
    }

    idslist &operator=(idslist &&other) noexcept {
        if (this != &other) [[likely]] {
            move_from(std::move(other));
        }
        return *this;
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return head.right == nullptr;
    }

    void clear() noexcept {
        head.right = nullptr;
        head.left = mock_head();
    }

    [[nodiscard]]
    T &front() const noexcept {
        return *head.right;
    }

    [[nodiscard]]
    T &back() const noexcept {
        return *head.left;
    }

    void push_front(T *node) noexcept {
        if (head.right == nullptr) {
            head.left = node;
        }
        (node->*M).right = head.right;
        head.right = node;
    }

    void push_back(T *node) noexcept {
        (node->*M).right = nullptr;
        (head.left->*M).right = node;
        head.left = node;
    }

    T *pop_front() noexcept {
        T *first = head.right;
        if (first == nullptr) [[unlikely]] {
            return nullptr;
        }
        T *first_right = (first->*M).right;
        // Tail?
        if (first_right == nullptr) [[unlikely]] {
            head.left = mock_head();
        }
        head.right = first_right;
        return first;
    }

    T *remove(T *node) noexcept {
        T *left = mock_head();
        for (T *right = (left->*M).right; right != nullptr;) {
            if (right == node) {
                right = (right->*M).right;
                (left->*M).right = right;
                // Tail?
                if (right == nullptr) [[unlikely]] {
                    head.left = left;
                }
                return node;
            }
            left = right;
            right = (left->*M).right;
        }
        return nullptr;
    }

    template <typename T_CV>
    struct iterator_t {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T_CV;
        using difference_type = std::ptrdiff_t;
        using pointer = T_CV *;
        using reference = T_CV &;

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

        iterator_t &operator++() noexcept {
            current = (current->*M).right;
            return *this;
        }

        iterator_t operator++(int) noexcept {
            pointer old = current;
            current = (current->*M).right;
            return iterator_t{old};
        }

        bool operator==(const iterator_t &other) const noexcept {
            return current == other.current;
        }

       private:
        pointer current{nullptr};
    };

    using iterator = iterator_t<T>;
    using const_iterator = iterator_t<const T>;

    iterator begin() const noexcept {
        return iterator{head.right};
    }

    iterator begin() noexcept {
        return iterator{head.right};
    }

    iterator end() const noexcept {
        return iterator{nullptr};
    }

    iterator end() noexcept {
        return iterator{nullptr};
    }

    const_iterator cbegin() const noexcept {
        return const_iterator{head.right};
    }

    const_iterator cend() const noexcept {
        return const_iterator{nullptr};
    }

   private:
    void copy_from(const idslist &other) noexcept {
        if (other.empty()) [[unlikely]] {
            clear();
        } else {
            head = other.head;
        }
    }

    void move_from(idslist &&other) noexcept {
        if (other.empty()) [[unlikely]] {
            clear();
        } else {
            head = other.head;
            other.clear();
        }
    }

    [[nodiscard]]
    T *mock_head() noexcept {
        // UB!!!
        return container_of(M, static_cast<MT *>(static_cast<isnode<T> *>(&head)));
    }

    [[nodiscard]]
    const T *const_mock_head() const noexcept {
        // UB!!!
        return const_container_of(M, static_cast<const MT *>(static_cast<const isnode<T> *>(&head)));
    }

    idnode<T> head;
};

} // namespace uit