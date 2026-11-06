// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
// SPDX-License-Identifier: MPL-2.0
//
// This project is licensed under the Mozilla Public License 2.0.
// Please see the accompanying LICENSE file in this project root
// or https://spdx.org/licenses/MPL-2.0.html for details.

#include <cstdint>
#include <iostream>
#include <uit/islist.hpp>

class apple {
   public:
    apple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
   private:
    apple *right;
   public:
    int sn;

    using node_list_t = uit::islist<&apple::right>;
};

int main(int argc, char *argv[]) {
    apple::node_list_t list{};

    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};
    apple a3{503, 3};

    list.push_front(&a3);
    list.push_front(&a2);
    list.push_front(&a1);
    list.push_front(&a0);

    for (const auto &i: list) {
        std::cout << "sn: " << i.sn << ", weight: " << i.weight << std::endl;
    }
    return 0;
}