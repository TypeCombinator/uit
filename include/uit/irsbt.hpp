// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#pragma once
#include <uit/intrusive.hpp>
#include <functional>

// References:
// [0] Chen Qifeng. Size Balanced Tree. 2006.
// [1] Yoichi Hirai and Kazuhiko Yamamoto. Balancing weight-balanced trees. 2011.
namespace uit {

template <typename T, stag N = "", typename CMP = std::less<>>
class irsbt {
   public:
    using node_t = irsbt_node<T, N>;
    using np_t = T *;

    irsbt() noexcept {
        head = mock_sentinel();
    }

    [[nodiscard]]
    static bool is_sentinel(const T *node) noexcept {
        return node == static_cast<const T *>(&sentinel);
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
        return head->node_t::size;
    }

    np_t insert_unique(np_t node) noexcept {
        return insert_unique_impl(head, node);
    }

    np_t winsert_unique(np_t node) noexcept {
        return winsert_unique_impl(head, node);
    }

    void insert_multi(np_t node) noexcept {
        insert_multi_impl(head, node);
    }

    void winsert_multi(np_t node) noexcept {
        winsert_multi_impl(head, node);
    }

    np_t remove_unique(const T &node) noexcept {
        // Remove without balance!
        return remove_unique_impl(head, node);
    }

    template <typename K>
        requires has_is_transparent<CMP>
    np_t remove_unique(const K &k) noexcept {
        return remove_unique_impl(head, k);
    }

    [[nodiscard]]
    np_t find(const T &node) const noexcept {
        return find_impl(head, node);
    }

    template <typename K>
        requires has_is_transparent<CMP>
    [[nodiscard]]
    np_t find(const K &k) const noexcept {
        return find_impl(head, k);
    }

    [[nodiscard]]
    np_t at(std::size_t pos) const noexcept {
        np_t root = head;

        while (!is_sentinel(root)) {
            std::size_t lsize = root->node_t::left->node_t::size;
            if (lsize == pos) {
                return root;
            } else if (lsize > pos) {
                root = root->node_t::left;
            } else {
                pos -= (lsize + 1);
                root = root->node_t::right;
            }
        }
        return nullptr;
    }

    // TODO: for multiset or multimap, position of the element is a range.
    [[nodiscard]]
    std::size_t position(const T &node) const noexcept {
        return position_impl(head, node);
    }

    template <typename K>
        requires has_is_transparent<CMP>
    [[nodiscard]]
    std::size_t position(const K &k) const noexcept {
        return position_impl(head, k);
    }

    [[nodiscard]]
    std::size_t height() const noexcept {
        return height_impl(head);
    }

    [[nodiscard]]
    std::size_t count_multi(const T &node) const noexcept {
        return count_multi_impl(head, node);
    }

    template <typename K>
        requires has_is_transparent<CMP>
    [[nodiscard]]
    std::size_t count_multi(const K &k) const noexcept {
        return count_multi_impl(head, k);
    }

   private:
    static void left_rotate(np_t &n) noexcept {
        np_t s = n->node_t::right;

        n->node_t::right = s->node_t::left;
        s->node_t::left = n;
        s->node_t::size = n->node_t::size;
        n->node_t::size = n->node_t::right->node_t::size + n->node_t::left->node_t::size + 1;
        n = s;
    }

    static void right_rotate(np_t &n) noexcept {
        np_t s = n->node_t::left;

        n->node_t::left = s->node_t::right;
        s->node_t::right = n;
        s->node_t::size = n->node_t::size;
        n->node_t::size = n->node_t::right->node_t::size + n->node_t::left->node_t::size + 1;
        n = s;
    }

    static void maintain(np_t &root, bool right_leaning) noexcept {
        if (right_leaning) {
            if (root->node_t::right->node_t::left->node_t::size
                > root->node_t::left->node_t::size) {
                right_rotate(root->node_t::right);
                left_rotate(root);
            } else if (
                root->node_t::right->node_t::right->node_t::size
                > root->node_t::left->node_t::size) {
                left_rotate(root);
            } else {
                return;
            }
        } else {
            if (root->node_t::left->node_t::right->node_t::size
                > root->node_t::right->node_t::size) {
                left_rotate(root->node_t::left);
                right_rotate(root);
            } else if (
                root->node_t::left->node_t::left->node_t::size
                > root->node_t::right->node_t::size) {
                right_rotate(root);
            } else {
                return;
            }
        }
        maintain(root->node_t::left, false);
        maintain(root->node_t::right, true);
        maintain(root, true);
        maintain(root, false);
    }

    template <auto DELTA = 3, auto GAMMA = 3>
    static void wmaintain(np_t &root, bool right_leaning) noexcept {
        if (right_leaning) {
            if ((root->node_t::left->node_t::size * DELTA + 1)
                < root->node_t::right->node_t::size) {
                if (root->node_t::right->node_t::right->node_t::size * GAMMA
                    < root->node_t::right->node_t::size) {
                    right_rotate(root->node_t::right);
                }
                left_rotate(root);
            }
        } else {
            if ((root->node_t::right->node_t::size * DELTA + 1)
                < root->node_t::left->node_t::size) {
                if (root->node_t::left->node_t::left->node_t::size * GAMMA
                    < root->node_t::left->node_t::size) {
                    left_rotate(root->node_t::left);
                }
                right_rotate(root);
            }
        }
    }

    np_t insert_unique_impl(np_t &root, np_t node) noexcept {
        if (is_sentinel(root)) [[unlikely]] {
            node->node_t::right = mock_sentinel();
            node->node_t::left = mock_sentinel();
            node->node_t::size = 1;

            root = node;
            return nullptr;
        }
        if (cmp(*node, *root)) {
            node = insert_unique_impl(root->node_t::left, node);
            if (node == nullptr) {
                root->node_t::size++;
                maintain(root, false);
            }
            return node;
        } else if (cmp(*root, *node)) {
            node = insert_unique_impl(root->node_t::right, node);
            if (node == nullptr) {
                root->node_t::size++;
                maintain(root, true);
            }
            return node;
        } else {
            return root;
        }
    }

    // TODO: Use "if constexpr" to combine insert_unique_impl and winsert_unique_impl to one.
    np_t winsert_unique_impl(np_t &root, np_t node) noexcept {
        if (is_sentinel(root)) [[unlikely]] {
            node->node_t::right = mock_sentinel();
            node->node_t::left = mock_sentinel();
            node->node_t::size = 1;

            root = node;
            return nullptr;
        }
        if (cmp(*node, *root)) {
            node = winsert_unique_impl(root->node_t::left, node);
            if (node == nullptr) {
                root->node_t::size++;
                wmaintain(root, false);
            }
            return node;
        } else if (cmp(*root, *node)) {
            node = winsert_unique_impl(root->node_t::right, node);
            if (node == nullptr) {
                root->node_t::size++;
                wmaintain(root, true);
            }
            return node;
        } else {
            return root;
        }
    }

    void insert_multi_impl(np_t &root, np_t node) noexcept {
        if (is_sentinel(root)) [[unlikely]] {
            node->node_t::right = mock_sentinel();
            node->node_t::left = mock_sentinel();
            node->node_t::size = 1;

            root = node;
            return;
        }
        root->node_t::size++;
        if (cmp(*node, *root)) {
            insert_multi_impl(root->node_t::left, node);
            maintain(root, false);
        } else {
            insert_multi_impl(root->node_t::right, node);
            maintain(root, true);
        }
    }

    void winsert_multi_impl(np_t &root, np_t node) noexcept {
        if (is_sentinel(root)) [[unlikely]] {
            node->node_t::right = mock_sentinel();
            node->node_t::left = mock_sentinel();
            node->node_t::size = 1;

            root = node;
            return;
        }
        root->node_t::size++;
        if (cmp(*node, *root)) {
            winsert_multi_impl(root->node_t::left, node);
            wmaintain(root, false);
        } else {
            winsert_multi_impl(root->node_t::right, node);
            wmaintain(root, true);
        }
    }

    template <typename K>
    np_t remove_unique_impl(np_t &root, const K &node) noexcept {
        if (is_sentinel(root)) [[unlikely]] {
            return nullptr;
        }
        np_t result;
        if (cmp(node, *root)) {
            result = remove_unique_impl(root->node_t::left, node);
            if (result != nullptr) {
                root->node_t::size--;
            }
            return result;
        } else if (cmp(*root, node)) {
            result = remove_unique_impl(root->node_t::right, node);
            if (result != nullptr) {
                root->node_t::size--;
            }
            return result;
        } else {
            result = root;
            if (is_sentinel(root->node_t::right)) {
                root = root->node_t::left;
            } else if (is_sentinel(root->node_t::left)) { // Unnecessary branch!
                root = root->node_t::right;
            } else {
                np_t r = root->node_t::right;
                if (is_sentinel(r->node_t::left)) {
                    r->node_t::left = root->node_t::left;
                    r->node_t::size = root->node_t::size - 1;
                    root = r;
                } else {
                    np_t sp = r;
                    np_t s = sp->node_t::left;

                    sp->node_t::size--;
                    while (!is_sentinel(s->node_t::left)) {
                        sp = s;
                        sp->node_t::size--;
                        s = s->node_t::left;
                    }
                    sp->node_t::left = s->node_t::right;

                    s->node_t::right = r;
                    s->node_t::left = root->node_t::left;
                    s->node_t::size = root->node_t::size - 1;

                    root = s;
                }
            }
            return result;
        }
    }

    template <typename K>
    [[nodiscard]]
    np_t find_impl(const T *root, const K &node) const noexcept {
        while (!is_sentinel(root)) {
            if (cmp(node, *root)) {
                root = root->node_t::left;
            } else if (cmp(*root, node)) {
                root = root->node_t::right;
            } else {
                return const_cast<np_t>(root);
            }
        }
        return nullptr;
    }

    [[nodiscard]]
    std::size_t height_impl(const T *root) const noexcept {
        if (is_sentinel(root)) {
            return 0;
        }
        std::size_t left_height = height_impl(root->node_t::left);
        std::size_t right_height = height_impl(root->node_t::right);
        return (left_height > right_height ? left_height : right_height) + 1;
    }

    template <typename K>
    std::size_t position_impl(const T *root, const K &node) const noexcept {
        std::size_t pos = 0;
        while (!is_sentinel(root)) {
            if (cmp(node, *root)) {
                root = root->node_t::left;
            } else {
                pos += (root->node_t::left->node_t::size + 1);
                if (!cmp(*root, node)) {
                    return pos - 1;
                }
                root = root->node_t::right;
            }
        }
        return std::size_t(-1);
    }

    template <typename K>
    std::size_t count_multi_impl(const T *root, const K &node) const noexcept {
        if (is_sentinel(root)) {
            return 0;
        }
        if (cmp(node, *root)) {
            return count_multi_impl(root->node_t::left, node);
        } else if (cmp(*root, node)) {
            return count_multi_impl(root->node_t::right, node);
        } else {
            return 1 + count_multi_impl(root->node_t::right, node)
                 + count_multi_impl(root->node_t::left, node);
        }
    }

    static T *mock_sentinel() noexcept {
        return static_cast<T *>(const_cast<node_t *>(&sentinel));
    }

    static inline const node_t sentinel{
        {{static_cast<T *>(const_cast<node_t *>(&irsbt::sentinel))},
         static_cast<T *>(const_cast<node_t *>(&irsbt::sentinel))},
        0
    };

    // TODO: need a macro for the msvc.
    [[no_unique_address]]
    CMP cmp;
    T *head;
};

} // namespace uit