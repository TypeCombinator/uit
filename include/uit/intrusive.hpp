// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#pragma once

#include <cstddef>

namespace uit {

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

template <typename T>
struct isnode {
    T *right;
};

template <typename T>
struct idnode : public isnode<T> {
    T *left;
};

template <typename T>
struct irsbt_node : public idnode<T> {
    std::size_t size;
};

template <typename T>
concept has_is_transparent = requires { typename T::is_transparent; };

} // namespace uit