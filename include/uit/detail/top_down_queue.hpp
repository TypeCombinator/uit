// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_DETAIL_TOP_DOWN_QUEUE_D61ED956_F6EC_434D_B18A_993B6646105E
#define UIT_DETAIL_TOP_DOWN_QUEUE_D61ED956_F6EC_434D_B18A_993B6646105E

namespace uit { namespace detail {
template <typename T>
struct top_down_queue {
    static constexpr unsigned max_capacity = 4;
    static constexpr unsigned index_mask = max_capacity - 1;
    using np_t = T *;

    np_t *m_pointer[max_capacity];
    unsigned m_path;
    unsigned m_read_index;
    signed m_size; // Notice: The type of m_size must be signed!

    explicit top_down_queue() noexcept
        : m_path{}
        , m_read_index{}
        , m_size{} {
    }

    void push_right_path(np_t *pointer) noexcept {
        m_pointer[(m_read_index + m_size) & index_mask] = pointer;
        m_path |= (1u << m_size);
        m_size++;
    }

    void push_left_path(np_t *pointer) noexcept {
        m_pointer[(m_read_index + m_size) & index_mask] = pointer;
        m_size++;
    }

    void push_path(np_t *pointer, bool is_right) noexcept {
        m_pointer[(m_read_index + m_size) & index_mask] = pointer;
        m_path |= (static_cast<unsigned>(is_right) << m_size);
        m_size++;
    }

    template <unsigned step = 1>
    void pop() noexcept {
        m_path >>= step;
        m_read_index += step;
        m_size -= step;
    }

    void pop(unsigned step) noexcept {
        m_path >>= step;
        m_read_index += step;
        m_size -= step;
    }

    void set_front(np_t *pointer) noexcept {
        m_pointer[m_read_index & index_mask] = pointer;
    }

    np_t *front_pointer() const noexcept {
        return m_pointer[m_read_index & index_mask];
    }

    auto path_queue() const noexcept {
        return m_path;
    }

    auto size() const noexcept {
        return m_size;
    }
};
}} // namespace uit::detail
#endif // top_down_queue.hpp