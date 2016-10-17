cmake_minimum_required(VERSION 3.6)
cmake_policy(SET CMP0048 NEW)
project(bgp_samples)


set(SAMPLE_DESCRIPTION "BGP sample application using YANG Development Kit Library, the library for YDK API.")


set(bgp_samples bgp_create; bgp_update;bgp_read;bgp_delete;bgp_routing_policy_create)

# set default build type if not specified by user
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0")

# Find dependent libraries
find_library(ydk_location ydk)
find_library(ydk_ydktest_location ydk_ydktest)
find_library(xml2_location xml2)
find_library(curl_location curl)
find_library(ssh_location ssh)
find_library(ssh_threads_location ssh_threads)
find_library(pcre_location pcre)
find_library(xslt_location xslt)
find_package(Boost COMPONENTS log_setup log thread date_time system filesystem unit_test_framework REQUIRED)
set(BOOST_INCLUDE_DIRS $boost_installation_prefix/include)

include_directories(SYSTEM ${BOOST_INCLUDE_DIR})

# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif()

foreach(sample IN LISTS bgp_samples)
    add_executable(${sample} ${sample}.cpp args_parser.cpp)
    target_compile_definitions(${sample} PRIVATE "BOOST_LOG_DYN_LINK=1")

    target_link_libraries(${sample}
            ${ydk_location} 
            ${ydk_ydktest_location} 
            ${xml2_location}
            ${curl_location}
            ${ssh_location}
            ${ssh_threads_location}
            ${pcre_location}
            ${xslt_location}
            ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
            ${Boost_LOG_SETUP_LIBRARY}
            ${Boost_LOG_LIBRARY}
            ${BOOST_THREAD_LIBRARY}
            ${BOOST_DATE_TIME_LIBRARY}
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            -rdynamic
            )

    set_property(TARGET ${sample} PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${sample} PROPERTY CXX_STANDARD_REQUIRED ON)
endforeach(sample)
