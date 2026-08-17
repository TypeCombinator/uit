// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#include <catch2/catch_test_macros.hpp>
#include <common/apple.hpp>
#include <uit/islist.hpp>

using list_t = uit::islist<&sapple::right>;
using node_t = sapple;

static_assert(std::is_trivial_v<list_t>);

TEST_CASE("basic", "[ilist]") {
    constexpr std::size_t vec_size = 5;
    std::vector<node_t> vec;
    vec.reserve(vec_size);
    for (std::size_t i{}; i < vec_size; i++) {
        vec.emplace_back(500 + i, i);
    }

    node_t node_guard{0, 0};
    node_guard.right = &node_guard;
    auto vec_init = [&vec, &node_guard]() {
        for (auto &e: vec) {
            e.right = &node_guard;
        }
    };
    SECTION("empty") {
        list_t list{};
        REQUIRE(list.empty());
    }
    SECTION("push front") {
        vec_init();
        list_t list{};
        for (std::size_t i{}; i < vec_size; i++) {
            list.push_front(&vec[(vec_size - 1) - i]);
        }
        REQUIRE(&list.front() == &vec[0]);
        for (std::size_t i{}; i < vec_size; i++) {
            if (i < (vec_size - 1)) {
                REQUIRE(vec[i].right == &vec[i + 1]);
            } else {
                REQUIRE(vec[i].right == nullptr);
            }
        }
    }
    SECTION("remove") {
        vec_init();
        list_t list{};
        static_assert(vec_size >= 5);
        for (std::size_t i{}; i < 5; i++) {
            list.push_front(&vec[(5 - 1) - i]);
        }
        list.remove(&vec[1]);
        REQUIRE(vec[0].right == &vec[2]);
        REQUIRE(&list.front() == &vec[0]);
        list.remove(&vec[3]);
        REQUIRE(vec[2].right == &vec[4]);
        REQUIRE(&list.front() == &vec[0]);
        list.remove(&vec[4]);
        REQUIRE(vec[2].right == nullptr);
        REQUIRE(&list.front() == &vec[0]);
        list.remove(&vec[0]);
        REQUIRE(vec[2].right == nullptr);
        REQUIRE(&list.front() == &vec[2]);
        list.remove(&vec[2]);
        REQUIRE(list.empty());
    }
    SECTION("iterator") {
        vec_init();
        list_t list{};
        for (std::size_t i{}; i < vec_size; i++) {
            list.push_front(&vec[(vec_size - 1) - i]);
        }
        const auto &clist = list;
        std::size_t i;
        i = {}; // Range-based for loop
        for (auto &e: list) {
            REQUIRE(&e == &vec[i]);
            i++;
        }
        i = {};
        for (auto &e: clist) {
            REQUIRE(&e == &vec[i]);
            i++;
        }
        i = {}; // Const iterator
        auto it_cend = list.cend();
        for (auto it = list.cbegin(); it != it_cend; it++) {
            REQUIRE(&(*it) == &vec[i]);
            i++;
        }
        i = {};
        it_cend = clist.cend();
        for (auto it = clist.cbegin(); it != it_cend; it++) {
            REQUIRE(&(*it) == &vec[i]);
            i++;
        }
    }
}