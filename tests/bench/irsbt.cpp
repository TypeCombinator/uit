// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include <common/apple.hpp>
#include <uit/irsbt.hpp>

// TODO: need a generic generator.
static std::vector<rsbt_apple>
    generate_random_vector(uint32_t seed, uint32_t size, uint32_t dis_a, uint32_t dis_b) {
    std::vector<rsbt_apple> v;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<uint32_t> dis(dis_a, dis_b);
    v.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        v.emplace_back(dis(gen), i);
    }
    return v;
}

static void isbt_insert_multi_random(benchmark::State& state) {
    std::size_t size = state.range(0);
    auto&& data = generate_random_vector(23, size, 0, size * 8);
    uit::irsbt<&rsbt_apple::node> tree{};

    for (auto _: state) {
        for (auto& e: data) {
            tree.insert_multi(&e);
        }
        tree.clear();
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(isbt_insert_multi_random)->RangeMultiplier(2)->Range(1 << 10, 1 << 18)->Complexity();