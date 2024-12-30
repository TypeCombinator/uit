#pragma once
#include <cstdint>
#include "uit/intrusive.hpp"

struct dapple : public uit::idnode<dapple> {
    dapple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    int sn;
};

struct sapple : public uit::isnode<sapple> {
    sapple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    int sn;
};

struct sbt_apple : public uit::isbt_node<sbt_apple> {
    explicit sbt_apple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    bool operator<(const sbt_apple &other) const noexcept {
        return weight < other.weight;
    }

    bool operator<(uint64_t other_weight) const noexcept {
        return weight < other_weight;
    }

    friend bool operator<(uint64_t other_weight, const sbt_apple &self) noexcept {
        return other_weight < self.weight;
    }

    uint64_t weight;
    int sn;
};