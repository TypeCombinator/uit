// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_INTRUSIVE_F553190D_A9E1_4D56_9310_9CE4208E5ADD
#define UIT_INTRUSIVE_F553190D_A9E1_4D56_9310_9CE4208E5ADD
#include <cstddef>

namespace uit {

namespace detail {
template <typename T, template <typename...> typename U>
constexpr bool is_template_instance_of_v = false;
// TODO:
template <typename... Ts, template <typename...> typename U>
constexpr bool is_template_instance_of_v<U<Ts...>, U> = true;
} // namespace detail

template <typename U, template <typename...> typename T>
constexpr bool is_template_instance_of_v = detail::is_template_instance_of_v<U, T>;

namespace detail {
template <auto M>
struct member_access;

template <typename T, typename MT, MT T::*M>
struct member_access<M> {
    using container_type = T;
    using member_type = MT;
};
} // namespace detail

template <auto M>
using container_t = detail::member_access<M>::container_type;

template <auto M>
using member_t = detail::member_access<M>::member_type;

template <typename M, typename T>
constexpr size_t offset_of(M T::*field) noexcept {
    return reinterpret_cast<size_t>(&(static_cast<T *>(nullptr)->*field));
}

template <typename M, typename T>
T *container_of(M T::*field, M *m) noexcept {
    return reinterpret_cast<T *>(reinterpret_cast<unsigned char *>(m) - offset_of(field));
}

// Reduce overloading.
template <typename M, typename T>
const T *const_container_of(M T::*field, const M *m) noexcept {
    return reinterpret_cast<const T *>(
        reinterpret_cast<const unsigned char *>(m) - offset_of(field));
}

// TODO: Make it deprecated and use split member pointers instead.
template <typename T>
struct isnode {
    T *right;
};

template <typename T>
struct idnode : public isnode<T> {
    T *left;
};

template <typename T>
concept has_is_transparent = requires { typename T::is_transparent; };

} // namespace uit
#endif // intrusive.hpp