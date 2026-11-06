// SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
// SPDX-License-Identifier: MPL-2.0
//
// This project is licensed under the Mozilla Public License 2.0.
// Please see the accompanying LICENSE file in this project root
// or https://spdx.org/licenses/MPL-2.0.html for details.

#ifndef ATTRIBUTES_B4B11AB1_414B_49BC_BCA2_C9A8AE148820
#define ATTRIBUTES_B4B11AB1_414B_49BC_BCA2_C9A8AE148820

#if defined(__has_cpp_attribute) && __has_cpp_attribute(msvc::no_unique_address)
#    define UIT_ATTR_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#    define UIT_ATTR_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif

#endif /* attributes.hpp */