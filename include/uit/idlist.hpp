// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_IDLIST_217E7022_9D7F_4924_BF85_F58D26EC0395
#define UIT_IDLIST_217E7022_9D7F_4924_BF85_F58D26EC0395
#include <iterator>
#include <uit/intrusive.hpp>

namespace uit {

template <auto Right, auto Left>
class idlist;

template <typename T, typename MT, MT T::*Right, MT T::*Left>
class idlist<Right, Left> {
   public:
    idlist() noexcept {
        m_left = m_right = mock_head();
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
        return mhead == mhead->*Right;
    }

    void clear() noexcept {
        m_left = m_right = mock_head();
    }

    [[nodiscard]]
    T &front() const noexcept {
        return *m_right;
    }

    [[nodiscard]]
    T &back() const noexcept {
        return *m_left;
    }

    static void insert(T *node, T *left, T *right) noexcept {
        node->*Right = right;
        node->*Left = left;

        left->*Right = node;
        right->*Left = node;
    }

    static void remove(T *left, T *right) noexcept {
        left->*Right = right;
        right->*Left = left;
    }

    static void remove(T *node) noexcept {
        remove(node->*Left, node->*Right);
    }

    void push_front(T *node) noexcept {
        T *mhead = mock_head();
        insert(node, mhead, mhead->*Right);
    }

    void push_back(T *node) noexcept {
        T *mhead = mock_head();
        insert(node, mhead->*Left, mhead);
    }

    T *pop_front() noexcept {
        T *mhead = mock_head();
        T *right = mhead->*Right;
        if (right == mhead) [[unlikely]] {
            return nullptr;
        }
        remove(mhead, right->*Right);
        return right;
    }

    T *pop_back() noexcept {
        T *mhead = mock_head();
        T *left = mhead->*Left;
        if (left == mhead) [[unlikely]] {
            return nullptr;
        }
        remove(left->*Left, mhead);
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
                current = current->*Right;
            } else {
                current = current->*Left;
            }
            return *this;
        }

        iterator_t operator++(int) noexcept {
            pointer old = current;
            if constexpr (!is_reverse) {
                current = current->*Right;
            } else {
                current = current->*Left;
            }
            return iterator_t{old};
        }

        iterator_t &operator--() noexcept {
            if constexpr (!is_reverse) {
                current = current->*Left;
            } else {
                current = current->*Right;
            }
            return *this;
        }

        iterator_t operator--(int) noexcept {
            pointer old = current;
            if constexpr (!is_reverse) {
                current = current->*Left;
            } else {
                current = current->*Right;
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
        return const_iterator{const_mock_head()->*Right};
    }

    iterator begin() noexcept {
        return iterator{mock_head()->*Right};
    }

    const_iterator end() const noexcept {
        return const_iterator{const_mock_head()};
    }

    iterator end() noexcept {
        return iterator{mock_head()};
    }

    const_iterator cbegin() const noexcept {
        return const_iterator{const_mock_head()->*Right};
    }

    const_iterator cend() const noexcept {
        return const_iterator{const_mock_head()};
    }

    const_reverse_iterator rbegin() const noexcept {
        return const_reverse_iterator{const_mock_head()->*Left};
    }

    reverse_iterator rbegin() noexcept {
        return reverse_iterator{mock_head()->*Left};
    }

    const_reverse_iterator rend() const noexcept {
        return const_reverse_iterator{const_mock_head()};
    }

    reverse_iterator rend() noexcept {
        return reverse_iterator{mock_head()};
    }

    const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{const_mock_head()->*Left};
    }

    const_reverse_iterator crend() const noexcept {
        return const_reverse_iterator{const_mock_head()};
    }
   private:
    void move_from(idlist &&other) noexcept {
        if (other.empty()) [[unlikely]] {
            clear();
        } else {
            m_right = other.m_right;
            m_left = other.m_left;

            T *mhead = mock_head();
            m_right->*Left = mhead;
            m_left->*Right = mhead;

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
#endif // idlist.hpp