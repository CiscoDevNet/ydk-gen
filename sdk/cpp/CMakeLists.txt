cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)
project(ydk VERSION 0.4.2)
message ("Generating project ${PROJECT_NAME} ${PROJECT_VERSION}")

set (YDK_DIR ${CMAKE_SOURCE_DIR}/ydk)
set (TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)

add_subdirectory(ydk)

enable_testing()
add_subdirectory(tests)