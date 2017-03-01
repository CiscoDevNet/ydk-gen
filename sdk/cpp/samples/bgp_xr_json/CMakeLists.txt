cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(samples)


set(SAMPLE_DESCRIPTION "BGP sample application using YANG Development Kit Library, the library for YDK API.")


set(samples bgp_xr_read
            bgp_xr_write)

# set default build type if not specified by user
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -fprofile-arcs -ftest-coverage")

# Find dependent libraries
find_library(ydk_location ydk)
find_library(ydk_cisco_ios_xr_location ydk_cisco_ios_xr)
find_library(xml2_location xml2)
find_library(curl_location curl)
find_library(ssh_location ssh)
find_library(ssh_threads_location ssh_threads)
find_library(pcre_location pcre)
find_library(xslt_location xslt)
find_library(pthread_location pthread)
find_library(dl_location dl)

include_directories(SYSTEM)

# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

foreach(sample IN LISTS samples)
    add_executable(${sample} ${sample}.cpp ../args_parser.cpp)

    target_link_libraries(${sample}
            ${ydk_location} 
            ${ydk_cisco_ios_xr_location} 
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

    set_property(TARGET ${sample} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${sample} PROPERTY CXX_STANDARD_REQUIRED ON)
endforeach(sample)

