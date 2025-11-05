#pragma once
#include <cstdint>
#include "uit/intrusive.hpp"

struct rsbt_apple : public uit::irsbt_node<rsbt_apple> {
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
    int sn;
};