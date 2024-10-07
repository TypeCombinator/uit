#include "gtest/gtest.h"
#include "uit/idlist.hpp"

struct apple {
    apple(uint64_t weight, int sn)
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    uit::idnode<apple> node;
    int sn;
};

TEST(idlist_test, empty) {
    uit::idlist<&apple::node> list{};
    EXPECT_TRUE(list.empty());
}

TEST(idlist_test, push_back1) {
    uit::idlist<&apple::node> list{};
    apple a0(500, 0);

    list.push_back(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_EQ(&a0, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idlist_test, push_back2) {
    uit::idlist<&apple::node> list{};
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

TEST(idlist_test, push_front1) {
    uit::idlist<&apple::node> list{};
    apple a0(501, 0);

    list.push_front(&a0);
    EXPECT_EQ(&a0, &list.front());
    EXPECT_EQ(&a0, &list.back());
    EXPECT_FALSE(list.empty());
}

TEST(idlist_test, push_front2) {
    uit::idlist<&apple::node> list{};
    apple a0(500, 0);
    apple a1(501, 1);

    list.push_front(&a0);
    list.push_front(&a1);

    const apple &last = list.back();
    EXPECT_EQ(&a0, &last);

    const apple *second_to_last = last.node.left;
    EXPECT_EQ(&a1, second_to_last);
    EXPECT_EQ(&a1, &list.front());
    EXPECT_FALSE(list.empty());
}

TEST(idlist_test, clear) {
    uit::idlist<&apple::node> list{};
    apple a0{500, 0};
    apple a1{501, 1};

    list.push_front(&a0);
    list.push_front(&a1);
    EXPECT_FALSE(list.empty());

    list.clear();
    EXPECT_TRUE(list.empty());
}

TEST(idlist_test, remove) {
    uit::idlist<&apple::node> list{};
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

TEST(idlist_test, iterator) {
    uit::idlist<&apple::node> list{};
    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};
    apple a3{503, 3};

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
    uit::idlist<&apple::node> list{};
    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};
    apple a3{503, 3};

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
    uit::idlist<&apple::node> list{};
    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};
    apple a3{503, 3};

    list.push_back(&a0);
    list.push_back(&a1);
    list.push_back(&a2);
    list.push_back(&a3);

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