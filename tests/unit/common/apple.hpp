// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#pragma once
#include <cstdint>
#include "uit/intrusive.hpp"

struct dapple {

    dapple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    uit::idnode<dapple> node;
    int sn;
};

struct sapple {
    sapple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    uit::isnode<sapple> node;
    int sn;
};

struct sapple1 {
    sapple1(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    sapple1 *right;
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