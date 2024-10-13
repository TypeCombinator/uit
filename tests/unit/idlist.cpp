#include "gtest/gtest.h"
#include "common/apple.hpp"
#include "uit/idlist.hpp"

TEST(idlist_test, empty) {
    uit::idlist<&dapple::node> list{};
    EXPECT_TRUE(list.empty());
}

TEST(idlist_test, push_back1) {
    uit::idlist<&dapple::node> list{};
    dapple a0(500, 0);

    list.push_back(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_EQ(&a0, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idlist_test, push_back2) {
    uit::idlist<&dapple::node> list{};
    dapple a0(500, 0);
    dapple a1(501, 1);

    list.push_back(&a0);
    list.push_back(&a1);

    const dapple &first = list.front();
    EXPECT_EQ(&a0, &first);

    const dapple *second = first.node.right;
    EXPECT_EQ(&a1, second);
    EXPECT_EQ(&a1, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idlist_test, push_front1) {
    uit::idlist<&dapple::node> list{};
    dapple a0(501, 0);

    list.push_front(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_EQ(&a0, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idlist_test, push_front2) {
    uit::idlist<&dapple::node> list{};
    dapple a0(500, 0);
    dapple a1(501, 1);

    list.push_front(&a0);
    list.push_front(&a1);

    const dapple &last = list.back();
    EXPECT_EQ(&a0, &last);

    const dapple *second_to_last = last.node.left;
    EXPECT_EQ(&a1, second_to_last);
    EXPECT_EQ(&a1, &list.front());
    EXPECT_FALSE(list.empty());
}

TEST(idlist_test, clear) {
    uit::idlist<&dapple::node> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};

    list.push_front(&a0);
    list.push_front(&a1);
    EXPECT_FALSE(list.empty());

    list.clear();
    EXPECT_TRUE(list.empty());
}

TEST(idlist_test, remove) {
    uit::idlist<&dapple::node> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};
    dapple a2{502, 2};

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

TEST(idlist_test, iterator) {
    uit::idlist<&dapple::node> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};
    dapple a2{502, 2};
    dapple a3{503, 3};

    list.push_back(&a2);
    list.push_back(&a3);

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

TEST(idlist_test, reverse_iterator) {
    uit::idlist<&dapple::node> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};
    dapple a2{502, 2};
    dapple a3{503, 3};

    list.push_front(&a0);
    list.push_front(&a1);
    list.push_front(&a2);
    list.push_front(&a3);


    {
        uint64_t weight = 500;
        int sn = 0;
        auto it_end = list.rend();
        for (auto it = list.rbegin(); it != it_end; it++) {
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
        auto it_end = clist.rend();
        for (auto it = clist.rbegin(); it != it_end; it++) {
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
        auto it_end = list.crend();
        for (auto it = list.crbegin(); it != it_end; it++) {
            EXPECT_EQ(it->weight, weight);
            EXPECT_EQ(it->sn, sn);
            weight++;
            sn++;
        }
    }
}

TEST(idlist_test, algorithm) {
    uit::idlist<&dapple::node> list{};
    dapple a0{500, 0};
    dapple a1{501, 1};
    dapple a2{502, 2};
    dapple a3{503, 3};

    list.push_back(&a0);
    list.push_back(&a1);
    list.push_back(&a2);
    list.push_back(&a3);

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

TEST(idlist_test, copy) {
    static_assert(!std::is_copy_constructible<uit::idlist<&dapple::node>>::value, "");
    static_assert(!std::is_copy_assignable<uit::idlist<&dapple::node>>::value, "");
}

TEST(idlist_test, move_ctor) {
    static_assert(std::is_move_constructible<uit::idlist<&dapple::node>>::value, "");
    {
        uit::idlist<&dapple::node> list{};
        uit::idlist<&dapple::node> list_other{std::move(list)};

        EXPECT_TRUE(list.empty());
        EXPECT_TRUE(list_other.empty());
    }
    {
        uit::idlist<&dapple::node> list{};
        dapple a0{500, 0};
        dapple a1{501, 1};
        dapple a2{502, 2};
        dapple a3{503, 3};

        list.push_front(&a3);
        list.push_front(&a2);
        list.push_front(&a1);
        list.push_front(&a0);

        uit::idlist<&dapple::node> list_other{std::move(list)};

        EXPECT_TRUE(list.empty());
        EXPECT_FALSE(list_other.empty());
        EXPECT_EQ(&list_other.front(), &a0);
        EXPECT_EQ(&list_other.back(), &a3);
        EXPECT_EQ(a0.node.left, a3.node.right);

        uint32_t sn = 0;
        for (const auto &i: list_other) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}

TEST(idlist_test, move_assign) {
    static_assert(std::is_move_assignable<uit::idlist<&dapple::node>>::value, "");
    {
        uit::idlist<&dapple::node> list{};
        uit::idlist<&dapple::node> list_other{};

        list_other = std::move(list);
        EXPECT_TRUE(list.empty());
        EXPECT_TRUE(list_other.empty());
    }
    {
        uit::idlist<&dapple::node> list{};
        uit::idlist<&dapple::node> list_other{};

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
        EXPECT_EQ(a0.node.left, a3.node.right);

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
        EXPECT_EQ(a0.node.left, a3.node.right);

        sn = 0;
        for (const auto &i: list) {
            EXPECT_EQ(i.weight, 500 + sn);
            EXPECT_EQ(i.sn, sn);
            sn++;
        }
    }
}