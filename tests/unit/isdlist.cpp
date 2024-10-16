#include "gtest/gtest.h"
#include "common/apple.hpp"
#include "uit/isdlist.hpp"

TEST(isdlist_test, empty) {
    uit::isdlist<dapple> list{};
    EXPECT_TRUE(list.empty());
}

TEST(isdlist_test, push_front1) {
    uit::isdlist<dapple> list{};
    dapple a0(501, 0);

    list.push_front(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_FALSE(list.empty());
    EXPECT_EQ(a0.right, nullptr);
}

TEST(isdlist_test, push_front2) {
    uit::isdlist<dapple> list{};
    dapple a0(500, 0);
    dapple a1(501, 1);

    list.push_front(&a0);
    list.push_front(&a1);

    EXPECT_FALSE(list.empty());
    const dapple &second_to_last = list.front();
    EXPECT_EQ(&a1, &list.front());
    EXPECT_EQ(&a0, list.front().right);
    EXPECT_EQ(a0.left, &a1);
    EXPECT_EQ(a0.right, nullptr);
}

TEST(isdlist_test, remove) {
    uit::isdlist<dapple> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};
    dapple a2{502, 2};

    list.push_front(&a2);
    list.push_front(&a1);
    list.push_front(&a0);
    EXPECT_FALSE(list.empty());

    list.remove(&a1);

    const auto *result = list.pop_front();
    EXPECT_EQ(result, &a0);

    result = list.pop_front();
    EXPECT_EQ(result, &a2);

    EXPECT_EQ(list.pop_front(), nullptr);
}

TEST(isdlist_test, remove1) {
    uit::isdlist<dapple> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};
    dapple a2{502, 2};

    list.push_front(&a0);
    list.push_front(&a1);
    list.push_front(&a2);
    EXPECT_FALSE(list.empty());

    const dapple *moc_head = a2.left;

    list.remove(&a1);
    EXPECT_EQ(&list.front(), &a2);
    EXPECT_EQ(a2.right, &a0);
    EXPECT_EQ(a0.left, &a2);

    list.remove(&a2);
    EXPECT_EQ(&list.front(), &a0);
    EXPECT_EQ(a0.left, moc_head);

    list.remove(&a0);
    EXPECT_TRUE(list.empty());

    list.push_front(&a2);
    EXPECT_EQ(&list.front(), &a2);
    EXPECT_EQ(a2.left, moc_head);
}

TEST(isdlist_test, iterator) {
    uit::isdlist<dapple> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};
    dapple a2{502, 2};
    dapple a3{503, 3};

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

TEST(isdlist_test, range_based_for) {
    uit::isdlist<dapple> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};
    dapple a2{502, 2};
    dapple a3{503, 3};

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

TEST(isdlist_test, algorithm) {
    uit::isdlist<dapple> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};
    dapple a2{502, 2};
    dapple a3{503, 3};

    list.push_front(&a3);
    list.push_front(&a2);
    list.push_front(&a1);
    list.push_front(&a0);

    uint32_t sn = 0;
    std::for_each(list.cbegin(), list.cend(), [&sn](const dapple &i) -> void {
        EXPECT_EQ(i.weight, 500 + sn);
        EXPECT_EQ(i.sn, sn);
        sn++;
    });

    auto result = std::find_if(list.cbegin(), list.cend(), [](const dapple &i) -> bool {
        return i.weight >= 501;
    });
    EXPECT_NE(result, list.cend());
    EXPECT_EQ(result->weight, 501);
    EXPECT_EQ(result->sn, 1);
}

TEST(isdlist_test, copy) {
    static_assert(!std::is_copy_constructible<uit::isdlist<dapple>>::value, "");
    static_assert(!std::is_copy_assignable<uit::isdlist<dapple>>::value, "");
}

TEST(isdlist_test, move_ctor) {
    static_assert(std::is_move_constructible<uit::isdlist<dapple>>::value, "");
    {
        uit::isdlist<dapple> list{};
        uit::isdlist<dapple> list_other{std::move(list)};

        EXPECT_TRUE(list.empty());
        EXPECT_TRUE(list_other.empty());
    }
    {
        uit::isdlist<dapple> list{};
        dapple a0{500, 0};
        dapple a1{501, 1};
        dapple a2{502, 2};
        dapple a3{503, 3};

        list.push_front(&a3);
        list.push_front(&a2);
        list.push_front(&a1);
        list.push_front(&a0);

        uit::isdlist<dapple> list_other{std::move(list)};

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

TEST(isdlist_test, move_assign) {
    static_assert(std::is_move_assignable<uit::isdlist<dapple>>::value, "");
    {
        uit::isdlist<dapple> list{};
        uit::isdlist<dapple> list_other{};

        list_other = std::move(list);
        EXPECT_TRUE(list.empty());
        EXPECT_TRUE(list_other.empty());
    }
    {
        uit::isdlist<dapple> list{};
        uit::isdlist<dapple> list_other{};

        dapple a0{500, 0};
        dapple a1{501, 1};
        dapple a2{502, 2};
        dapple a3{503, 3};

        list.push_front(&a3);
        list.push_front(&a2);
        list.push_front(&a1);
        list.push_front(&a0);

        // Move self.
        list = std::move(list);

        EXPECT_FALSE(list.empty());

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