#include <iostream>
#include <uit/iiqheap.hpp>

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
    uint32_t index;
    int sn;
};

int main(int argc, char *argv[]) {
    uit::iiqheap<&apple::index> q{100}; // A priority queue.

    apple a0{502, 0};
    apple a1{503, 1};
    apple a2{501, 2};
    apple a3{500, 3};

    q.push(&a0);
    q.push(&a1);
    q.push(&a2);
    q.push(&a3);

    for (; !q.empty(); q.pop()) {
        const apple &node = q.top();
        std::cout << "sn: " << node.sn << ", weight: " << node.weight << std::endl;
    }
    return 0;
}