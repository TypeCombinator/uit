#pragma once
#include <cstdint>
#include "uit/intrusive.hpp"

struct dapple : public uit::idnode<dapple> {
    dapple(uint64_t weight, int sn)
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    int sn;
};

struct sapple : public uit::isnode<sapple> {
    sapple(uint64_t weight, int sn)
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    int sn;
};