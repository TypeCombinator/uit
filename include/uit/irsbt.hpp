// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_IRSBT_863421E6_3490_4C93_AD0F_0645A51AA38F
#define UIT_IRSBT_863421E6_3490_4C93_AD0F_0645A51AA38F
#include <uit/intrusive.hpp>
#include <functional>

// References:
// [0] Chen Qifeng. Size Balanced Tree. 2006.
// [1] Yoichi Hirai and Kazuhiko Yamamoto. Balancing weight-balanced trees. 2011.
// Notices:
// [0] The acronym irsbt stands for intrusive recursive size-balanced tree.
// [1] The mock sentinel will involve UB, but the code works correctly.
// [2] Nodes in the tree don‘t have parent pointers, so some operations can only be implemented
// recursively.
// [3] This is an implementation I plan to deprecate. Please refer to the alternative implementation
// in irwbt.hpp.
namespace uit {
// struct [[deprecated]] irsbt;
template <auto Right, auto Left, auto Size, typename CMP = std::less<>>
struct irsbt;

template <typename T, typename MT, MT T::*Right, MT T::*Left, auto Size, typename CMP>
struct irsbt<Right, Left, Size, CMP> {
   public:
    using np_t = T *;

    irsbt() noexcept {
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
            std::size_t lsize = root->*Left->*Size;
            if (lsize == pos) {
                return root;
            } else if (lsize > pos) {
                root = root->*Left;
            } else {
                pos -= (lsize + 1);
                root = root->*Right;
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

    static void maintain(np_t &root, bool right_leaning) noexcept {
        if (right_leaning) {
            if (root->*Right->*Left->*Size > root->*Left->*Size) {
                right_rotate(root->*Right);
                left_rotate(root);
            } else if (root->*Right->*Right->*Size > root->*Left->*Size) {
                left_rotate(root);
            } else {
                return;
            }
        } else {
            if (root->*Left->*Right->*Size > root->*Right->*Size) {
                left_rotate(root->*Left);
                right_rotate(root);
            } else if (root->*Left->*Left->*Size > root->*Right->*Size) {
                right_rotate(root);
            } else {
                return;
            }
        }
        maintain(root->*Left, false);
        maintain(root->*Right, true);
        maintain(root, true);
        maintain(root, false);
    }

    template <auto DELTA = 3, auto GAMMA = 3>
    static void wmaintain(np_t &root, bool right_leaning) noexcept {
        if (right_leaning) {
            if ((root->*Left->*Size * DELTA + 1) < (root->*Right->*Size)) {
                if (root->*Right->*Right->*Size * GAMMA < root->*Right->*Size) {
                    right_rotate(root->*Right);
                }
                left_rotate(root);
            }
        } else {
            if ((root->*Right->*Size * DELTA + 1) < root->*Left->*Size) {
                if (root->*Left->*Left->*Size * GAMMA < root->*Left->*Size) {
                    left_rotate(root->*Left);
                }
                right_rotate(root);
            }
        }
    }

    np_t insert_unique_impl(np_t &root, np_t node) noexcept {
        if (is_sentinel(root)) [[unlikely]] {
            node->*Right = mock_sentinel();
            node->*Left = mock_sentinel();
            node->*Size = 1;

            root = node;
            return nullptr;
        }
        if (cmp(*node, *root)) {
            node = insert_unique_impl(root->*Left, node);
            if (node == nullptr) {
                (root->*Size)++;
                maintain(root, false);
            }
            return node;
        } else if (cmp(*root, *node)) {
            node = insert_unique_impl(root->*Right, node);
            if (node == nullptr) {
                (root->*Size)++;
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
            node->*Right = mock_sentinel();
            node->*Left = mock_sentinel();
            node->*Size = 1;

            root = node;
            return nullptr;
        }
        if (cmp(*node, *root)) {
            node = winsert_unique_impl(root->*Left, node);
            if (node == nullptr) {
                (root->*Size)++;
                wmaintain(root, false);
            }
            return node;
        } else if (cmp(*root, *node)) {
            node = winsert_unique_impl(root->*Right, node);
            if (node == nullptr) {
                (root->*Size)++;
                wmaintain(root, true);
            }
            return node;
        } else {
            return root;
        }
    }

    void insert_multi_impl(np_t &root, np_t node) noexcept {
        if (is_sentinel(root)) [[unlikely]] {
            node->*Right = mock_sentinel();
            node->*Left = mock_sentinel();
            node->*Size = 1;

            root = node;
            return;
        }
        (root->*Size)++;
        if (cmp(*node, *root)) {
            insert_multi_impl(root->*Left, node);
            maintain(root, false);
        } else {
            insert_multi_impl(root->*Right, node);
            maintain(root, true);
        }
    }

    void winsert_multi_impl(np_t &root, np_t node) noexcept {
        if (is_sentinel(root)) [[unlikely]] {
            node->*Right = mock_sentinel();
            node->*Left = mock_sentinel();
            node->*Size = 1;

            root = node;
            return;
        }
        (root->*Size)++;
        if (cmp(*node, *root)) {
            winsert_multi_impl(root->*Left, node);
            wmaintain(root, false);
        } else {
            winsert_multi_impl(root->*Right, node);
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
            result = remove_unique_impl(root->*Left, node);
            if (result != nullptr) {
                (root->*Size)--;
            }
            return result;
        } else if (cmp(*root, node)) {
            result = remove_unique_impl(root->*Right, node);
            if (result != nullptr) {
                (root->*Size)--;
            }
            return result;
        } else {
            result = root;
            if (is_sentinel(root->*Right)) {
                root = root->*Left;
            } else if (is_sentinel(root->*Left)) { // Unnecessary branch!
                root = root->*Right;
            } else {
                np_t r = root->*Right;
                if (is_sentinel(r->*Left)) {
                    r->*Left = root->*Left;
                    r->*Size = root->*Size - 1;
                    root = r;
                } else {
                    np_t sp = r;
                    np_t s = sp->*Left;

                    (sp->*Size)--;
                    while (!is_sentinel(s->*Left)) {
                        sp = s;
                        (sp->*Size)--;
                        s = s->*Left;
                    }
                    sp->*Left = s->*Right;

                    s->*Right = r;
                    s->*Left = root->*Left;
                    s->*Size = root->*Size - 1;

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
                root = root->*Left;
            } else if (cmp(*root, node)) {
                root = root->*Right;
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
        std::size_t left_height = height_impl(root->*Left);
        std::size_t right_height = height_impl(root->*Right);
        return (left_height > right_height ? left_height : right_height) + 1;
    }

    template <typename K>
    std::size_t position_impl(const T *root, const K &node) const noexcept {
        std::size_t pos = 0;
        while (!is_sentinel(root)) {
            if (cmp(node, *root)) {
                root = root->*Left;
            } else {
                pos += (root->*Left->*Size + 1);
                if (!cmp(*root, node)) {
                    return pos - 1;
                }
                root = root->*Right;
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
            return count_multi_impl(root->*Left, node);
        } else if (cmp(*root, node)) {
            return count_multi_impl(root->*Right, node);
        } else {
            return 1 + count_multi_impl(root->*Right, node) + count_multi_impl(root->*Left, node);
        }
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
#endif