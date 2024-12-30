#pragma once
#include "intrusive.hpp"
#include <functional>

// Reference: Chen Qifeng. Size Balanced Tree. 2006.
namespace uit {

// It's simpler and better to use std::less<> as the default comparator.
template <auto M, typename CMP = std::less<>>
class isbt;

template <typename T, isbt_node<T> T::*M, typename CMP>
class isbt<M, CMP> {
   public:
    using np_t = T *;

    isbt() noexcept {
        head = mock_sentinel();
    }

    [[nodiscard]]
    static bool is_sentinel(const T *node) noexcept {
        return node == uit::const_container_of(M, &sentinel);
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return is_sentinel(head);
    }

    [[nodiscard]]
    std::size_t size() const noexcept {
        return (head->*M).size;
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
            std::size_t lsize = ((root->*M).left->*M).size;
            if (lsize == pos) {
                return root;
            } else if (lsize > pos) {
                root = (root->*M).left;
            } else {
                pos -= (lsize + 1);
                root = (root->*M).right;
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
        np_t s = (n->*M).right;

        (n->*M).right = (s->*M).left;
        (s->*M).left = n;
        (s->*M).size = (n->*M).size;
        (n->*M).size = ((n->*M).right->*M).size + ((n->*M).left->*M).size + 1;
        n = s;
    }

    static void right_rotate(np_t &n) noexcept {
        np_t s = (n->*M).left;

        (n->*M).left = (s->*M).right;
        (s->*M).right = n;
        (s->*M).size = (n->*M).size;
        (n->*M).size = ((n->*M).right->*M).size + ((n->*M).left->*M).size + 1;
        n = s;
    }

    static void maintain(np_t &root, bool right_leaning) noexcept {
        if (right_leaning) {
            if ((((root->*M).right->*M).left->*M).size > ((root->*M).left->*M).size) {
                right_rotate((root->*M).right);
                left_rotate(root);
            } else if ((((root->*M).right->*M).right->*M).size > ((root->*M).left->*M).size) {
                left_rotate(root);
            } else {
                return;
            }
        } else {
            if ((((root->*M).left->*M).right->*M).size > ((root->*M).right->*M).size) {
                left_rotate((root->*M).left);
                right_rotate(root);
            } else if ((((root->*M).left->*M).left->*M).size > ((root->*M).right->*M).size) {
                right_rotate(root);
            } else {
                return;
            }
        }
        maintain((root->*M).left, false);
        maintain((root->*M).right, true);
        maintain(root, true);
        maintain(root, false);
    }

    np_t insert_unique_impl(np_t &root, np_t node) noexcept {
        if (is_sentinel(root)) [[unlikely]] {
            (node->*M).right = mock_sentinel();
            (node->*M).left = mock_sentinel();
            (node->*M).size = 1;

            root = node;
            return nullptr;
        }
        if (cmp(*node, *root)) {
            node = insert_unique_impl((root->*M).left, node);
            if (node == nullptr) {
                (root->*M).size++;
                maintain(root, false);
            }
            return node;
        } else if (cmp(*root, *node)) {
            node = insert_unique_impl((root->*M).right, node);
            if (node == nullptr) {
                (root->*M).size++;
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
            result = remove_unique_impl((root->*M).left, node);
            if (result != nullptr) {
                (root->*M).size--;
            }
            return result;
        } else if (cmp(*root, node)) {
            result = remove_unique_impl((root->*M).right, node);
            if (result != nullptr) {
                (root->*M).size--;
            }
            return result;
        } else {
            result = root;
            if (is_sentinel((root->*M).right)) {
                root = (root->*M).left;
            } else if (is_sentinel((root->*M).left)) { // Unnecessary branch!
                root = (root->*M).right;
            } else {
                np_t r = (root->*M).right;
                if (is_sentinel((r->*M).left)) {
                    (r->*M).left = (root->*M).left;
                    (r->*M).size = (root->*M).size - 1;
                    root = r;
                } else {
                    np_t sp = r;
                    np_t s = (sp->*M).left;

                    (sp->*M).size--;
                    while (!is_sentinel((s->*M).left)) {
                        sp = s;
                        (sp->*M).size--;
                        s = (s->*M).left;
                    }
                    (sp->*M).left = (s->*M).right;

                    (s->*M).right = r;
                    (s->*M).left = (root->*M).left;
                    (s->*M).size = (root->*M).size - 1;

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
                root = (root->*M).left;
            } else if (cmp(*root, node)) {
                root = (root->*M).right;
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
        std::size_t hl = height_impl((node->*M).left);
        std::size_t hr = height_impl((node->*M).right);
        return (hl > hr ? hl : hr) + 1;
    }

    template <typename K>
    std::size_t position_impl(const T *root, const K &node) const noexcept {
        std::size_t pos = 0;
        while (!is_sentinel(root)) {
            if (cmp(node, *root)) {
                root = (root->*M).left;
            } else {
                pos += (((root->*M).left->*M).size + 1);
                if (!cmp(*root, node)) {
                    return pos - 1;
                }
                root = (root->*M).right;
            }
        }
        return std::size_t(-1);
    }

    // Unlike the mock_head, although this is also UB, it will not lead to unexpected behavoir.
    static T *mock_sentinel() noexcept {
        return uit::container_of(M, const_cast<uit::isbt_node<T> *>(&sentinel));
    }

    static inline const isbt_node<T> sentinel{
        {{uit::container_of(M, const_cast<uit::isbt_node<T> *>(&isbt::sentinel))},
         uit::container_of(M, const_cast<uit::isbt_node<T> *>(&isbt::sentinel))},
        0
    };

    // TODO: need a macro for the msvc.
    [[no_unique_address]] CMP cmp;
    T *head;
};

} // namespace uit