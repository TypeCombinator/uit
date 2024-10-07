#pragma once
#include <iterator>
#include "intrusive.hpp"

namespace uit {

template <auto M>
class islist;

template <typename T, isnode<T>(T::*M)>
class islist<M> {
   public:
    islist() noexcept {
        head.right = nullptr;
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return head.right == nullptr;
    }

    void clear() noexcept {
        head.right = nullptr;
    }

    [[nodiscard]]
    T& front() const noexcept {
        return *head.right;
    }

    void push_front(T* node) noexcept {
        (node->*M).right = head.right;
        head.right = node;
    }

    T* pop_front() noexcept {
        T* first = head.right;
        if (first == nullptr) [[unlikely]] {
            return nullptr;
        }
        head.right = (first->*M).right;
        return first;
    }

    T* remove(T* node) noexcept {
        T* left = mock_head();
        for (T* right = head.right; right != nullptr;) {
            if (right == node) {
                (left->*M).right = (right->*M).right;
                return node;
            }
            left = right;
            right = (right->*M).right;
        }
        return nullptr;
    }

    T* remove_without_ub(T* node) noexcept {
        T** left = &head.right;
        for (T* right = head.right; right != nullptr;) {
            if (right == node) {
                *left = (right->*M).right;
                return node;
            }
            left = &(right->*M).right;
            right = (right->*M).right; // Equivalent to "right = *left;"
        }
        return nullptr;
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
        reference
            operator*() noexcept {
            return *current;
        }

        [[nodiscard]]
        reference
            operator*() const noexcept {
            return *current;
        }

        [[nodiscard]]
        pointer
            operator->() noexcept {
            return current;
        }

        [[nodiscard]]
        pointer
            operator->() const noexcept {
            return current;
        }

        iterator_t& operator++() noexcept {
            current = (current->*M).right;
            return *this;
        }

        iterator_t operator++(int) noexcept {
            pointer old = current;
            current = (current->*M).right;
            return iterator_t{old};
        }

        bool operator==(const iterator_t& other) const noexcept {
            return current == other.current;
        }

       private:
        pointer current{nullptr};
    };

    using iterator = iterator_t<T>;
    using const_iterator = iterator_t<const T>;

    const_iterator begin() const {
        return const_iterator{head.right};
    }

    iterator begin() {
        return iterator{head.right};
    }

    const_iterator end() const {
        return const_iterator{nullptr};
    }

    iterator end() {
        return iterator{nullptr};
    }

    const_iterator cbegin() const {
        return const_iterator{head.right};
    }

    const_iterator cend() const {
        return const_iterator{nullptr};
    }
   private:
    [[nodiscard]]
    T* mock_head() noexcept {
        // UB!!!
        return container_of(M, &head);
    }

    [[nodiscard]]
    const T* const_mock_head() const noexcept {
        // UB!!!
        return const_container_of(M, &head);
    }

    isnode<T> head;
};

} // namespace uit