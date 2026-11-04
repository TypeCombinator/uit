# SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
# SPDX-License-Identifier: MPL-2.0
#
# This project is licensed under the Mozilla Public License 2.0.
# Please see the accompanying LICENSE file in this project root
# or https://spdx.org/licenses/MPL-2.0.html for details.

add_executable(uit_experiment_tests
  islist.cpp
  isdlist.cpp
  idslist.cpp
  idlist.cpp
)
target_include_directories(uit_experiment_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(uit_experiment_tests PRIVATE
  uit
  gtest_main
  # fmt::fmt
)