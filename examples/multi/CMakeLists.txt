# SPDX-FileCopyrightText: 2026 TypeCombinator <typecombinator@foxmail.com>
# SPDX-License-Identifier: MPL-2.0
#
# This project is licensed under the Mozilla Public License 2.0.
# Please see the accompanying LICENSE file in this project root
# or https://spdx.org/licenses/MPL-2.0.html for details.

get_filename_component(CURRENT_TARGET ${CMAKE_CURRENT_SOURCE_DIR} NAME)

add_executable(${CURRENT_TARGET}
  main.cpp
)

target_link_libraries(${CURRENT_TARGET}
  PRIVATE
  uit
)