
cmake_minimum_required(VERSION 3.16)
project(cpplox)

set(CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include (FetchContent)

enable_testing()
conan_basic_setup()

file(GLOB_RECURSE sources "src/**.cpp")
file(GLOB_RECURSE test_sources "src/**.cpp")

list(FILTER test_sources EXCLUDE REGEX ".*main.cpp$")
file(GLOB_RECURSE test_cases "test/**.cpp")

add_library(cpplox_deps INTERFACE)
target_link_libraries(cpplox_deps INTERFACE
    ${CONAN_LIBS}
)

target_compile_options(cpplox_deps INTERFACE
    -Werror=return-type
    -Werror=unused
    -Werror=pessimizing-move
    -Werror=redundant-move
)

set_target_properties(cpplox_deps PROPERTIES
    INTERFACE CXX_STANDARD 20
    INTERFACE CXX_STANDARD_REQUIRED True
)

target_include_directories(cpplox_deps
    INTERFACE include
    INTERFACE external
)

add_executable(cpplox
    ${sources}
)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/external/")

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(cpplox_test
    ${test_cases} ${test_sources}
)

target_link_libraries(cpplox cpplox_deps)
target_link_libraries(cpplox_test cpplox_deps gtest_main)
add_test(NAME the_tests COMMAND tests)

