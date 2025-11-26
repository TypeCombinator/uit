// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UNIT_COMMON_APPLE_4D7E9564_ED2C_460C_A0EC_2F4E25AB186A
#define UNIT_COMMON_APPLE_4D7E9564_ED2C_460C_A0EC_2F4E25AB186A
#include <cstdint>
#include <uit/intrusive.hpp>

struct dapple {
    dapple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    dapple *right;
    dapple *left;
    int sn;
};

struct sapple {
    sapple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    sapple *right;
    int sn;
};

struct rsbt_apple {
    explicit rsbt_apple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    bool operator<(const rsbt_apple &other) const noexcept {
        return weight < other.weight;
    }

    bool operator<(uint64_t other_weight) const noexcept {
        return weight < other_weight;
    }

    friend bool operator<(uint64_t other_weight, const rsbt_apple &self) noexcept {
        return other_weight < self.weight;
    }

    uint64_t weight;
    rsbt_apple *right;
    rsbt_apple *left;
    size_t size;
    int sn;
};
#endif // apple.hpp