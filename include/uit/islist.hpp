// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_ISLIST_1838F0F2_B823_46A0_B1F1_2796453C214C
#define UIT_ISLIST_1838F0F2_B823_46A0_B1F1_2796453C214C
#include <iterator>
#include <uit/intrusive.hpp>

namespace uit {

template <auto Right>
class islist;

template <typename T, typename MT, MT T::* Right>
class islist<Right> {
   public:
    islist() noexcept {
        m_right = nullptr;
    }

    islist(const islist& other) noexcept = default;

    islist& operator=(const islist& other) noexcept = default;

    islist(islist&& other) noexcept {
        m_right = other.m_right;
        other.clear();
    }

    islist& operator=(islist&& other) noexcept {
        if (this != &other) {
            m_right = other.m_right;
            other.clear();
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
        node->*Right = m_right;
        m_right = node;
    }

    T* pop_front() noexcept {
        T* first = m_right;
        if (first == nullptr) [[unlikely]] {
            return nullptr;
        }
        m_right = first->*Right;
        return first;
    }

    T* remove(T* node) noexcept {
        T** left = &m_right;
        for (T* right = m_right; right != nullptr;) {
            if (right == node) {
                *left = right->*Right;
                return node;
            }
            left = &(right->*Right);
            right = right->*Right; // Equivalent to "right = *left;"
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

       private:
        pointer current{nullptr};
    };

    using iterator = iterator_t<T>;
    using const_iterator = iterator_t<const T>;

    const_iterator begin() const {
        return const_iterator{m_right};
    }

    iterator begin() {
        return iterator{m_right};
    }

    const_iterator end() const {
        return const_iterator{nullptr};
    }

    iterator end() {
        return iterator{nullptr};
    }

    const_iterator cbegin() const {
        return const_iterator{m_right};
    }

    const_iterator cend() const {
        return const_iterator{nullptr};
    }
   private:
    T* m_right;
};

} // namespace uit
#endif // islist.hpp