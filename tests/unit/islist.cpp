// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#include <algorithm>
#include "gtest/gtest.h"
#include "common/apple.hpp"
#include "uit/islist.hpp"

TEST(islist_test, empty) {
    uit::islist<&sapple::node> list{};
    EXPECT_TRUE(list.empty());
}

TEST(islist_test, push_front1) {
    uit::islist<&sapple::node> list{};

    sapple a0{501, 0};

    list.push_front(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_EQ(a0.node.right, nullptr);
    EXPECT_FALSE(list.empty());
}

TEST(islist_test, push_front2) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};

    list.push_front(&a0);
    list.push_front(&a1);
    const sapple &second_to_last = list.front();
    EXPECT_EQ(&a1, &list.front());
    EXPECT_EQ(&a0, list.front().node.right);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(a0.node.right, nullptr);
    EXPECT_EQ(a1.node.right, &a0);
}

TEST(islist_test, pop_front0) {
    uit::islist<&sapple::node> list{};
    EXPECT_EQ(list.pop_front(), nullptr);
}

TEST(islist_test, pop_front1) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    list.push_front(&a0);

    const sapple *first = list.pop_front();
    EXPECT_EQ(first, &a0);
    EXPECT_EQ(list.pop_front(), nullptr);
    EXPECT_TRUE(list.empty());
}

TEST(islist_test, pop_front2) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{500, 1};
    list.push_front(&a0);
    list.push_front(&a1);

    EXPECT_EQ(list.pop_front(), &a1);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(list.pop_front(), &a0);
    EXPECT_TRUE(list.empty());
}

TEST(islist_test, remove) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};
    sapple a2{502, 2};

    list.push_front(&a0);
    list.push_front(&a1);
    list.push_front(&a2);

    const sapple *result = list.remove(&a1);
    EXPECT_EQ(result, &a1);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(a2.node.right, &a0);
    EXPECT_EQ(a0.node.right, nullptr);

    result = list.remove(&a0);
    EXPECT_EQ(result, &a0);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(a2.node.right, nullptr);

    result = list.remove_without_ub(&a2);
    EXPECT_EQ(result, &a2);
    EXPECT_TRUE(list.empty());
}

TEST(islist_test, remove_without_ub) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};
    sapple a2{502, 2};

    list.push_front(&a0);
    list.push_front(&a1);
    list.push_front(&a2);

    const sapple *result = list.remove_without_ub(&a1);
    EXPECT_EQ(result, &a1);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(a2.node.right, &a0);
    EXPECT_EQ(a0.node.right, nullptr);

    result = list.remove_without_ub(&a0);
    EXPECT_EQ(result, &a0);
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(a2.node.right, nullptr);

    result = list.remove_without_ub(&a2);
    EXPECT_EQ(result, &a2);
    EXPECT_TRUE(list.empty());
}

TEST(islist_test, remove1) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};
    sapple a2{502, 2};

    list.push_front(&a2);
    list.push_front(&a1);
    list.push_front(&a0);
    EXPECT_FALSE(list.empty());


    list.remove(&a1);
    EXPECT_EQ(&list.front(), &a0);

    list.remove(&a0);
    EXPECT_EQ(&list.front(), &a2);

    list.remove(&a2);
    EXPECT_TRUE(list.empty());

    list.push_front(&a0);
    EXPECT_EQ(&list.front(), &a0);
}

TEST(islist_test, clear) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};

    list.push_front(&a0);
    list.push_front(&a1);
    EXPECT_FALSE(list.empty());

    list.clear();
    EXPECT_TRUE(list.empty());
}

TEST(islist_test, iterator) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};
    sapple a2{502, 2};
    sapple a3{503, 3};

    list.push_front(&a3);
    list.push_front(&a2);
    list.push_front(&a1);
    list.push_front(&a0);

    {
        uint64_t weight = 500;
        int sn = 0;
        auto it_end = list.end();
        for (auto it = list.begin(); it != it_end; it++) {
            EXPECT_EQ(it->weight, weight);
            EXPECT_EQ(it->sn, sn);
            weight++;
            sn++;
        }
    }

    {
        const auto &clist = list;
        uint64_t weight = 500;
        int sn = 0;
        auto it_end = clist.end();
        for (auto it = clist.begin(); it != it_end; it++) {
            EXPECT_EQ(it->weight, weight);
            EXPECT_EQ(it->sn, sn);
            weight++;
            sn++;
        }
    }

    {
        const auto &clist = list;
        uint64_t weight = 500;
        int sn = 0;
        auto it_end = list.cend();
        for (auto it = list.cbegin(); it != it_end; it++) {
            EXPECT_EQ(it->weight, weight);
            EXPECT_EQ(it->sn, sn);
            weight++;
            sn++;
        }
    }
}

TEST(islist_test, range_based_for) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};
    sapple a2{502, 2};
    sapple a3{503, 3};

    list.push_front(&a3);
    list.push_front(&a2);
    list.push_front(&a1);
    list.push_front(&a0);

    uint64_t weight = 500;
    int sn = 0;
    for (auto &i: list) {
        EXPECT_EQ(i.weight, weight);
        EXPECT_EQ(i.sn, sn);
        weight++;
        sn++;
    }
    weight = 500;
    sn = 0;
    const auto &clist = list;
    for (const auto &i: clist) {
        EXPECT_EQ(i.weight, weight);
        EXPECT_EQ(i.sn, sn);
        weight++;
        sn++;
    }
}

TEST(islist_test, algorithm) {
    uit::islist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};
    sapple a2{502, 2};
    sapple a3{503, 3};

    list.push_front(&a3);
    list.push_front(&a2);
    list.push_front(&a1);
    list.push_front(&a0);

    uint32_t sn = 0;
    std::for_each(list.cbegin(), list.cend(), [&sn](const sapple &i) -> void {
        EXPECT_EQ(i.weight, 500 + sn);
        EXPECT_EQ(i.sn, sn);
        sn++;
    });

    auto result = std::find_if(list.cbegin(), list.cend(), [](const sapple &i) -> bool {
        return i.weight >= 501;
    });
    EXPECT_NE(result, list.cend());
    EXPECT_EQ(result->weight, 501);
    EXPECT_EQ(result->sn, 1);
}

TEST(islist_test, copy_ctor) {
    static_assert(std::is_copy_constructible<uit::islist<&sapple::node>>::value, "");
    {
        uit::islist<&sapple::node> list{};
        uit::islist<&sapple::node> list_other{list};

        EXPECT_TRUE(list_other.empty());
        EXPECT_TRUE(list.empty());
    }
    {
        uit::islist<&sapple::node> list{};
        sapple a0{500, 0};
        sapple a1{501, 1};
        sapple a2{502, 2};
        sapple a3{503, 3};

        list.push_front(&a3);
        list.push_front(&a2);
        list.push_front(&a1);
        list.push_front(&a0);

        uit::islist<&sapple::node> list_other{list};
        EXPECT_FALSE(list_other.empty());
        EXPECT_EQ(&list_other.front(), &a0);

        uint32_t sn = 0;
        for (const auto &i: list_other) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}

TEST(islist_test, copy_assign) {
    static_assert(std::is_copy_assignable<uit::islist<&sapple::node>>::value, "");
    {
        uit::islist<&sapple::node> list{};
        uit::islist<&sapple::node> list_other{};

        list_other = list;

        EXPECT_TRUE(list_other.empty());
        EXPECT_TRUE(list.empty());
    }
    {
        uit::islist<&sapple::node> list{};
        sapple a0{500, 0};
        sapple a1{501, 1};
        sapple a2{502, 2};
        sapple a3{503, 3};

        list.push_front(&a3);
        list.push_front(&a2);
        list.push_front(&a1);
        list.push_front(&a0);

        uit::islist<&sapple::node> list_other{};

        // Copy self.
        list = list;
        EXPECT_EQ(&list.front(), &a0);

        uint32_t sn = 0;
        for (const auto &i: list) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }

        // Copy other.
        list_other = list;
        EXPECT_FALSE(list_other.empty());
        EXPECT_EQ(&list_other.front(), &a0);

        sn = 0;
        for (const auto &i: list_other) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}

TEST(islist_test, move_ctor) {
    static_assert(std::is_move_constructible<uit::islist<&sapple::node>>::value, "");

    {
        uit::islist<&sapple::node> list{};
        uit::islist<&sapple::node> list_other{std::move(list)};
        EXPECT_TRUE(list.empty());
        EXPECT_TRUE(list_other.empty());
    }
    {
        uit::islist<&sapple::node> list{};
        sapple a0{500, 0};
        sapple a1{501, 1};
        sapple a2{502, 2};
        sapple a3{503, 3};

        list.push_front(&a3);
        list.push_front(&a2);
        list.push_front(&a1);
        list.push_front(&a0);

        uit::islist<&sapple::node> list_other{std::move(list)};

        EXPECT_TRUE(list.empty());
        EXPECT_FALSE(list_other.empty());
        EXPECT_EQ(&list_other.front(), &a0);

        uint32_t sn = 0;
        for (const auto &i: list_other) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}

TEST(islist_test, move_assign) {
    static_assert(std::is_move_assignable<uit::islist<&sapple::node>>::value, "");
    {
        uit::islist<&sapple::node> list{};
        uit::islist<&sapple::node> list_other{};

        list_other = list;
        EXPECT_TRUE(list.empty());
        EXPECT_TRUE(list_other.empty());
    }
    {
        uit::islist<&sapple::node> list{};
        uit::islist<&sapple::node> list_other{};

        sapple a0{500, 0};
        sapple a1{501, 1};
        sapple a2{502, 2};
        sapple a3{503, 3};

        list.push_front(&a3);
        list.push_front(&a2);
        list.push_front(&a1);
        list.push_front(&a0);

        // Move self.
        list = std::move(list);

        EXPECT_EQ(&list.front(), &a0);

        uint32_t sn = 0;
        for (const auto &i: list) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }

        // Move other.
        list_other = std::move(list);

        EXPECT_TRUE(list.empty());
        EXPECT_FALSE(list_other.empty());
        EXPECT_EQ(&list_other.front(), &a0);

        sn = 0;
        for (const auto &i: list_other) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}