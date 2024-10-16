#include <iostream>
#include <uit/idslist.hpp>

class apple : private uit::isnode<apple> {
   public:
    apple(uint64_t weight, int sn)
        : weight(weight)
        , sn(sn) {
    }

    friend uit::idslist<apple>;
    uint64_t weight;
    int sn;
};

int main(int argc, char *argv[]) {
    uit::idslist<apple> list{};

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