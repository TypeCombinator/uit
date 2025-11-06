#include <iostream>
#include <uit/irheap.hpp>

class apple {
   public:
    apple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    bool operator<(const apple &other) noexcept {
        return weight < other.weight;
    }

    uint64_t weight;
    apple *right;
    apple *left;
    int sn;
};

int main(int argc, char *argv[]) {
    uit::irheap<&apple::right, &apple::left> priotity_q{};

    apple a0{502, 0};
    apple a1{503, 1};
    apple a2{501, 2};
    apple a3{500, 3};

    priotity_q.push(&a0);
    priotity_q.push(&a1);
    priotity_q.push(&a2);
    priotity_q.push(&a3);

    const apple *node;
    while ((node = priotity_q.pop()) != nullptr) {
        std::cout << "sn: " << node->sn << ", weight: " << node->weight << std::endl;
    }
    return 0;
}