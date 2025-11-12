#include <iostream>
#include <uit/irheap.hpp>

class apple {
   public:
    apple(uint64_t weight, int sn) noexcept
        : weight{weight}
        , sn{sn} {
    }

    bool operator<(const apple &other) const noexcept {
        return weight < other.weight;
    }

    uint64_t weight;
    apple *right;
    apple *left;
    int sn;
};

int main(int argc, char *argv[]) {
    uit::irheap<&apple::right, &apple::left> priority_q{};

    apple a0{502, 0};
    apple a1{503, 1};
    apple a2{501, 2};
    apple a3{500, 3};

    priority_q.push(&a0);
    priority_q.push(&a1);
    priority_q.push(&a2);
    priority_q.push(&a3);

    for (; !priority_q.empty(); priority_q.pop()) {
        const apple &node = priority_q.top();
        std::cout << "sn: " << node.sn << ", weight: " << node.weight << std::endl;
    }
    return 0;
}