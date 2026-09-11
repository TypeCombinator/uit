#include <uit/intrusive.hpp>

namespace uit_instrusive_test {
struct foo {
    int a;
    const int b;
};

static_assert(uit::offset_of(&foo::a) == 0);
static_assert(uit::offset_of(&foo::b) == 4);

} // namespace uit_instrusive_test