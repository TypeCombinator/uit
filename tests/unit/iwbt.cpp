#include "uit/isbt.hpp"
#include <vector>
#include <cmath>
#include "gtest/gtest.h"
#include "common/apple.hpp"

TEST(iwbt_test, empty) {
    uit::isbt<sbt_apple> tree{};
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
}

TEST(iwbt_test, insert_unique1) {
    uit::isbt<sbt_apple> tree{};
    sbt_apple a0{500, 0};

    tree.winsert_unique(&a0);
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), 1);
}

TEST(iwbt_test, insert_unique2) {
    uit::isbt<sbt_apple> tree{};
    sbt_apple a0{500, 0};
    sbt_apple a1{501, 1};

    tree.winsert_unique(&a0);
    tree.winsert_unique(&a1);
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), 2);
}

TEST(iwbt_test, insert_unique3) {
    uit::isbt<sbt_apple> tree{};
    sbt_apple a0{500, 0};
    sbt_apple a1{501, 1};
    sbt_apple a2{502, 2};
    sbt_apple a3{501, 3};
    const sbt_apple *result[4];

    result[0] = tree.winsert_unique(&a0);
    result[1] = tree.winsert_unique(&a1);
    result[2] = tree.winsert_unique(&a2);
    result[3] = tree.winsert_unique(&a3);

    EXPECT_EQ(result[0], nullptr);
    EXPECT_EQ(result[1], nullptr);
    EXPECT_EQ(result[2], nullptr);
    EXPECT_EQ(result[3], &a1);
    EXPECT_EQ(tree.size(), 3);
}

TEST(iwbt_test, insert_unique) {
    uit::isbt<sbt_apple> tree{};
    std::vector<sbt_apple> vec;
    const std::size_t vec_size = 100;
    // std::size_t max_height = std::ceil(1.44 * std::log2(vec_size + 1.5) - 1.33);

    vec.reserve(vec_size);
    for (std::size_t i = 0; i < vec_size; i++) {
        vec.emplace_back(i, i);
    }
    for (auto &i: vec) {
        tree.winsert_unique(&i);
    }

    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), vec_size);
    // EXPECT_LE(tree.height(), max_height);
}

TEST(iwbt_test, remove_unique) {
    uit::isbt<sbt_apple> tree{};
    sbt_apple a0{500, 0};
    sbt_apple a1{501, 1};
    sbt_apple a2{502, 2};
    sbt_apple a3{503, 3};
    const sbt_apple *result[8];

    tree.winsert_unique(&a0);
    tree.winsert_unique(&a1);
    tree.winsert_unique(&a2);
    tree.winsert_unique(&a3);

    result[0] = tree.remove_unique(a0);
    result[1] = tree.remove_unique(a1);
    result[2] = tree.remove_unique(a0);
    result[3] = tree.remove_unique(a1);
    result[4] = tree.remove_unique(a2);
    result[5] = tree.remove_unique(a2);
    result[6] = tree.remove_unique(a3);
    result[7] = tree.remove_unique(a3);

    EXPECT_EQ(result[0], &a0);
    EXPECT_EQ(result[1], &a1);
    EXPECT_EQ(result[2], nullptr);
    EXPECT_EQ(result[3], nullptr);

    EXPECT_EQ(result[4], &a2);
    EXPECT_EQ(result[5], nullptr);
    EXPECT_EQ(result[6], &a3);
    EXPECT_EQ(result[7], nullptr);

    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
}

TEST(iwbt_test, find) {
    uit::isbt<sbt_apple> tree{};
    sbt_apple a0{500, 0};
    sbt_apple a1{501, 1};
    sbt_apple a2{502, 2};
    sbt_apple a3{503, 3};
    sbt_apple a4{504, 4};

    tree.winsert_unique(&a0);
    tree.winsert_unique(&a1);
    tree.winsert_unique(&a2);
    tree.winsert_unique(&a3);

    EXPECT_EQ(tree.find(a0), &a0);
    EXPECT_EQ(tree.find(a1), &a1);
    EXPECT_EQ(tree.find(a2), &a2);
    EXPECT_EQ(tree.find(a3), &a3);
    EXPECT_EQ(tree.find(a4), nullptr);


    EXPECT_EQ(tree.find(500), &a0);
    EXPECT_EQ(tree.find(501), &a1);
    EXPECT_EQ(tree.find(502), &a2);
    EXPECT_EQ(tree.find(503), &a3);
    EXPECT_EQ(tree.find(504), nullptr);
}

TEST(iwbt_test, at) {
    uit::isbt<sbt_apple> tree{};
    sbt_apple a0{500, 0};
    sbt_apple a1{501, 1};
    sbt_apple a2{502, 2};
    sbt_apple a3{503, 3};

    tree.winsert_unique(&a0);
    tree.winsert_unique(&a1);
    tree.winsert_unique(&a2);
    tree.winsert_unique(&a3);

    EXPECT_EQ(tree.at(0), &a0);
    EXPECT_EQ(tree.at(1), &a1);
    EXPECT_EQ(tree.at(2), &a2);
    EXPECT_EQ(tree.at(3), &a3);
    EXPECT_EQ(tree.at(4), nullptr);
    EXPECT_EQ(tree.at(5), nullptr);
}

TEST(iwbt_test, position) {
    uit::isbt<sbt_apple> tree{};
    sbt_apple a0{500, 0};
    sbt_apple a1{501, 1};
    sbt_apple a2{502, 2};
    sbt_apple a3{503, 3};
    sbt_apple a4{504, 4};

    tree.winsert_unique(&a0);
    tree.winsert_unique(&a1);
    tree.winsert_unique(&a2);
    tree.winsert_unique(&a3);

    EXPECT_EQ(tree.position(a0), 0);
    EXPECT_EQ(tree.position(a1), 1);
    EXPECT_EQ(tree.position(a2), 2);
    EXPECT_EQ(tree.position(a3), 3);
    EXPECT_EQ(tree.position(a4), std::size_t(-1));


    EXPECT_EQ(tree.position(500), 0);
    EXPECT_EQ(tree.position(501), 1);
    EXPECT_EQ(tree.position(502), 2);
    EXPECT_EQ(tree.position(503), 3);
    EXPECT_EQ(tree.position(504), std::size_t(-1));
}
