// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#pragma once

#include <cstddef>

namespace uit {

template <std::size_t N>
struct stag {
    consteval stag() = default;

    consteval stag(const char (&str)[N]) {
        for (std::size_t i = 0; i < N; ++i) {
            data[i] = str[i];
        }
    }

    char data[N];
};

template <typename T, stag S = "">
struct isnode {
    T *right;
};

template <typename T, stag S = "">
struct idnode : public isnode<T, S> {
    T *left;
};

template <typename T, stag S = "">
struct irsbt_node : public idnode<T, S> {
    std::size_t size;
};

template <typename T>
concept has_is_transparent = requires { typename T::is_transparent; };

} // namespace uit