get_filename_component(CURRENT_TARGET ${CMAKE_CURRENT_SOURCE_DIR} NAME)

add_executable(${CURRENT_TARGET}
  main.cpp
)

target_link_libraries(${CURRENT_TARGET}
  PRIVATE
  uit
)