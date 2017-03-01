cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(ydk_bundle_test)
set(YDK_UNITTEST_TARGET_NAME "ydk_bundle_test")

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")
set(bundle_tests_src
        test_core_netconf.cpp
        test_netconf_client.cpp
        test_restconf_client.cpp
        testsanitynctest.cpp
        test_crud.cpp
        test_sanity_levels.cpp
        test_sanity_types.cpp
        testsanityvalidationtest.cpp
        test_netconf_provider.cpp
        test_restconf_provider.cpp
        test_sanity_codec.cpp
        test_netconf_operations.cpp
        test_executor_service.cpp
        test_netconf_service.cpp
        test_errors.cpp
        test_opendaylight.cpp
        main.cpp)

find_library(xml2_location xml2)
find_library(curl_location curl)
find_library(ssh_location ssh)
find_library(ssh_threads_location ssh_threads)
find_library(pcre_location pcre)
find_library(xslt_location xslt)
find_library(pthread_location pthread)
find_library(dl_location dl)
find_library(ydk_location ydk)
find_library(ydk_ydktest_location ydk_ydktest)
find_library(ydk_ydktest_new_location ydk_ydktest_new)

set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -fprofile-arcs -ftest-coverage")


# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE debug)
endif()

include_directories(
    ${PROJECT_SOURCE_DIR}/../core/src
)

add_executable(${YDK_UNITTEST_TARGET_NAME} ${bundle_tests_src})
set_property(TARGET ${YDK_UNITTEST_TARGET_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${YDK_UNITTEST_TARGET_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(${YDK_UNITTEST_TARGET_NAME}
    ${ydk_location}
    ${ydk_ydktest_location}
    ${ydk_ydktest_new_location}
    ${xml2_location}
    ${curl_location}
    ${ssh_location}
    ${ssh_threads_location}
    ${pcre_location}
    ${xslt_location}
    ${pthread_location}
    ${dl_location}
    -rdynamic
)

add_test(NAME ${PROJECT_NAME} COMMAND $<TARGET_FILE:${YDK_UNITTEST_TARGET_NAME}>)

configure_file("${PROJECT_SOURCE_DIR}/main.cpp.in" "${PROJECT_SOURCE_DIR}/main.cpp" ESCAPE_QUOTES @ONLY)
