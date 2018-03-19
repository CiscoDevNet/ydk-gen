cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0048 NEW)
set(YDK_UNITTEST_TARGET_NAME "ydk_core_test")

set(core_tests_src bgptest.cpp
               core_test.cpp
               test_codec.cpp
               test_entity.cpp
               test_value.cpp
               test_value_list.cpp
               test_capabilities_parser.cpp
               main.cpp)

set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
if(COVERAGE STREQUAL True)
    set(CMAKE_CXX_FLAGS_DEBUG   "-g3 -O0 -fprofile-arcs -ftest-coverage")
    set(CMAKE_BUILD_TYPE debug)
else()
    set(CMAKE_CXX_FLAGS_DEBUG   "-g3 -O0")
endif()

foreach(test_name IN LISTS core_tests)
    add_executable(${test_name} ${test_name}.cpp)
    set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${test_name} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_link_libraries(${test_name} ydk
        curl
        xslt
        pcre
        ssh_threads
        xml2
        ssh
        dl)
    
    add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
endforeach(test_name)

add_executable(${YDK_UNITTEST_TARGET_NAME} ${core_tests_src})
set_property(TARGET ${YDK_UNITTEST_TARGET_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${YDK_UNITTEST_TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(${YDK_UNITTEST_TARGET_NAME} ydk)
add_test(NAME ${YDK_UNITTEST_TARGET_NAME} COMMAND $<TARGET_FILE:${YDK_UNITTEST_TARGET_NAME}>)

configure_file("${PROJECT_SOURCE_DIR}/tests/main.cpp.in" "${PROJECT_SOURCE_DIR}/tests/main.cpp" ESCAPE_QUOTES @ONLY)
