#pragma once
#include "intrusive.hpp"
#include <functional>

namespace uit {

template <typename T, stag N = "", typename CMP = std::less<>>
class isbt {
   public:
    using node_t = isbt_node<T, N>;
    using np_t = T *;

    isbt() noexcept {
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

    [[nodiscard]]
    std::size_t size() const noexcept {
        return head->node_t::size;
    }

    np_t insert_unique(np_t node) noexcept {
        return insert_unique_impl(head, node);
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
    std::size_t height_impl(const T *node) const noexcept {
        if (is_sentinel(node)) {
            return 0;
        }
        std::size_t hl = height_impl(node->node_t::left);
        std::size_t hr = height_impl(node->node_t::right);
        return (hl > hr ? hl : hr) + 1;
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

    static T *mock_sentinel() noexcept {
        return static_cast<T *>(const_cast<node_t *>(&sentinel));
    }

    static inline const node_t sentinel{
        {{static_cast<T *>(const_cast<node_t *>(&isbt::sentinel))},
         static_cast<T *>(const_cast<node_t *>(&isbt::sentinel))},
        0
    };

    // TODO: need a macro for the msvc.
    [[no_unique_address]] CMP cmp;
    T *head;
};

} // namespace uit