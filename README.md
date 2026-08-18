# UIT

## Introduction

**UIT** is a Unified Intrusive Linked List library. This library provides a unified implementation of linear linked lists and tree-type linked lists, allowing different types of linked lists to be flexibly converted into one another.

### Linear Linked List

| types          | number of head  node pointers | number of data node pointers | order        | comments                                                     |
| -------------- | ----------------------------- | ---------------------------- | ------------ | ------------------------------------------------------------ |
| `uit::islist`  | single                        | single                       | LIFO         | Singly Linked List.                                          |
| `uit::isdlist` | single                        | double                       | LIFO         | It's equivalent to the hlist in linux, and mainly used to implement hash tables. |
| `uit::idslist` | double                        | single                       | LIFO or FIFO | Data nodes have a smaller footprint.                         |
| `uit::idlist`  | double                        | double                       | LIFO or FIFO | Doubly Linked List.                                          |

### Tree-Type Linked List

| types         | was mock_sentinel used?            | comments                                                     |
| ------------- | ---------------------------------- | ------------------------------------------------------------ |
| `uit::irsbt`  | Yes (but the code works correctly) | Intrusive Recursive Size-Balanced Tree                       |
| `uit::irwbt`  | Yes (but the code works correctly) | Intrusive Recursive Weight-Balanced Tree<br />It's is a top-down implementation that avoids recursion. |
| `uit::irheap` | **No**                             | Intrusive Recursive Heap<br />Actually, recursion is not used, it's fully implemented with iteration. |
| `uit::iheap`  | **No**                             | Intrusive Heap<br />The code isn't in this repository, see the [PR](https://github.com/NVIDIA/stdexec/pull/1674) to stdexec. |

### Others

| types          | comments                                                     |
| -------------- | ------------------------------------------------------------ |
| `uit::iiqheap` | Intrusive Indexed Quad Heap<br />Simpler code and better performance, but not suitable for scenarios where the upper limit of timer count is undetermined and delay-sensitive, as the internal pointer array may need resizing. |

## Examples

### `uit::islist`

```c++
#include <iostream>
#include <uit/islist.hpp>

class apple {
   public:
    apple(uint64_t weight, int sn) noexcept
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    apple *right;
    int sn;
};

int main(int argc, char *argv[]) {
    uit::islist<&apple::right> list{};

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
```

### `uit:irheap`

```c++
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
    uit::irheap<&apple::right, &apple::left> q{}; // A priority queue.

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
```

### More

Please refer to the **[examples](examples/)** and **[tests](tests/unit/)** folder.

## Radicalism

The C++ object model has many limitations, which cause some inconveniences in both the design and usage of this library. For example, the remove method of `uit::idlist` cannot be implemented as a static method, which is far less elegant than the Linux kernel linked list implementation. If we do not care about the constraints of the C++ object model, pursue symmetric and beautiful code, and allow some undefined behavior, we can refer to the implementation under the `uit::experiment` namespace.

This implementation uses the **mock_head** trick to simulate a sentinel node, which greatly simplifies the implementation of linear linked lists. See the [Experiment.md](Experiment.md) document for details.