// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_IDSLIST_16F6335A_28C5_45A0_8DF3_66A706C1714A
#define UIT_IDSLIST_16F6335A_28C5_45A0_8DF3_66A706C1714A
#include <iterator>
#include <uit/intrusive.hpp>

namespace uit {

template <auto Right>
class idslist;

template <typename T, typename MT, MT T::*Right>
class idslist<Right> {
   public:
    idslist() noexcept {
        m_right = nullptr;
        m_left = mock_head();
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
        return m_right == nullptr;
    }

    void clear() noexcept {
        m_right = nullptr;
        m_left = mock_head();
    }

    [[nodiscard]]
    T &front() const noexcept {
        return *m_right;
    }

    [[nodiscard]]
    T &back() const noexcept {
        return *m_left;
    }

    void push_front(T *node) noexcept {
        if (m_right == nullptr) {
            m_left = node;
        }
        node->*Right = m_right;
        m_right = node;
    }

    void push_back(T *node) noexcept {
        node->*Right = nullptr;
        m_left->*Right = node;
        m_left = node;
    }

    T *pop_front() noexcept {
        T *first = m_right;
        if (first == nullptr) [[unlikely]] {
            return nullptr;
        }
        T *first_right = first->*Right;
        // Tail?
        if (first_right == nullptr) [[unlikely]] {
            m_left = mock_head();
        }
        m_right = first_right;
        return first;
    }

    T *remove(T *node) noexcept {
        T *left = mock_head();
        for (T *right = left->*Right; right != nullptr;) {
            if (right == node) {
                right = right->*Right;
                left->*Right = right;
                // Tail?
                if (right == nullptr) [[unlikely]] {
                    m_left = left;
                }
                return node;
            }
            left = right;
            right = left->*Right;
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
            current = current->*Right;
            return *this;
        }

        iterator_t operator++(int) noexcept {
            pointer old = current;
            current = current->*Right;
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
    void copy_from(const idslist &other) noexcept {
        if (other.empty()) [[unlikely]] {
            clear();
        } else {
            m_right = other.m_right;
            m_left = other.m_left;
        }
    }

    void move_from(idslist &&other) noexcept {
        if (other.empty()) [[unlikely]] {
            clear();
        } else {
            m_right = other.m_right;
            m_left = other.m_left;
            other.clear();
        }
    }

    [[nodiscard]]
    T *mock_head() noexcept {
        // UB!!!
        return container_of(Right, &m_right);
    }

    [[nodiscard]]
    const T *const_mock_head() const noexcept {
        // UB!!!
        return const_container_of(Right, &m_right);
    }

    T *m_right;
    T *m_left;
};

} // namespace uit
#endif // idslist.hpp