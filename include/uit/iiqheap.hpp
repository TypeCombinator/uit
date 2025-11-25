// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_IIQHEAP_6B2407E1_D0A5_4D0A_8D6F_6DDC5A03F2E7
#define UIT_IIQHEAP_6B2407E1_D0A5_4D0A_8D6F_6DDC5A03F2E7
#include <uit/intrusive.hpp>
#include <functional>
#include <cstring>

namespace uit {
// Extra attribute that can be added to the allocator.
template <typename T>
struct no_expanding {
    using type = T;
};

template <
    auto Index,
    typename CMP = std::less<>,
    typename Allocator = std::allocator<container_t<Index>*>>
class iiqheap;

template <typename T, typename MT, MT T::* Index, typename CMP, typename Allocator>
class iiqheap<Index, CMP, Allocator> {
    using np_t = T*;
    using index_t = MT;
    static constexpr bool allocator_no_expanding =
        is_template_instance_of_v<Allocator, no_expanding>;

    template <typename U, bool Fixed>
    struct allocator_selector;

    template <typename U>
    struct allocator_selector<U, true> {
        using type = Allocator::type;
    };

    template <typename U>
    struct allocator_selector<U, false> {
        using type = Allocator;
    };

    using allocator_type = allocator_selector<int, allocator_no_expanding>::type;
    using allocator_traits = std::allocator_traits<allocator_type>;
    static constexpr bool allocator_pocma =
        allocator_traits::propagate_on_container_move_assignment::value;
    static constexpr bool allocator_always_equal = allocator_traits::is_always_equal::value;

    static constexpr index_t parent_index(index_t index) noexcept {
        return (index - 1) >> 2;
    }

    static constexpr index_t child0_index(index_t index) noexcept {
        return (index << 2) + index_t{1};
    }
   public:
    explicit iiqheap(index_t reserve)
        : m_storage{}
        , m_allocator{} {
        m_storage = allocator_traits::allocate(m_allocator, reserve);
        m_capacity = reserve;
        m_size = 0;
    }

    explicit iiqheap(index_t reserve, const allocator_type& alloc)
        : m_storage{}
        , m_allocator{alloc} {
        m_storage = allocator_traits::allocate(m_allocator, reserve);
        m_capacity = reserve;
        m_size = 0;
    }

    iiqheap(iiqheap&& other) noexcept(std::is_nothrow_move_constructible_v<allocator_type>)
        : m_allocator(std::move(other.m_allocator)) {
        m_storage = other.m_storage;
        m_size = other.m_size;
        m_capacity = other.m_capacity;

        other.m_storage = nullptr;
        // other.m_size = 0;
    }

    iiqheap& operator=(iiqheap&& other) noexcept(allocator_pocma || allocator_always_equal) {
        if (this == &other) [[unlikely]] {
            return *this;
        }
        if constexpr (allocator_always_equal) {
            std::swap(m_storage, other.m_storage);
            std::swap(m_capacity, other.m_capacity);
            std::swap(m_size, other.m_size);
        } else if constexpr (allocator_pocma) {
            allocator_traits::deallocate(m_allocator, m_storage, m_capacity);
            m_allocator = std::move(other.m_allocator); // This can throw.
            m_storage = other.m_storage;
            m_capacity = other.m_capacity;
            m_size = other.m_size;

            other.m_storage = nullptr;
            // other.m_size = 0;
        } else {
            if (m_allocator != other.m_allocator) {
                if (allocator_no_expanding || (m_capacity >= other.m_size)) {
                    // If the capacity is insufficient and expansion is prohibited, it is UB.
                    std::memcpy(m_storage, other.m_storage, sizeof(np_t) * other.m_size);
                    m_size = other.m_size;
                } else {
                    np_t* new_storage = allocator_traits::allocate(m_allocator, other.m_capacity);
                    std::memcpy(new_storage, other.m_storage, sizeof(np_t) * other.m_size);
                    allocator_traits::deallocate(m_allocator, m_storage, m_capacity);
                    m_storage = new_storage;
                    m_capacity = other.m_capacity;
                    m_size = other.m_size;
                }
            } else {
                std::swap(m_storage, other.m_storage);
                std::swap(m_capacity, other.m_capacity);
                std::swap(m_size, other.m_size);
            }
        }
        return *this;
    }

    // The pointer array is referenced by the index of the node，so it's move-only.
    iiqheap(const iiqheap&) = delete;

    iiqheap& operator=(const iiqheap&) = delete;

    ~iiqheap() {
        if (m_storage != nullptr) {
            allocator_traits::deallocate(m_allocator, m_storage, m_capacity);
            m_storage = nullptr;
        }
    }

    void push(np_t node) noexcept(allocator_no_expanding) {
        // TODO: Process the maximum of index_t
        if constexpr (!allocator_no_expanding) {
            if ((m_size + 1) > m_capacity) [[unlikely]] {
                expand_capacity(m_capacity * 2);
            }
        }
        sift_up(m_storage, node, m_size);
        m_size++;
    }

    [[nodiscard]]
    T& top() const noexcept {
        return *m_storage[0];
    }

    [[nodiscard]]
    bool empty() const noexcept {
        return m_size == 0;
    }

    void clear() noexcept {
        m_size = 0;
    }

    void pop() noexcept {
        // TODO: UIT_ASSERT(m_size > 0);
        m_size--;
        sift_down(m_storage, m_size, m_storage[m_size], 0);
    }

    void remove(np_t node) noexcept {
        // TODO: UIT_ASSERT(m_size > 0);
        m_size--;
        auto node_idx = node->*Index;
        if (node_idx < m_size) {
            if ((node_idx > 0) && CMP{}(*m_storage[m_size], *m_storage[parent_index(node_idx)])) {
                sift_up(m_storage, m_storage[m_size], node_idx);
            } else {
                sift_down(m_storage, m_size, m_storage[m_size], node_idx);
            }
        }
    }

    // For a min-heap, it's equivalent to the decrease-key operation.
    void sift_up(np_t node) noexcept {
        auto node_idx = node->*Index;
        if (node_idx > 0) {
            sift_up(m_storage, node, node_idx);
        }
    }

    // For a min-heap, it's equivalent to the increase-key operation.
    void sift_down(np_t node) noexcept {
        sift_down(m_storage, m_size, node, node->*Index);
    }

    void sift(np_t node) noexcept {
        auto node_idx = node->*Index;
        if ((node_idx > 0) && CMP{}(*node, *m_storage[parent_index(node_idx)])) {
            sift_up(m_storage, node, node_idx);
        } else {
            sift_down(m_storage, m_size, node, node_idx);
        }
    }

    [[nodiscard]]
    index_t size() const noexcept {
        return m_size;
    }

    [[nodiscard]]
    index_t capacity() const noexcept {
        return m_capacity;
    }
   private:
    void expand_capacity(index_t grater_capacity) {
        np_t* new_storage = allocator_traits::allocate(m_allocator, grater_capacity);

        std::memcpy(new_storage, m_storage, sizeof(np_t) * m_capacity);
        allocator_traits::deallocate(m_allocator, m_storage, m_capacity);
        m_storage = new_storage;
        m_capacity = grater_capacity;
    }

    static inline void sift_up(np_t* begin_ptr, np_t cur, index_t cur_idx) noexcept {
        index_t parent_idx;
        np_t parent;
        while (cur_idx > 0) {
            parent_idx = parent_index(cur_idx);
            parent = begin_ptr[parent_idx];
            if (!CMP{}(*cur, *parent)) {
                break;
            }
            begin_ptr[cur_idx] = parent;
            begin_ptr[cur_idx]->*Index = cur_idx;
            cur_idx = parent_idx;
        }
        begin_ptr[cur_idx] = cur;
        cur->*Index = cur_idx;
    }

    static inline void
        sift_down(np_t* begin_ptr, index_t size, np_t cur, index_t cur_idx) noexcept {
        auto child_idx = child0_index(cur_idx);
        np_t* min_ptr;
        np_t* child_ptr;
        while ((child_idx + 3u) < size) {
            min_ptr = begin_ptr + child_idx;
            child_ptr = min_ptr + 1;
            if (CMP{}(**child_ptr, **min_ptr)) {
                min_ptr = child_ptr;
            }

            child_ptr++;
            if (CMP{}(**child_ptr, **min_ptr)) {
                min_ptr = child_ptr;
            }

            child_ptr++;
            if (CMP{}(**child_ptr, **min_ptr)) {
                min_ptr = child_ptr;
            }
            if (CMP{}(**min_ptr, *cur)) {
                begin_ptr[cur_idx] = *min_ptr;
                (*min_ptr)->*Index = cur_idx;
                cur_idx = min_ptr - begin_ptr;

                child_idx = child0_index(cur_idx);
            } else {
                begin_ptr[cur_idx] = cur;
                cur->*Index = cur_idx;
                return;
            }
        }
        if (child_idx < size) {
            child_ptr = begin_ptr + child_idx;
            min_ptr = child_ptr;
            // TODO: UIT_ATTR_ASSUME(size - child_idx < 4);
            switch (size - child_idx) {
            case 3:
                child_ptr++;
                if (CMP{}(**child_ptr, **min_ptr)) {
                    min_ptr = child_ptr;
                }
                [[fallthrough]];
            case 2:
                child_ptr++;
                if (CMP{}(**child_ptr, **min_ptr)) {
                    min_ptr = child_ptr;
                }
            default:
                break;
            }
            if (CMP{}(**min_ptr, *cur)) {
                begin_ptr[cur_idx] = *min_ptr;
                (*min_ptr)->*Index = cur_idx;
                cur_idx = min_ptr - begin_ptr;
            }
        }
        begin_ptr[cur_idx] = cur;
        cur->*Index = cur_idx;
    }

    // std::vector might be a better choice.
    np_t* m_storage;
    index_t m_size;
    index_t m_capacity;
    // TODO: Need a macro for msvc.
    [[no_unique_address]]
    allocator_type m_allocator;
};

namespace pmr {
template <auto Index, typename CMP = std::less<>>
using iiqheap = ::uit::iiqheap<Index, CMP, std::pmr::polymorphic_allocator<container_t<Index>*>>;
} // namespace pmr

} // namespace uit
#endif // iiqheap.hpp