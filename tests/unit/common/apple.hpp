// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
// SPDX-License-Identifier: MPL-2.0
//
// This project is licensed under the Mozilla Public License 2.0.
// Please see the accompanying LICENSE file in this project root
// or https://spdx.org/licenses/MPL-2.0.html for details.

#ifndef UNIT_COMMON_APPLE_4D7E9564_ED2C_460C_A0EC_2F4E25AB186A
#define UNIT_COMMON_APPLE_4D7E9564_ED2C_460C_A0EC_2F4E25AB186A
#include <cstdint>
#include <uit/intrusive.hpp>

struct dapple {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    explicit dapple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    dapple *right;
    dapple *left;
    int sn;
};

struct sapple {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    explicit sapple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    sapple *right;
    int sn;
};

struct rsbt_apple {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
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