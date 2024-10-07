#include "gtest/gtest.h"
#include "uit/idslist.hpp"

struct apple {
    apple(uint64_t _weight, int _sn)
        : weight(_weight)
        , sn(_sn) {
    }

    uint64_t weight;
    uit::isnode<apple> node;
    int sn;
};

TEST(idslist_test, empty) {
    uit::idslist<&apple::node> list{};
    EXPECT_TRUE(list.empty());
}

TEST(idslist_test, push_back1) {
    uit::idslist<&apple::node> list{};
    apple a0(500, 0);

    list.push_back(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_EQ(&a0, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idslist_test, push_back2) {
    uit::idslist<&apple::node> list{};
    apple a0(500, 0);
    apple a1(501, 1);

    list.push_back(&a0);
    list.push_back(&a1);

    const apple &first = list.front();
    EXPECT_EQ(&a0, &first);

    const apple *second = first.node.right;
    EXPECT_EQ(&a1, second);
    EXPECT_EQ(&a1, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idslist_test, push_front1) {
    uit::idslist<&apple::node> list{};
    apple a0(501, 0);

    list.push_front(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_EQ(&a0, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idslist_test, push_front2) {
    uit::idslist<&apple::node> list{};
    apple a0(500, 0);
    apple a1(501, 1);

    list.push_front(&a0);
    list.push_front(&a1);

    const apple &last = list.back();
    EXPECT_EQ(&a0, &last);
    EXPECT_EQ(a0.node.right, nullptr);

    const apple &second_to_last = list.front();
    EXPECT_EQ(&a1, &second_to_last);
    EXPECT_EQ(a1.node.right, &a0);
    EXPECT_FALSE(list.empty());
}

TEST(idslist_test, remove) {
    uit::idslist<&apple::node> list{};
    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};

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
    uit::idslist<&apple::node> list{};
    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};

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
    uit::idslist<&apple::node> list{};
    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};
    apple a3{503, 3};

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
    uit::idslist<&apple::node> list{};
    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};
    apple a3{503, 3};

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
    uit::idslist<&apple::node> list{};
    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};
    apple a3{503, 3};

    list.push_front(&a3);
    list.push_front(&a2);
    list.push_front(&a1);
    list.push_front(&a0);

    uint32_t sn = 0;
    std::for_each(list.cbegin(), list.cend(), [&sn](const apple &i) -> void {
        EXPECT_EQ(i.weight, 500 + sn);
        EXPECT_EQ(i.sn, sn);
        sn++;
    });

    auto result = std::find_if(list.cbegin(), list.cend(), [](const apple &i) -> bool {
        return i.weight >= 501;
    });
    EXPECT_NE(result, list.cend());
    EXPECT_EQ(result->weight, 501);
    EXPECT_EQ(result->sn, 1);
}