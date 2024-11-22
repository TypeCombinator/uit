#include <iostream>
#include <uit/islist.hpp>

class apple {
   public:
    apple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    uit::isnode<apple> node;
   private:
    uit::isnode<apple> node1;
   public:
    int sn;

    using node_list_t = uit::islist<&apple::node1>;
};

int main(int argc, char *argv[]) {
    uit::islist<&apple::node> list{};
    apple::node_list_t list1{};

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