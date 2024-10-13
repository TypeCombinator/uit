#include "gtest/gtest.h"
#include "common/apple.hpp"
#include "uit/idslist.hpp"

TEST(idslist_test, empty) {
    uit::idslist<&sapple::node> list{};
    EXPECT_TRUE(list.empty());
}

TEST(idslist_test, push_back1) {
    uit::idslist<&sapple::node> list{};
    sapple a0(500, 0);

    list.push_back(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_EQ(&a0, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idslist_test, push_back2) {
    uit::idslist<&sapple::node> list{};
    sapple a0(500, 0);
    sapple a1(501, 1);

    list.push_back(&a0);
    list.push_back(&a1);

    const sapple &first = list.front();
    EXPECT_EQ(&a0, &first);

    const sapple *second = first.node.right;
    EXPECT_EQ(&a1, second);
    EXPECT_EQ(&a1, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idslist_test, push_front1) {
    uit::idslist<&sapple::node> list{};
    sapple a0(501, 0);

    list.push_front(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_EQ(&a0, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idslist_test, push_front2) {
    uit::idslist<&sapple::node> list{};
    sapple a0(500, 0);
    sapple a1(501, 1);

    list.push_front(&a0);
    list.push_front(&a1);

    const sapple &last = list.back();
    EXPECT_EQ(&a0, &last);
    EXPECT_EQ(a0.node.right, nullptr);

    const sapple &second_to_last = list.front();
    EXPECT_EQ(&a1, &second_to_last);
    EXPECT_EQ(a1.node.right, &a0);
    EXPECT_FALSE(list.empty());
}

TEST(idslist_test, remove) {
    uit::idslist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};
    sapple a2{502, 2};

    list.push_back(&a0);
    list.push_back(&a1);
    list.push_back(&a2);
    EXPECT_FALSE(list.empty());

    list.remove(&a1);

    const auto *result = list.pop_front();
    EXPECT_EQ(result, &a0);

    result = list.pop_front();
    EXPECT_EQ(result, &a2);

    EXPECT_EQ(list.pop_front(), nullptr);
}

TEST(idslist_test, remove1) {
    uit::idslist<&sapple::node> list{};
    sapple a0{500, 0};
    sapple a1{501, 1};
    sapple a2{502, 2};

    list.push_back(&a0);
    list.push_back(&a1);
    list.push_back(&a2);
    EXPECT_FALSE(list.empty());


    list.remove(&a1);
    EXPECT_EQ(&list.front(), &a0);
    EXPECT_EQ(&list.back(), &a2);

    list.remove(&a0);
    EXPECT_EQ(&list.front(), &a2);
    EXPECT_EQ(&list.back(), &a2);

    list.remove(&a2);
    EXPECT_TRUE(list.empty());

    list.push_back(&a0);
    EXPECT_EQ(&list.front(), &a0);
    EXPECT_EQ(&list.back(), &a0);
}

TEST(idslist_test, iterator) {
    uit::idslist<&sapple::node> list{};
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

TEST(idslist_test, range_based_for) {
    uit::idslist<&sapple::node> list{};
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

TEST(idslist_test, algorithm) {
    uit::idslist<&sapple::node> list{};
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

TEST(idslist_test, copy_ctor) {
    static_assert(std::is_copy_constructible<uit::idslist<&sapple::node>>::value, "");
    {
        uit::idslist<&sapple::node> list{};
        uit::idslist<&sapple::node> list_other{list};

        EXPECT_TRUE(list_other.empty());
        EXPECT_TRUE(list.empty());
    }
    {
        uit::idslist<&sapple::node> list{};
        sapple a0{500, 0};
        sapple a1{501, 1};
        sapple a2{502, 2};
        sapple a3{503, 3};

        list.push_front(&a3);
        list.push_front(&a2);
        list.push_front(&a1);
        list.push_front(&a0);

        uit::idslist<&sapple::node> list_other{list};
        EXPECT_FALSE(list_other.empty());
        EXPECT_EQ(&list_other.front(), &a0);
        EXPECT_EQ(&list_other.back(), &a3);

        uint32_t sn = 0;
        for (const auto &i: list_other) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}

TEST(idslist_test, copy_assign) {
    static_assert(std::is_copy_assignable<uit::idslist<&sapple::node>>::value, "");
    {
        uit::idslist<&sapple::node> list{};
        uit::idslist<&sapple::node> list_other{};

        list_other = list;

        EXPECT_TRUE(list_other.empty());
        EXPECT_TRUE(list.empty());
    }
    {
        uit::idslist<&sapple::node> list{};
        sapple a0{500, 0};
        sapple a1{501, 1};
        sapple a2{502, 2};
        sapple a3{503, 3};

        list.push_front(&a3);
        list.push_front(&a2);
        list.push_front(&a1);
        list.push_front(&a0);

        uit::idslist<&sapple::node> list_other{};

        // Copy self.
        list = list;
        EXPECT_EQ(&list.front(), &a0);
        EXPECT_EQ(&list.back(), &a3);

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
        EXPECT_EQ(&list_other.back(), &a3);

        sn = 0;
        for (const auto &i: list_other) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}

TEST(idslist_test, move_ctor) {
    static_assert(std::is_move_constructible<uit::idslist<&sapple::node>>::value, "");
    {
        uit::idslist<&sapple::node> list{};
        uit::idslist<&sapple::node> list_other{std::move(list)};
        EXPECT_TRUE(list.empty());
        EXPECT_TRUE(list_other.empty());
    }
    {
        uit::idslist<&sapple::node> list{};
        sapple a0{500, 0};
        sapple a1{501, 1};
        sapple a2{502, 2};
        sapple a3{503, 3};

        list.push_back(&a0);
        list.push_back(&a1);
        list.push_back(&a2);
        list.push_back(&a3);

        uit::idslist<&sapple::node> list_other{std::move(list)};

        EXPECT_TRUE(list.empty());
        EXPECT_FALSE(list_other.empty());
        EXPECT_EQ(&list_other.front(), &a0);
        EXPECT_EQ(&list_other.back(), &a3);

        uint32_t sn = 0;
        for (const auto &i: list_other) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}

TEST(idslist_test, move_assign) {
    static_assert(std::is_move_assignable<uit::idslist<&sapple::node>>::value, "");
    {
        uit::idslist<&sapple::node> list{};
        uit::idslist<&sapple::node> list_other{};

        list_other = list;
        EXPECT_TRUE(list.empty());
        EXPECT_TRUE(list_other.empty());
    }
    {
        uit::idslist<&sapple::node> list{};
        uit::idslist<&sapple::node> list_other{};

        sapple a0{500, 0};
        sapple a1{501, 1};
        sapple a2{502, 2};
        sapple a3{503, 3};

        list.push_back(&a0);
        list.push_back(&a1);
        list.push_back(&a2);
        list.push_back(&a3);

        // Move self.
        list = std::move(list);

        EXPECT_EQ(&list.front(), &a0);
        EXPECT_EQ(&list.back(), &a3);

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
        EXPECT_EQ(&list_other.back(), &a3);

        sn = 0;
        for (const auto &i: list_other) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}