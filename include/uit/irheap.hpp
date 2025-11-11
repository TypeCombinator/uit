// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_IRHEAP_001A9AE4_0EE4_4CF6_9871_6AE428316DD7
#define UIT_IRHEAP_001A9AE4_0EE4_4CF6_9871_6AE428316DD7
#include <functional>
#include <uit/bit.hpp>

namespace uit {
// An intrusive heap implementation without parent pointers in nodes, it's suitable for scenarios
// that don't require support for removing arbitrary node but only need to pop the top node.

// Notes:
// [0]: This implementation doesn't use mock_head, thus there's no risk of UB caused by it, it's
// safe for use.
// [1]: If support for arbitrary node removal is required, please refer to this PR:
// https://github.com/NVIDIA/stdexec/pull/1674
// [2]: The acronym irheap stands for intrusive recursive heap. While the initial prototype employed
// recursion, it soon dawned on me that an iterative approach would suffice, rendering recursion
// entirely unnecessary.
template <auto Right, auto Left, typename CMP = std::less<>>
class irheap;

template <typename T, typename MT, MT T::*Right, MT T::*Left, typename CMP>
class irheap<Right, Left, CMP> {
   public:
    using np_t = T *;
    using m_size_t = std::size_t;

    static constexpr m_size_t path_bit_mask = m_size_t{1} << (sizeof(m_size_t) * 8 - 1);

    // Note: The caller must ensure that the n is non-zero.
    static m_size_t path_bits(m_size_t n) noexcept {
        return ((n << 1) | 1u) << uit::countl_zero<m_size_t, true>(n);
    }

    irheap() noexcept
        : m_head{nullptr}
        , m_size{0} {
    }

    void push(np_t node) noexcept {
        m_size++;
        np_t *cur_ptr = &m_head;
        np_t cur = m_head;

        auto path = path_bits(m_size);

        while (path != path_bit_mask) {
            if (CMP{}(*node, *cur)) {
                *cur_ptr = node;
                do { // Replace one by one until reaching the bottom.
                    if (path & path_bit_mask) {
                        node->*Right = cur;
                        node->*Left = cur->*Left;
                        node = cur;
                        cur = cur->*Right;
                    } else {
                        node->*Right = cur->*Right;
                        node->*Left = cur;
                        node = cur;
                        cur = cur->*Left;
                    }
                    path <<= 1;
                } while (path != path_bit_mask);

                // The right child of the last node on the path is always null.
                // node->*Right = nullptr;
                node->*Left = nullptr;
                return;
            }
            if (path & path_bit_mask) {
                cur_ptr = &(cur->*Right);
            } else {
                cur_ptr = &(cur->*Left);
            }
            cur = *cur_ptr;
            path <<= 1;
        }

        *cur_ptr = node;
        node->*Right = nullptr;
        node->*Left = nullptr;
    }

    np_t pop() noexcept {
        np_t top = m_head;
        if (m_size <= 1u) [[unlikely]] {
            m_head = nullptr;
            m_size = 0;
            return top;
        }
        np_t cur = remove_last_leaf(&m_head, m_size);
        // Replace the top.
        cur->*Right = top->*Right;
        cur->*Left = top->*Left;
        m_head = cur;

        m_size--;
        sift_down(m_head, &m_head);
        return top;
    }

    [[nodiscard]]
    np_t front() const noexcept {
        return m_head;
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_head == nullptr;
    }

    [[nodiscard]]
    auto size() const noexcept {
        return m_size;
    }

    void clear() noexcept {
        *this = {};
    }
   private:
    static inline void swap_with_right_child(np_t cur, np_t child) noexcept {
        cur->*Right = child->*Right;
        child->*Right = cur;

        np_t t = cur->*Left;
        cur->*Left = child->*Left;
        child->*Left = t;
    }

    static inline void swap_with_left_child(np_t cur, np_t child) noexcept {
        np_t t = cur->*Right;
        cur->*Right = child->*Right;
        child->*Right = t;

        cur->*Left = child->*Left;
        child->*Left = cur;
    }

    static inline np_t remove_last_leaf(np_t *cur_ptr, m_size_t path) noexcept {
        np_t cur = *cur_ptr;
        path = path_bits(path);
        while (path != path_bit_mask) {
            if (path & path_bit_mask) {
                cur_ptr = &(cur->*Right);
            } else {
                cur_ptr = &(cur->*Left);
            }
            cur = *cur_ptr;
            path <<= 1;
        }
        *cur_ptr = nullptr;
        return cur;
    }

    static inline void sift_down(np_t cur, np_t *cur_ptr) noexcept {
        np_t left = cur->*Left;
        while (left != nullptr) {
            np_t right = cur->*Right;
            if ((right != nullptr) && CMP{}(*right, *left)) {
                if (CMP{}(*right, *cur)) {
                    swap_with_right_child(cur, right);
                    *cur_ptr = right;
                    cur_ptr = &(right->*Right);
                } else {
                    break;
                }
            } else {
                if (CMP{}(*left, *cur)) {
                    swap_with_left_child(cur, left);
                    *cur_ptr = left;
                    cur_ptr = &(left->*Left);
                } else {
                    break;
                }
            }
            left = cur->*Left;
        }
    }

    T *m_head; // A root pointer.
    m_size_t m_size;
};

} // namespace uit

#endif // irheap.hpp