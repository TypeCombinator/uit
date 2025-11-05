// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include "common/linux_irbt.h"

#define container_of(_ptr_, _type_, _member_)                                                      \
    ((_type_ *) ((unsigned char *) (_ptr_) - offsetof(_type_, _member_)))

struct linux_rbt_apple {
    explicit linux_rbt_apple(uint64_t weight, int sn) noexcept
        : weight{weight}
        , sn{sn} {
    }

    uint64_t weight;
    struct rb_node node;
    int sn;
};

static void linux_irbt_insert(rb_root *root, linux_rbt_apple *node) {
    struct rb_node **cur_ptr = &(root->rb_node);
    struct rb_node *parent = NULL;
    while (*cur_ptr) {
        parent = *cur_ptr;
        if (node->weight < container_of(parent, linux_rbt_apple, node)->weight) {
            cur_ptr = &parent->rb_left;
        } else {
            cur_ptr = &parent->rb_right;
        }
    }
    rb_link_node(&node->node, parent, cur_ptr);
    __rb_insert(&node->node, root);
}

static void linux_irbt_erase(rb_root *root, const linux_rbt_apple *node) {
    rb_node *cur = root->rb_node;
    linux_rbt_apple *cur_container;

    while (cur != NULL) {
        cur_container = container_of(cur, linux_rbt_apple, node);
        if (node->weight < cur_container->weight) {
            cur = cur->rb_left;
        } else if (cur_container->weight < node->weight) {
            cur = cur->rb_right;
        } else {
            rb_erase(cur, root);
            return;
        }
    }
}

// TODO: need a generic generator.
static std::vector<linux_rbt_apple>
    generate_random_vector(uint32_t seed, uint32_t size, uint32_t dis_a, uint32_t dis_b) {
    std::vector<linux_rbt_apple> v;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<uint32_t> dis(dis_a, dis_b);
    v.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        v.emplace_back(dis(gen), i);
    }
    return v;
}

static void linux_irbt_insert_multi_random(benchmark::State &state) {
    std::size_t size = state.range(0);
    auto &&data = generate_random_vector(23, size, 0, size * 8);
    rb_root tree = {NULL};

    for (auto _: state) {
        for (auto &i: data) {
            linux_irbt_insert(&tree, &i);
        }
        // Clear the tree.
        tree.rb_node = NULL;
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(linux_irbt_insert_multi_random)->RangeMultiplier(2)->Range(1 << 10, 1 << 18)->Complexity();

static void linux_irbt_erase_random(benchmark::State &state) {
    std::size_t size = state.range(0);
    auto &&data = generate_random_vector(23, size, 0, size * 8);
    rb_root tree = {NULL};

    for (auto _: state) {
        state.PauseTiming();
        for (auto &e: data) {
            linux_irbt_insert(&tree, &e);
        }
        state.ResumeTiming();

        for (const auto &e: data) {
            linux_irbt_erase(&tree, &e);
        }
        if (tree.rb_node != NULL) {
            state.SkipWithError("The tree is not empty.");
            break;
        }
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(linux_irbt_erase_random)->RangeMultiplier(2)->Range(1 << 10, 1 << 18)->Complexity();