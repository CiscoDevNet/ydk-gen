cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0048 NEW)
project(ydk_bundle_test)
set(YDK_UNITTEST_TARGET_NAME "ydk_bundle_test")

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")
set(bundle_tests_src
        main.cpp
        test_c_api.cpp
        test_crud.cpp
        test_errors.cpp
        test_executor_service.cpp
        test_netconf_operations.cpp
        test_netconf_provider.cpp
        test_netconf_service.cpp
        test_netconf_ssh_client.cpp
        test_netconf_tcp_client.cpp
        test_on_demand_loading.cpp
        test_opendaylight.cpp
        test_path_api.cpp
        test_restconf_client.cpp
        test_restconf_provider.cpp
        test_sanity_codec.cpp
        test_sanity_levels.cpp
        test_sanity_types.cpp
        test_xml_subtree.cpp
        testsanitynctest.cpp
        testsanityvalidationtest.cpp
        )

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
if(COVERAGE STREQUAL True)
    set(CMAKE_CXX_FLAGS_DEBUG   "-g3 -O0 -fprofile-arcs -ftest-coverage")
    set(CMAKE_BUILD_TYPE debug)
else()
    set(CMAKE_CXX_FLAGS_DEBUG   "-g3 -O0")
endif()


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
