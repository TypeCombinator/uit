#pragma once
#include <cstdint>
#include "uit/intrusive.hpp"

struct dapple {
    dapple(uint64_t weight, int sn)
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    uit::idnode<dapple> node;
    int sn;
};

struct sapple {
    sapple(uint64_t weight, int sn)
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    uit::isnode<sapple> node;
    int sn;
};