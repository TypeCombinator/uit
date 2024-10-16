#include <iostream>
#include <uit/islist.hpp>

class apple
    : public uit::isnode<apple, "0">
    , private uit::isnode<apple, "1"> {
   public:
    apple(uint64_t weight, int sn)
        : weight(weight)
        , sn(sn) {
    }
    friend class uit::islist<apple, "1">;
    uint64_t weight;
    int sn;
};

int main(int argc, char *argv[]) {
    uit::islist<apple, "0"> list{};
    uit::islist<apple, "1"> list1{};

    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};
    apple a3{503, 3};

    list.push_front(&a3);
    list.push_front(&a2);
    list.push_front(&a1);
    list.push_front(&a0);

    list1.push_front(&a0);
    list1.push_front(&a1);
    list1.push_front(&a2);
    list1.push_front(&a3);

    std::cout << "list:" << std::endl;
    for (const auto &i: list) {
        std::cout << "sn: " << i.sn << ", weight: " << i.weight << std::endl;
    }

    std::cout << "list1:" << std::endl;
    for (const auto &i: list1) {
        std::cout << "sn: " << i.sn << ", weight: " << i.weight << std::endl;
    }
    return 0;
}