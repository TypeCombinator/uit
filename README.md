# UIT

## Introduction

**UIT** is a Unified Intrusive Linked List library. This library brings together 4 different linked lists based on a unified abstraction, the core trick is **mock_head**.

| types          | number of head  node pointers | number of data node pointers | order        | comments                                                     |
| -------------- | ----------------------------- | ---------------------------- | ------------ | ------------------------------------------------------------ |
| `uit::islist`  | single                        | single                       | LIFO         | Singly Linked List.                                          |
| `uit::isdlist` | single                        | double                       | LIFO         | It's equivalent to the hlist in linux, and mainly used to implement hash tables. |
| `uit::idslist` | double                        | single                       | LIFO or FIFO | It's mainly used to implement queues, node storage is more efficient than doubly linked lists. |
| `uit::idlist`  | double                        | double                       | LIFO or FIFO | Doubly Linked List.                                          |

The trick named **mock_head** brings the following advantages and disadvantages.

### advantages

- Simple and easy-to-understand code
- Fewer branches and better performance, e.g. there's no branch in the `uit::idslist::push_back` method
- Secondary pointers are no longer required, even the hlist(`uit::isdlist`) is no exception
- Truly zero overhead
- No macros

### disadvantages

- **mock_head** is an undefined behavior in C++, compiler may produce unexpected results.

### notice

This project is in the process of experimental exploration, for the **mock_head** is an UB, it cannot be used for production projects.

The project has been tested on GCC 11.4.0 and Clang 18.06, there were several failures, please see the table below.

|            | Number of failures in debug | Number of failures in release(-O3) |
| ---------- | --------------------------- | ---------------------------------- |
| gcc11.4.0  | 0                           | 0                                  |
| clang18.06 | 0                           | 1                                  |

## mock_head

For simplicity, I will use a singly linked list as an example in C language. Firstly, we define a singly linked list node type `struct node`, and implement the `push_front`, the code is as follows:

```c
#include <stddef.h>

struct node {
    int sn;
    struct node *next;
};

static struct node *head = NULL;

void push_front(struct node *n) {
    n->next = head;
    head = n;
}
```

Traditionally, when we want to remove a node from a singly linked list, we need the help of a secondary pointer.

```c
struct node *remove_with_secondary_pointer(struct node *n) {
    struct node **prev = &head;
    for (struct node *cur = *prev; cur != NULL;) {
        if (cur == n) {
            *prev = cur->next;
            return cur;
        }
        prev = &cur->next;
        cur = *prev;
    }
    return NULL;
}
```

Why do we need a secondary pointer? The core reason is that the shapes(type) of the head node and data node are different, and the secondary pointer unifies them. If we define the head as `static struct node head = {0, NULL};`, the secondary pointer will no longer be needed, but there is a problem of memory waste, the member `int sn;` is redundant in the example, we only need the member`struct node next;`.

The solution is to use `container_of` to simulate the data node with the head node, the code is as follows:

```c
#define container_of(_field_, _type_, _member_)                                                    \
    ((_type_ *) ((size_t) (_field_) -offsetof(_type_, _member_)))

#define MOCK_HEAD(_field_, _type_, _member_) container_of(_field_, _type_, _member_)

struct node *remove_with_mock_head(struct node *n) {
    struct node *prev = MOCK_HEAD(&head, struct node, next);
    for (struct node *cur = prev->next; cur != NULL;) {
        if (cur == n) {
            prev->next = cur->next;
            return cur;
        }
        prev = cur;
        cur = prev->next;
    }
    return NULL;
}
```

As you can see, **mock_head** is just a alias of the `contianer_of`, it unifies head node and data node, just like secondary pointer. It should be noted that when accessing head node members, all members except for `struct node *next;` are undefined in the example.

Of course, this is just an example. The implementation of container_of is slightly different in C++, please see the [header](include/uit/intrusive.hpp) in this library.

## Example

```c++
#include <iostream>
#include <uit/islist.hpp>

class apple {
   public:
    apple(uint64_t weight, int sn)
        : weight(weight)
        , sn(sn) {
    }

    uint64_t weight;
    uit::isnode<apple> node;
    int sn;
};

int main(int argc, char *argv[]) {
    uit::islist<&apple::node> list{};

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

## TODO

- Need more tests.