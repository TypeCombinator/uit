// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_IRWBT_E773160D_0F94_4DD2_8A57_6FB1F0D3A109
#define UIT_IRWBT_E773160D_0F94_4DD2_8A57_6FB1F0D3A109
#include <uit/intrusive.hpp>
#include <uit/detail/top_down_queue.hpp>
#include <cstdint>
#include <functional>

// References:
// [0] Yoichi Hirai and Kazuhiko Yamamoto. Balancing weight-balanced trees. 2011.
// [1] Lukas Barth and Dorothea Wagner. Engineering Top-Down Weight-Balanced Trees.
// Notices:
// [0] The acronym irsbt stands for intrusive recursive size-balanced tree.
// [1] The mock sentinel will involve UB, but the code works correctly.
// [2] Nodes in the tree don‘t have parent pointers, so some operations can only be implemented
// recursively.
namespace uit {
template <auto Right, auto Left, auto Size, typename CMP = std::less<>>
struct irwbt;

template <typename T, typename MT, MT T::*Right, MT T::*Left, auto Size, typename CMP>
struct irwbt<Right, Left, Size, CMP> {
   public:
    using np_t = T *;
    using cnp_t = const T *;
    using nsize_t = uit::member_t<Size>;

    irwbt() noexcept {
        head = mock_sentinel();
    }

    [[nodiscard]]
    static bool is_sentinel(const T *node) noexcept {
        return node == const_mock_sentinel();
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return is_sentinel(head);
    }

    void clear() noexcept {
        head = mock_sentinel();
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return head->*Size;
    }

    void insert_multi(np_t node) noexcept {
        np_t *cur_ptr = &head;
        np_t cur = head;
        if (is_sentinel(cur)) [[unlikely]] {
            insert_leaf(*cur_ptr, node);
            return;
        }
        (cur->*Size)++;
        while (1) {
            if (cmp(*node, *cur)) { // l
                np_t left = cur->*Left;
                if (!is_sentinel(left)) [[likely]] { // look-ahead-1
                    (left->*Size)++;
                    if ((cur->*Right->*Size * 3 + 1) < left->*Size) [[unlikely]] {
                        bool is_ll = cmp(*node, *left);
                        nsize_t ll_size = is_ll ? (left->*Left->*Size + 1) : left->*Left->*Size;
                        np_t *ptr = cur_ptr;
                        // lr.S = l.S - ll.S -1
                        if (ll_size * 2 < (left->*Size - ll_size)) { // double-rotate
                            if (is_ll) {                             // ll
                                np_t ll = left->*Left;
                                if (!is_sentinel(ll)) [[likely]] { // look-ahead-2
                                    (ll->*Size)++;
                                    cur_ptr = &(left->*Left);
                                } else {
                                    insert_leaf(left->*Left, node);
                                    cur_ptr = nullptr;
                                }
                            } else { // lr
                                np_t lr = left->*Right;
                                if (!is_sentinel(lr)) [[likely]] { // look-ahead-2
                                    (lr->*Size)++;
                                    if (!cmp(*node, *lr)) {                        // lrr
                                        if (!is_sentinel(lr->*Right)) [[likely]] { // look-ahead-3
                                            (lr->*Right->*Size)++;
                                            cur_ptr = &(cur->*Left);
                                        } else {
                                            insert_leaf(lr->*Right, node);
                                            cur_ptr = nullptr;
                                        }
                                    } else {                                      // lrl
                                        if (!is_sentinel(lr->*Left)) [[likely]] { // look-ahead-3
                                            (lr->*Left->*Size)++;
                                            cur_ptr = &(left->*Right);
                                        } else {
                                            insert_leaf(lr->*Left, node);
                                            cur_ptr = nullptr;
                                        }
                                    }
                                } else {
                                    insert_leaf(left->*Right, node);
                                    cur_ptr = nullptr;
                                }
                            }
                            left_rotate(cur->*Left);
                            right_rotate(*ptr);
                        } else {         // single-rotate
                            if (is_ll) { // ll
                                np_t ll = left->*Left;
                                if (!is_sentinel(ll)) [[likely]] { // look-ahead-2
                                    (ll->*Size)++;
                                    cur_ptr = &(left->*Left);
                                } else {
                                    insert_leaf(left->*Left, node);
                                    cur_ptr = nullptr;
                                }
                            } else { // lr
                                np_t lr = left->*Right;
                                if (!is_sentinel(lr)) [[likely]] { // look-ahead-2
                                    (lr->*Size)++;
                                    cur_ptr = &(cur->*Left);
                                } else {
                                    insert_leaf(left->*Right, node);
                                    cur_ptr = nullptr;
                                }
                            }
                            right_rotate(*ptr);
                        }
                        if (cur_ptr == nullptr) [[unlikely]] {
                            return;
                        }
                        cur = *cur_ptr;
                    } else {
                        cur_ptr = &(cur->*Left);
                        cur = left;
                    }
                } else {
                    insert_leaf(cur->*Left, node);
                    return;
                }
            } else { // r
                np_t right = cur->*Right;
                if (!is_sentinel(right)) [[likely]] { // look-ahead-1
                    (right->*Size)++;
                    if ((cur->*Left->*Size * 3 + 1) < right->*Size) [[unlikely]] {
                        bool is_rr = !cmp(*node, *right);
                        nsize_t rr_size = is_rr ? (right->*Right->*Size + 1) : right->*Right->*Size;
                        np_t *ptr = cur_ptr;
                        // rl.S = r.S - rr.S -1
                        if (rr_size * 2 < (right->*Size - rr_size)) { // double-rotate
                            if (is_rr) {                              // rr
                                np_t rr = right->*Right;
                                if (!is_sentinel(rr)) [[likely]] { // look-ahead-2
                                    (rr->*Size)++;
                                    cur_ptr = &(right->*Right);
                                } else {
                                    insert_leaf(right->*Right, node);
                                    cur_ptr = nullptr;
                                }
                            } else { // rl
                                np_t rl = right->*Left;
                                if (!is_sentinel(rl)) [[likely]] { // look-ahead-2
                                    (rl->*Size)++;
                                    if (cmp(*node, *rl)) {                        // rll
                                        if (!is_sentinel(rl->*Left)) [[likely]] { // look-ahead-3
                                            (rl->*Left->*Size)++;
                                            cur_ptr = &(cur->*Right);
                                        } else {
                                            insert_leaf(rl->*Left, node);
                                            cur_ptr = nullptr;
                                        }
                                    } else {                                       // rlr
                                        if (!is_sentinel(rl->*Right)) [[likely]] { // look-ahead-3
                                            (rl->*Right->*Size)++;
                                            cur_ptr = &(right->*Left);
                                        } else {
                                            insert_leaf(rl->*Right, node);
                                            cur_ptr = nullptr;
                                        }
                                    }
                                } else {
                                    insert_leaf(right->*Left, node);
                                    cur_ptr = nullptr;
                                }
                            }
                            right_rotate(cur->*Right);
                            left_rotate(*ptr);
                        } else {         // single-rotate
                            if (is_rr) { // rr
                                np_t rr = right->*Right;
                                if (!is_sentinel(rr)) [[likely]] { // look-ahead-2
                                    (rr->*Size)++;
                                    cur_ptr = &(right->*Right);
                                } else {
                                    insert_leaf(right->*Right, node);
                                    cur_ptr = nullptr;
                                }
                            } else { // rl
                                np_t rl = right->*Left;
                                if (!is_sentinel(rl)) [[likely]] { // look-ahead-2
                                    (rl->*Size)++;
                                    cur_ptr = &(cur->*Right);
                                } else {
                                    insert_leaf(right->*Left, node);
                                    cur_ptr = nullptr;
                                }
                            }
                            left_rotate(*ptr);
                        }
                        if (cur_ptr == nullptr) [[unlikely]] {
                            return;
                        }
                        cur = *cur_ptr;
                    } else {
                        cur_ptr = &(cur->*Right);
                        cur = right;
                    }
                } else {
                    insert_leaf(cur->*Right, node);
                    return;
                }
            }
        }
    }

    void insert_multi_with_queue(np_t node) noexcept {
        detail::top_down_queue<T> q{};
        np_t *cur_ptr = &head;
        np_t cur = head;
        while (!is_sentinel(cur)) {
            (cur->*Size)++;
            if (cmp(*node, *cur)) {
                q.push_left_path(cur_ptr);
                cur_ptr = &(cur->*Left);
            } else {
                q.push_right_path(cur_ptr);
                cur_ptr = &(cur->*Right);
            }
            if (q.size() > 3) {
                top_down_insert_mainatin(q);
            }
            cur = *cur_ptr;
        }
        insert_leaf(*cur_ptr, node);
        while (q.size() > 1) {
            top_down_insert_mainatin(q);
        }
    }

    // insert unique
    bool insert(np_t node) noexcept {
        np_t *cur_ptr = &head;
        np_t cur = head;
        uint64_t path = 1; // Set a sentinel bit
        using stack_t = np_t *;
        stack_t stack[sizeof(path) * 8];
        stack_t *stack_ptr = &stack[0];

        while (!is_sentinel(cur)) {
            path <<= 1;
            *stack_ptr++ = cur_ptr;
            if (CMP{}(*node, *cur)) {
                cur_ptr = &(cur->*Left);
            } else if (CMP{}(*cur, *node)) {
                path |= 1;
                cur_ptr = &(cur->*Right);
            } else {
                return false;
            }
            cur = *cur_ptr;
        }
        insert_leaf(*cur_ptr, node);
        stack_ptr--;
        if (path > 1) [[likely]] {
            // The parent node of the new leaf node is balanced.
            ((**stack_ptr)->*Size)++;
            path >>= 1;
            stack_ptr--;
        }
        while (path > 1) {
            cur_ptr = *stack_ptr;
            ((*cur_ptr)->*Size)++;
            if (path & 1u) {
                maintain_right_leaning(*cur_ptr);
            } else {
                maintain_left_leaning(*cur_ptr);
            }
            path >>= 1;
            stack_ptr--;
        }
        return true;
    }

    // It's UB when the tree is empty, so you must check for emptiness before calling this function.
    np_t remove_leftmost() noexcept {
        np_t *cur_ptr = &head;
        np_t cur = head;

        (cur->*Size)--;
        while (!is_sentinel(cur->*Left)) {
            (cur->*Left->*Size)--;
            maintain_right_leaning(*cur_ptr);
            cur_ptr = &(cur->*Left);
            cur = *cur_ptr;
        }
        *cur_ptr = cur->*Right;
        return cur;
    }

    template <typename K>
    np_t remove(const K &node) noexcept {
        np_t cur = head;
        uint64_t path{};

        for (unsigned i{}; !is_sentinel(cur); i++) {
            if (cmp(node, *cur)) {
                cur = cur->*Left;
            } else if (cmp(*cur, node)) {
                cur = cur->*Right;
                path |= (uint64_t{1} << i);
            } else {
                path |= (uint64_t{1} << i); // Set a sentinel bit.

                np_t *cur_ptr = &head;
                cur = *cur_ptr;
                (cur->*Size)--;
                while (path > 1) {
                    if (path & 1) {
                        (cur->*Right->*Size)--;
                        maintain_left_leaning(*cur_ptr);
                        cur_ptr = &(cur->*Right);
                    } else {
                        (cur->*Left->*Size)--;
                        maintain_right_leaning(*cur_ptr);
                        cur_ptr = &(cur->*Left);
                    }
                    cur = *cur_ptr;
                    path >>= 1;
                }
                np_t right = cur->*Right;
                if (is_sentinel(right)) [[unlikely]] {
                    *cur_ptr = cur->*Left;
                } else {
                    if (is_sentinel(right->*Left)) [[unlikely]] {
                        right->*Left = cur->*Left;
                        right->*Size = cur->*Size;
                        *cur_ptr = right;
                    } else {
                        np_t leftmost = top_down_wremove_leftmost_for_remove(&(cur->*Right));
                        leftmost->*Right = cur->*Right;
                        leftmost->*Left = cur->*Left;
                        leftmost->*Size = cur->*Size;
                        *cur_ptr = leftmost;
                    }
                    maintain_left_leaning(*cur_ptr);
                }
                return cur;
            }
        }
        return nullptr;
    }

    static bool validate_sentinel() noexcept {
        auto s = const_mock_sentinel();
        return (s->*Right == s) && (s->*Left == s) && (s->*Size == 0);
    }
   private:
    static void top_down_insert_mainatin(detail::top_down_queue<T> &q) noexcept {
        auto cur_ptr = q.front_pointer();
        np_t cur = *cur_ptr;
        if (q.path_queue() & 1u) { // Right?
            if ((cur->*Left->*Size * 3 + 1) < cur->*Right->*Size) {
                if (cur->*Right->*Right->*Size * 2 < (cur->*Right->*Left->*Size + 1)) {
                    right_rotate(cur->*Right);
                    left_rotate(*cur_ptr);

                    if ((q.path_queue() & 2u) == 0) { // rl
                        cur = *cur_ptr;
                        if (q.path_queue() & 4u) { // rlr
                            q.template pop<3>();
                            q.set_front(&(cur->*Right->*Left));
                        } else { // rll
                            q.template pop<3>();
                            q.set_front(&(cur->*Left->*Right));
                        }
                    } else { // rr
                        q.template pop<2>();
                    }
                } else {
                    left_rotate(*cur_ptr);

                    if (q.path_queue() & 2u) { // rr
                        q.template pop<2>();
                    } else { // rl
                        q.template pop<2>();
                        q.set_front(&((*cur_ptr)->*Left->*Right));
                    }
                }
            } else {
                q.pop();
            }
        } else { // Left
            if ((cur->*Right->*Size * 3 + 1) < cur->*Left->*Size) {
                if (cur->*Left->*Left->*Size * 2 < (cur->*Left->*Right->*Size + 1)) {
                    left_rotate(cur->*Left);
                    right_rotate(*cur_ptr);

                    if (q.path_queue() & 2u) { // lr
                        cur = *cur_ptr;
                        if ((q.path_queue() & 4u) == 0) { // lrl
                            q.template pop<3>();
                            q.set_front(&(cur->*Left->*Right));
                        } else { // lrr
                            q.template pop<3>();
                            q.set_front(&(cur->*Right->*Left));
                        }
                    } else { // ll
                        q.template pop<2>();
                    }
                } else {
                    right_rotate(*cur_ptr);

                    if ((q.path_queue() & 2u) == 0u) { // ll
                        q.template pop<2>();
                    } else { // lr
                        q.template pop<2>();
                        q.set_front(&((*cur_ptr)->*Right->*Left));
                    }
                }
            } else {
                q.pop();
            }
        }
    }

    // It's UB when the left child is null.
    static np_t top_down_wremove_leftmost_for_remove(np_t *cur_ptr) noexcept {
        np_t cur = *cur_ptr;
        (cur->*Size)--;
        do {
            (cur->*Left->*Size)--;
            maintain_right_leaning(*cur_ptr);
            cur_ptr = &(cur->*Left);
            cur = *cur_ptr;
        } while (!is_sentinel(cur->*Left));
        *cur_ptr = cur->*Right;
        return cur;
    }

    static void left_rotate(np_t &n) noexcept {
        np_t s = n->*Right;

        n->*Right = s->*Left;
        s->*Left = n;
        s->*Size = n->*Size;
        n->*Size = n->*Right->*Size + n->*Left->*Size + 1;
        n = s;
    }

    static void right_rotate(np_t &n) noexcept {
        np_t s = n->*Left;

        n->*Left = s->*Right;
        s->*Right = n;
        s->*Size = n->*Size;
        n->*Size = n->*Right->*Size + n->*Left->*Size + 1;
        n = s;
    }

    static void maintain_right_leaning(np_t &root) noexcept {
        if ((root->*Left->*Size * 3 + 1) < (root->*Right->*Size)) {
            if (root->*Right->*Right->*Size * 2 < (root->*Right->*Left->*Size + 1)) {
                right_rotate(root->*Right);
            }
            left_rotate(root);
        }
    }

    static void maintain_left_leaning(np_t &root) noexcept {
        if ((root->*Right->*Size * 3 + 1) < root->*Left->*Size) {
            if (root->*Left->*Left->*Size * 2 < (root->*Left->*Right->*Size + 1)) {
                left_rotate(root->*Left);
            }
            right_rotate(root);
        }
    }

    static void insert_leaf(np_t &cur, np_t node) {
        cur = node;
        node->*Size = 1;
        node->*Right = mock_sentinel();
        node->*Left = mock_sentinel();
    }

    union sentinel_t {
        constexpr sentinel_t() noexcept {
            // UB!!! The lifetime of storage has not yet started.
            storage.*Right = &storage;
            storage.*Left = &storage;
            storage.*Size = 0;
        }

        T storage;
        unsigned char buffer[sizeof(T)];
    };

    static inline const sentinel_t sentinel{};

    static T *mock_sentinel() noexcept {
        return const_cast<T *>(&sentinel.storage);
    }

    static const T *const_mock_sentinel() noexcept {
        return &sentinel.storage;
    }

    // TODO: need a macro for the msvc.
    [[no_unique_address]]
    CMP cmp;
    T *head;
};
} // namespace uit
#endif // irwbt.hpp