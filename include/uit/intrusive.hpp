#pragma once

#include <cstddef>

namespace uit {

template <std::size_t SL>
struct stag {
    consteval stag() = default;

    consteval stag(char const (&str)[SL]) {
        for (size_t i = 0; i < SL; ++i) {
            data[i] = str[i];
        }
    }

    char data[SL];
};

template <typename T, stag N = "">
struct isnode {
    T *right;
};

template <typename T, stag N = "">
struct idnode : public isnode<T, N> {
    T *left;
};

} // namespace uit