#include <uit/irsbt.hpp>
#include <vector>
#include <cmath>
#include <gtest/gtest.h>
#include <common/apple.hpp>

TEST(isbt_test, empty) {
    uit::irsbt<rsbt_apple> tree{};
    EXPECT_TRUE(tree.empty());
    EXPECT_EQ(tree.size(), 0);
}

TEST(isbt_test, insert_unique1) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};

    tree.insert_unique(&a0);
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), 1);
}

TEST(isbt_test, insert_unique2) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};
    rsbt_apple a1{501, 1};

    tree.insert_unique(&a0);
    tree.insert_unique(&a1);
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), 2);
}

TEST(isbt_test, insert_unique3) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};
    rsbt_apple a1{501, 1};
    rsbt_apple a2{502, 2};
    rsbt_apple a3{501, 3};
    const rsbt_apple *result[4];

    result[0] = tree.insert_unique(&a0);
    result[1] = tree.insert_unique(&a1);
    result[2] = tree.insert_unique(&a2);
    result[3] = tree.insert_unique(&a3);

    EXPECT_EQ(result[0], nullptr);
    EXPECT_EQ(result[1], nullptr);
    EXPECT_EQ(result[2], nullptr);
    EXPECT_EQ(result[3], &a1);
    EXPECT_EQ(tree.size(), 3);
}

TEST(isbt_test, insert_unique) {
    uit::irsbt<rsbt_apple> tree{};
    std::vector<rsbt_apple> vec;
    const std::size_t vec_size = 10;
    std::size_t max_height = std::ceil(1.44 * std::log2(vec_size + 1.5) - 1.33);

    vec.reserve(vec_size);
    for (std::size_t i = 0; i < vec_size; i++) {
        vec.emplace_back(i, i);
    }
    for (auto &i: vec) {
        tree.insert_unique(&i);
    }
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), vec_size);
    EXPECT_LE(tree.height(), max_height);
}

TEST(isbt_test, insert_multi1) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};

    tree.insert_multi(&a0);
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), 1);
}

TEST(isbt_test, insert_multi2) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};
    rsbt_apple a1{501, 1};

    tree.insert_multi(&a0);
    tree.insert_multi(&a1);
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), 2);
}

TEST(isbt_test, insert_multi3) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};
    rsbt_apple a1{501, 1};
    rsbt_apple a2{502, 2};
    rsbt_apple a3{501, 3};

    tree.insert_multi(&a0);
    tree.insert_multi(&a1);
    tree.insert_multi(&a2);
    tree.insert_multi(&a3);
    EXPECT_EQ(tree.size(), 4);
    EXPECT_EQ(tree.count_multi(501), 2);
}

TEST(isbt_test, insert_multi) {
    uit::irsbt<rsbt_apple> tree{};
    std::vector<rsbt_apple> vec;
    const std::size_t vec_size = 100;
    std::size_t max_height = std::ceil(1.44 * std::log2(vec_size + 1.5) - 1.33);

    vec.reserve(vec_size);
    for (std::size_t i = 0; i < vec_size; i++) {
        vec.emplace_back(i, i);
    }
    for (auto &i: vec) {
        tree.insert_multi(&i);
    }
    EXPECT_FALSE(tree.empty());
    EXPECT_EQ(tree.size(), vec_size);
    EXPECT_LE(tree.height(), max_height);
}

TEST(isbt_test, remove_unique) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};
    rsbt_apple a1{501, 1};
    rsbt_apple a2{502, 2};
    rsbt_apple a3{503, 3};
    const rsbt_apple *result[8];

    tree.insert_unique(&a0);
    tree.insert_unique(&a1);
    tree.insert_unique(&a2);
    tree.insert_unique(&a3);

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

TEST(isbt_test, find) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};
    rsbt_apple a1{501, 1};
    rsbt_apple a2{502, 2};
    rsbt_apple a3{503, 3};
    rsbt_apple a4{504, 4};

    tree.insert_unique(&a0);
    tree.insert_unique(&a1);
    tree.insert_unique(&a2);
    tree.insert_unique(&a3);

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

TEST(isbt_test, at) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};
    rsbt_apple a1{501, 1};
    rsbt_apple a2{502, 2};
    rsbt_apple a3{503, 3};

    tree.insert_unique(&a0);
    tree.insert_unique(&a1);
    tree.insert_unique(&a2);
    tree.insert_unique(&a3);

    EXPECT_EQ(tree.at(0), &a0);
    EXPECT_EQ(tree.at(1), &a1);
    EXPECT_EQ(tree.at(2), &a2);
    EXPECT_EQ(tree.at(3), &a3);
    EXPECT_EQ(tree.at(4), nullptr);
    EXPECT_EQ(tree.at(5), nullptr);
}

TEST(isbt_test, position) {
    uit::irsbt<rsbt_apple> tree{};
    rsbt_apple a0{500, 0};
    rsbt_apple a1{501, 1};
    rsbt_apple a2{502, 2};
    rsbt_apple a3{503, 3};
    rsbt_apple a4{504, 4};

    tree.insert_unique(&a0);
    tree.insert_unique(&a1);
    tree.insert_unique(&a2);
    tree.insert_unique(&a3);

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
