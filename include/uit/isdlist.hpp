#pragma once
#include <iterator>
#include "intrusive.hpp"

namespace uit {

template <typename T, stag N = "">
class isdlist {
   public:
    using node_t = idnode<T, N>;

    isdlist() noexcept {
        head.right = nullptr;
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
        T* first;
        first = head.right;

        node->node_t::right = first;
        node->node_t::left = mock_head();

        head.right = node;
        if (first != nullptr) {
            first->node_t::left = node;
        }
    }

    static void remove(T* node) noexcept {
        T* right = node->node_t::right;
        T* left = node->node_t::left;

        if (right != nullptr) {
            right->node_t::left = left;
        }
        left->node_t::right = right;
    }

    T* pop_front() noexcept {
        T* right = head.right;
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

        bool operator!=(const iterator_t& other) const noexcept {
            return current != other.current;
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
    void move_from(isdlist&& other) noexcept {
        if (other.empty()) [[unlikely]] {
            clear();
        } else {
            head = other.head;

            head.right->node_t::left = mock_head();

            other.clear();
        }
    }

    [[nodiscard]]
    T* mock_head() noexcept {
        return static_cast<T*>(static_cast<node_t*>(&head));
    }

    [[nodiscard]]
    const T* const_mock_head() const noexcept {
        return static_cast<const T*>(static_cast<const node_t*>(&head));
    }

    isnode<T, N> head;
};

} // namespace uit