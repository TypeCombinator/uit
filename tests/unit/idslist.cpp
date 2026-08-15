// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#include <vector>
#include <catch2/catch_test_macros.hpp>
#include <common/apple.hpp>
#include <uit/idslist.hpp>

using list_t = uit::idslist<&sapple::right>;
using node_t = sapple;

static_assert(std::is_trivially_destructible_v<list_t>);
static_assert(std::is_trivially_copy_constructible_v<list_t>);
static_assert(std::is_copy_assignable_v<list_t>);
static_assert(std::is_trivially_copy_assignable_v<list_t>);
static_assert(std::is_trivially_move_constructible_v<list_t>);
static_assert(std::is_move_assignable_v<list_t>);
static_assert(std::is_trivially_move_assignable_v<list_t>);

TEST_CASE("basic", "[idslist]") {
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
    SECTION("push back") {
        vec_init();
        list_t list{};
        for (auto &e: vec) {
            list.push_back(&e);
        }
        REQUIRE(&list.front() == &vec[0]);
        REQUIRE(&list.back() == &vec[vec_size - 1]);
        for (std::size_t i{}; i < (vec_size - 1); i++) {
            REQUIRE(vec[i].right == &vec[i + 1]);
        }
        REQUIRE(vec.back().right == nullptr);
    }
    SECTION("push front") {
        vec_init();
        list_t list{};
        for (auto &e: vec) {
            list.push_front(&e);
        }
        REQUIRE(&list.front() == &vec[vec_size - 1]);
        REQUIRE(&list.back() == &vec[0]);
        for (std::size_t i{vec_size - 1}; i > 0; i--) {
            REQUIRE(vec[i].right == &vec[i - 1]);
        }
        REQUIRE(vec.front().right == nullptr);
    }
    SECTION("pop front") {
        vec_init();
        list_t list{};
        for (auto &e: vec) {
            list.push_back(&e);
        }
        for (std::size_t i{}; i < (vec_size - 1); i++) {
            REQUIRE(&vec[i] == &list.front());
            list.pop_front();
        }
        REQUIRE(&vec[vec_size - 1] == &list.front());
        REQUIRE(&vec[vec_size - 1] == &list.back());
        list.pop_front();
        REQUIRE(list.empty());
    }
    SECTION("remove") {
        vec_init();
        list_t list{};
        static_assert(vec_size >= 5);
        for (std::size_t i{}; i < 5; i++) {
            list.push_back(&vec[i]);
        }
        list.remove(&vec[1]);
        REQUIRE(vec[0].right == &vec[2]);
        REQUIRE(&list.front() == &vec[0]);
        REQUIRE(&list.back() == &vec[4]);
        list.remove(&vec[3]);
        REQUIRE(vec[2].right == &vec[4]);
        REQUIRE(&list.front() == &vec[0]);
        REQUIRE(&list.back() == &vec[4]);
        list.remove(&vec[4]);
        REQUIRE(vec[2].right == nullptr);
        REQUIRE(&list.front() == &vec[0]);
        REQUIRE(&list.back() == &vec[2]);
        list.remove(&vec[0]);
        REQUIRE(vec[2].right == nullptr);
        REQUIRE(&list.front() == &vec[2]);
        REQUIRE(&list.back() == &vec[2]);
        list.remove(&vec[2]);
        REQUIRE(list.empty());
    }
    SECTION("iterator") {
        vec_init();
        list_t list{};
        for (auto &e: vec) {
            list.push_back(&e);
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
    SECTION("ctor") {
        list_t list_org{};
        for (auto &e: vec) {
            list_org.push_back(&e);
        }
        list_t list_copy{list_org};
        std::size_t i;
        i = {};
        for (auto &e: list_copy) {
            REQUIRE(&e == &vec[i]);
            i++;
        }
        list_t list_move{std::move(list_org)};
        i = {};
        for (auto &e: list_move) {
            REQUIRE(&e == &vec[i]);
            i++;
        }
    }
    SECTION("assign") {
        list_t list_org{};
        for (auto &e: vec) {
            list_org.push_back(&e);
        }
        list_t list_copy{};
        list_copy = list_org;
        std::size_t i;
        i = {};
        for (auto &e: list_copy) {
            REQUIRE(&e == &vec[i]);
            i++;
        }
        list_t list_move{};
        list_move = std::move(list_org);
        i = {};
        for (auto &e: list_move) {
            REQUIRE(&e == &vec[i]);
            i++;
        }
    }
}