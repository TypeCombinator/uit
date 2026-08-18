#include <iostream>
#include <uit/idlist_proxy.hpp>
#include <uit/intrusive.hpp>

struct list_head {
    struct list_head *next, *prev;
};

class apple {
   public:
    apple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    struct list_head node;
    int sn;
};

int main(int argc, char *argv[]) {
    struct list_head head;
    uit::idlist_proxy<&list_head::next, &list_head::prev> list{head};
    list.clear();

    apple a0{500, 0};
    apple a1{501, 1};
    apple a2{502, 2};
    apple a3{503, 3};

    list.push_front(&a0.node);
    list.push_front(&a1.node);
    list.push_front(&a3.node);
    list.push_front(&a2.node);

    std::cout << "list:" << std::endl;
    for (const auto &i: list) {
        // UB!!!
        const apple *iptr = uit::const_container_of(&apple::node, &i);
        std::cout << "sn: " << iptr->sn << ", weight: " << iptr->weight << std::endl;
    }
    return 0;
}