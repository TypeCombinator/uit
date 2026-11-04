// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
// SPDX-License-Identifier: MPL-2.0
//
// This project is licensed under the Mozilla Public License 2.0.
// Please see the accompanying LICENSE file in this project root
// or https://spdx.org/licenses/MPL-2.0.html for details.

#include <uit/intrusive.hpp>

namespace uit_instrusive_test {
struct foo {
    int a;
    const int b;
};

static_assert(uit::offset_of(&foo::a) == 0);
static_assert(uit::offset_of(&foo::b) == 4);

} // namespace uit_instrusive_test