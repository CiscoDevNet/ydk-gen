cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(ydk_entity_test)

enable_testing()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")
set(netconf_tests
        test_core_netconf
        test_netconf_client
        test_restconf_client
        testsanitynctest
        test_crud
        test_sanity_levels
        test_sanity_types
        testsanityvalidationtest
        test_netconf_provider
        test_restconf_provider
        test_sanity_codec
        test_netconf_operations
        test_executor_service
        test_netconf_service
        test_errors
        test_opendaylight)

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
find_package(Boost REQUIRED)
find_package(Boost COMPONENTS log_setup log thread date_time system filesystem unit_test_framework REQUIRED)

set(BOOST_INCLUDE_DIRS $boost_installation_prefix/include)

include_directories(SYSTEM ${BOOST_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -fprofile-arcs -ftest-coverage")


# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE debug)
endif()

foreach(test_name IN LISTS netconf_tests)
    add_executable(${test_name} ${test_name}.cpp)
    set_property(TARGET ${test_name} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${test_name} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_include_directories(${test_name} PRIVATE ${BOOST_INCLUDE_DIRS} /opt/local/include /usr/local/include)
    target_compile_definitions(${test_name} PRIVATE "BOOST_TEST_DYN_LINK=1")
    target_compile_definitions(${test_name} PRIVATE "BOOST_LOG_DYN_LINK=1")
    target_link_libraries(${test_name}
        ${ydk_location}
        ${ydk_ydktest_location}
        ${ydk_location}
        ${xml2_location}
        ${curl_location}
        ${ssh_location}
        ${ssh_threads_location}
        ${pcre_location}
        ${xslt_location}
        ${pthread_location}
        ${dl_location}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        ${Boost_LOG_SETUP_LIBRARY}
        ${Boost_LOG_LIBRARY}
        ${BOOST_THREAD_LIBRARY}
        ${BOOST_DATE_TIME_LIBRARY}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        -rdynamic
    )


    add_test(NAME ${test_name} COMMAND $<TARGET_FILE:${test_name}>)
endforeach(test_name)

configure_file("${PROJECT_SOURCE_DIR}/config.hpp.in" "${PROJECT_SOURCE_DIR}/config.hpp" ESCAPE_QUOTES @ONLY)
