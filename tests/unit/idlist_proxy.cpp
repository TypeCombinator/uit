#include <vector>
#include <catch2/catch_test_macros.hpp>
#include <common/apple.hpp>
#include <uit/idlist_proxy.hpp>

using list_t = uit::idlist_proxy<&dapple::right, &dapple::left>;
using node_t = dapple;

static_assert(std::is_trivial_v<list_t>);

TEST_CASE("basic", "[idlist_proxy]") {
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

    node_t list_sentinel{0, 0};
    list_t list{list_sentinel};
    list.clear();

    REQUIRE(list.empty());

    SECTION("push front") {
        for (std::size_t i{}; i < vec_size; i++) {
            list.push_front(&vec[(vec_size - 1) - i]);
        }
        REQUIRE(&list.front() == &vec[0]);
        REQUIRE(&list.back() == &vec[vec_size - 1]);
        for (std::size_t i{}; i < vec_size; i++) {
            if (i < (vec_size - 1)) {
                REQUIRE(vec[i].right == &vec[i + 1]);
            } else {
                REQUIRE(vec[i].right == list.sentinel());
            }
            if (i > 0) {
                REQUIRE(vec[i].left == &vec[i - 1]);
            } else {
                REQUIRE(vec[i].left == list.sentinel());
            }
        }
    }
    SECTION("push back") {
        for (auto &e: vec) {
            list.push_back(&e);
        }
        REQUIRE(&list.front() == &vec[0]);
        REQUIRE(&list.back() == &vec[vec_size - 1]);
        for (std::size_t i{}; i < vec_size; i++) {
            if (i < (vec_size - 1)) {
                REQUIRE(vec[i].right == &vec[i + 1]);
            } else {
                REQUIRE(vec[i].right == list.sentinel());
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
            list.push_back(&e);
        }
        for (std::size_t i{}; i < vec_size; i++) {
            REQUIRE(&vec[i] == &list.front());
            REQUIRE(&vec[vec_size - 1] == &list.back());
            list.pop_front();
        }
        REQUIRE(list.empty());
    }
    SECTION("pop back") {
        for (auto &e: vec) {
            list.push_front(&e);
        }
        for (std::size_t i{}; i < vec_size; i++) {
            REQUIRE(&vec[i] == &list.back());
            REQUIRE(&vec[vec_size - 1] == &list.front());
            list.pop_back();
        }
        REQUIRE(list.empty());
    }
    SECTION("remove") {
        static_assert(vec_size >= 5);
        for (std::size_t i{}; i < 5; i++) {
            list.push_back(&vec[i]);
        }
        list.remove(&vec[1]);
        REQUIRE(vec[0].right == &vec[2]);
        REQUIRE(vec[2].left == &vec[0]);
        REQUIRE(&list.front() == &vec[0]);
        REQUIRE(&list.back() == &vec[4]);
        list.remove(&vec[3]);
        REQUIRE(vec[2].right == &vec[4]);
        REQUIRE(vec[4].left == &vec[2]);
        REQUIRE(&list.front() == &vec[0]);
        REQUIRE(&list.back() == &vec[4]);
        list.remove(&vec[4]);
        REQUIRE(vec[2].right == list.sentinel());
        REQUIRE(&list.front() == &vec[0]);
        REQUIRE(&list.back() == &vec[2]);
        list.remove(&vec[0]);
        REQUIRE(vec[2].right == list.sentinel());
        REQUIRE(vec[2].left == list.sentinel());
        REQUIRE(&list.front() == &vec[2]);
        REQUIRE(&list.back() == &vec[2]);
        list.remove(&vec[2]);
        REQUIRE(list.empty());
    }
    SECTION("iterator") {
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
        for (auto it = list.cbegin(); it != list.cend(); it++) {
            REQUIRE(&(*it) == &vec[i]);
            i++;
        }
        i = {};
        for (auto it = clist.cbegin(); it != clist.cend(); it++) {
            REQUIRE(&(*it) == &vec[i]);
            i++;
        }
    }
    SECTION("reverse iterator") {
        for (auto &e: vec) {
            list.push_front(&e);
        }
        const auto &clist = list;
        std::size_t i;
        i = {};
        for (auto it = list.rbegin(); it != list.rend(); it++) {
            REQUIRE(&(*it) == &vec[i]);
            i++;
        }
        i = {};
        for (auto it = list.crbegin(); it != list.crend(); it++) {
            REQUIRE(&(*it) == &vec[i]);
            i++;
        }
        i = {};
        for (auto it = clist.rbegin(); it != clist.rend(); it++) {
            REQUIRE(&(*it) == &vec[i]);
            i++;
        }
        i = {};
        for (auto it = clist.crbegin(); it != clist.crend(); it++) {
            REQUIRE(&(*it) == &vec[i]);
            i++;
        }
    }
}