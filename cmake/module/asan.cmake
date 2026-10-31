# AddressSanitizer options for the custom ASan build configuration.

if(MSVC) # For cl.exe or clang-cl.exe.
    # Reference: https://learn.microsoft.com/en-us/cpp/build/reference/oy-frame-pointer-omission?view=msvc-170
    set(UIT_SANITIZER_COMPILE_OPTIONS -fsanitize=address /Oy-)
    set(UIT_SANITIZER_LINK_OPTIONS /INCREMENTAL:NO)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        execute_process(
            COMMAND "${CMAKE_CXX_COMPILER}" /clang:-print-resource-dir
            OUTPUT_VARIABLE UIT_CLANG_CL_RESOURCE_DIR
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(UIT_CLANG_CL_RT_DIR "${UIT_CLANG_CL_RESOURCE_DIR}/lib/windows")
        list(APPEND UIT_SANITIZER_LINK_OPTIONS
            "/libpath:${UIT_CLANG_CL_RT_DIR}"
            "clang_rt.asan_dynamic-x86_64.lib"
            "/wholearchive:clang_rt.asan_dynamic_runtime_thunk-x86_64.lib"
        )
    endif()
elseif(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "Clang") # For clang.exe.
    set(UIT_SANITIZER_COMPILE_OPTIONS -fsanitize=address -fno-omit-frame-pointer)
    set(UIT_SANITIZER_LINK_OPTIONS -fsanitize=address -static-libsan)
else() # For gcc or clang.
    set(UIT_SANITIZER_COMPILE_OPTIONS -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
    set(UIT_SANITIZER_LINK_OPTIONS -fsanitize=address,undefined)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        # GCC's -Wmaybe-uninitialized false-positives inside libstdc++ <regex>
        # once ASan instrumentation changes inlining decisions; google
        # benchmark builds with -Werror, so silence the bogus warning.
        list(APPEND UIT_SANITIZER_COMPILE_OPTIONS -Wno-maybe-uninitialized)
    endif()
endif()