#pragma once
#include <iterator>
#include "intrusive.hpp"

namespace uit {

template <typename T, stag N = "">
class islist {
   public:
    using node_t = isnode<T, N>;

    islist() noexcept {
        head.right = nullptr;
    }

    islist(const islist& other) noexcept = default;

    islist& operator=(const islist& other) noexcept = default;

    islist(islist&& other) noexcept {
        head = other.head;
        other.clear();
    }

    islist& operator=(islist&& other) noexcept {
        if (this != &other) {
            head = other.head;
            other.clear();
        }
        return *this;
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
        node->node_t::right = head.right;
        head.right = node;
    }

    T* pop_front() noexcept {
        T* first = head.right;
        if (first == nullptr) [[unlikely]] {
            return nullptr;
        }
        head.right = first->node_t::right;
        return first;
    }

    T* remove(T* node) noexcept {
        T* left = mock_head();
        for (T* right = head.right; right != nullptr;) {
            if (right == node) {
                left->node_t::right = right->node_t::right;
                return node;
            }
            left = right;
            right = right->node_t::right;
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
            current = current->node_t::right;
            return *this;
        }

        iterator_t operator++(int) noexcept {
            pointer old = current;
            current = current->node_t::right;
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
        return static_cast<T*>(&head);
    }

    [[nodiscard]]
    const T* const_mock_head() const noexcept {
        return static_cast<const T*>(&head);
    }

    isnode<T, N> head;
};

} // namespace uit