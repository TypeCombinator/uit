#include <benchmark/benchmark.h>
#include <vector>
#include <random>
#include "common/freebsd_irbt.h"

#define FREEBSD_RBT_APPLE_BITS(_ptr_) (_RB_BITSUP(_ptr_, node) & _RB_LR)

struct freebsd_rbt_apple {
    explicit freebsd_rbt_apple(uint64_t weight, int sn) noexcept
        : weight{weight}
        , sn{sn} {
    }

    uint64_t weight;
    RB_ENTRY(freebsd_rbt_apple) node;
    int sn;
};

RB_HEAD(freebsd_rbt, freebsd_rbt_apple);

static int freebsd_rbt_apple_cmp(const freebsd_rbt_apple *a, const freebsd_rbt_apple *b) {
    return a->weight - b->weight;
}
RB_GENERATE_INTERNAL(freebsd_rbt, freebsd_rbt_apple, node, freebsd_rbt_apple_cmp, static)

static void freebsd_rbt_insert_multi(struct freebsd_rbt *root, struct freebsd_rbt_apple *node) {
    struct freebsd_rbt_apple **cur_ptr = &(root->rbh_root);
    struct freebsd_rbt_apple *parent = NULL;
    while (*cur_ptr) {
        parent = *cur_ptr;
        if (freebsd_rbt_apple_cmp(node, parent) < 0) {
            cur_ptr = &RB_LEFT(parent, node);
        } else {
            cur_ptr = &RB_RIGHT(parent, node);
        }
    }
    freebsd_rbt_RB_INSERT_FINISH(root, parent, cur_ptr, node);
}

static void freebsd_rbt_erase(struct freebsd_rbt *root, struct freebsd_rbt_apple *node) {
    freebsd_rbt_apple *cur = root->rbh_root;

    while (cur != NULL) {
        int cmp_ret = freebsd_rbt_apple_cmp(node, cur);
        if (cmp_ret < 0) {
            cur = RB_LEFT(cur, node);
        } else if (cmp_ret > 0) {
            cur = RB_RIGHT(cur, node);
        } else {
            freebsd_rbt_RB_REMOVE(root, cur);
            return;
        }
    }
}

// TODO: need a generic generator.
static std::vector<freebsd_rbt_apple>
    generate_random_vector(uint32_t seed, uint32_t size, uint32_t dis_a, uint32_t dis_b) {
    std::vector<freebsd_rbt_apple> v;
    std::mt19937 gen(seed);
    std::uniform_int_distribution<uint32_t> dis(dis_a, dis_b);
    v.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        v.emplace_back(dis(gen), i);
    }
    return v;
}

static void freebsd_irbt_insert_multi_random(benchmark::State &state) {
    std::size_t size = state.range(0);
    auto &&data = generate_random_vector(23, size, 0, size * 8);
    struct freebsd_rbt tree = {NULL};

    for (auto _: state) {
        for (auto &i: data) {
            freebsd_rbt_insert_multi(&tree, &i);
        }
        // Clear the tree.
        tree.rbh_root = NULL;
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(freebsd_irbt_insert_multi_random)
    ->RangeMultiplier(2)
    ->Range(1 << 10, 1 << 18)
    ->Complexity();

static void freebsd_irbt_erase_random(benchmark::State &state) {
    std::size_t size = state.range(0);
    auto &&data = generate_random_vector(23, size, 0, size * 8);
    struct freebsd_rbt tree = {NULL};

    for (auto _: state) {
        state.PauseTiming();
        for (auto &e: data) {
            freebsd_rbt_insert_multi(&tree, &e);
        }
        state.ResumeTiming();

        for (auto &e: data) {
            freebsd_rbt_erase(&tree, &e);
        }
        if (tree.rbh_root != NULL) {
            state.SkipWithError("The tree is not empty.");
            break;
        }
    }
    state.SetComplexityN(state.range(0));
}

BENCHMARK(freebsd_irbt_erase_random)->RangeMultiplier(2)->Range(1 << 10, 1 << 18)->Complexity();