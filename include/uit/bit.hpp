// SPDX-FileCopyrightText: 2025 TypeCombinator <typecombinator@foxmail.com>
//
// SPDX-License-Identifier: BSD 3-Clause

#ifndef UIT_BIT_3DCA1F74_7932_477A_ABD1_274DBDD21126
#define UIT_BIT_3DCA1F74_7932_477A_ABD1_274DBDD21126

namespace uit {
// TODO: Support msvc and others
template <typename T, bool enable_ub = false>
constexpr int countl_zero(T x) noexcept {
    if constexpr (!enable_ub) {
        if (x == 0) { // __builtin_clz(0) is UB!
            return sizeof(x) * 8;
        }
    }
    if constexpr (sizeof(x) < sizeof(unsigned)) {
        constexpr auto offset = (sizeof(unsigned) - sizeof(x)) * 8;
        return __builtin_clz(x) - offset;
    } else if constexpr (sizeof(x) == sizeof(unsigned)) {
        return __builtin_clz(x);
    } else if constexpr (sizeof(x) == sizeof(unsigned long)) {
        return __builtin_clzl(x);
    } else if constexpr (sizeof(x) == sizeof(unsigned long long)) {
        return __builtin_clzll(x);
    } else {
        static_assert(sizeof(x) <= sizeof(unsigned long long));
        return -1;
    }
}
} // namespace uit
#endif // bit.hpp