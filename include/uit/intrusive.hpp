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

} // namespace uit