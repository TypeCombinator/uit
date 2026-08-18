// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#include <vector>
#include <catch2/catch_test_macros.hpp>
#include "common/apple.hpp"
#include <uit/isdlist.hpp>

using list_t = uit::isdlist<&dapple::right, &dapple::left>;
using node_t = dapple;

static_assert(std::is_trivial_v<list_t>);

TEST_CASE("basic", "[isdlist]") {
    constexpr std::size_t vec_size = 5;
    std::vector<node_t> vec;
    vec.reserve(vec_size);
    for (std::size_t i{}; i < vec_size; i++) {
        vec.emplace_back(500 + i, i);
    }

    node_t node_guard{0, 0};
    node_guard.right = &node_guard;
    node_guard.left = &node_guard;
    for (auto &e: vec) {
        e.right = &node_guard;
        e.left = &node_guard;
    }
    list_t list{};
    REQUIRE(list.empty());

    SECTION("push front") {
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
            if (i > 0) {
                REQUIRE(vec[i].left == &vec[i - 1]);
            } else {
                REQUIRE(vec[i].left == list.sentinel());
            }
        }
    }
    SECTION("pop front") {
        for (auto &e: vec) {
            list.push_front(&e);
        }
        for (std::size_t i{}; i < vec_size; i++) {
            REQUIRE(&vec[(vec_size - 1) - i] == &list.front());
            list.pop_front();
        }
        REQUIRE(list.empty());
    }
    SECTION("remove") {
        static_assert(vec_size >= 5);
        for (std::size_t i{}; i < 5; i++) {
            list.push_front(&vec[(5 - 1) - i]);
        }
        list.remove(&vec[1]);
        REQUIRE(vec[0].right == &vec[2]);
        REQUIRE(vec[2].left == &vec[0]);
        REQUIRE(&list.front() == &vec[0]);
        list.remove(&vec[3]);
        REQUIRE(vec[2].right == &vec[4]);
        REQUIRE(vec[4].left == &vec[2]);
        REQUIRE(&list.front() == &vec[0]);
        list.remove(&vec[4]);
        REQUIRE(vec[2].right == nullptr);
        REQUIRE(&list.front() == &vec[0]);
        list.remove(&vec[0]);
        REQUIRE(vec[2].right == nullptr);
        REQUIRE(vec[2].left == list.sentinel());
        REQUIRE(&list.front() == &vec[2]);
        list.remove(&vec[2]);
        REQUIRE(list.empty());
    }
    SECTION("iterator") {
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