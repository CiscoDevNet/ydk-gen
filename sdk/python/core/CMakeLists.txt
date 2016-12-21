cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(path VERSION 0.5.2 LANGUAGES C CXX)
set(CMAKE_MACOSX_RPATH 1)

set(path_DESCRIPTION "Python wrapper around C++ path API using boost-python.")

set(python_src python.cpp)

# set default build type if not specified by user
set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0")

# Find dependent libraries
find_library(ydk_location ydk)
find_library(xml2_location xml2)
find_library(curl_location curl)
find_library(ssh_threads_location ssh_threads)
find_library(xslt_location xslt)
find_library(pcre_location pcre)
find_library(ssh_location ssh)

find_package(PythonLibs REQUIRED)
find_package(Boost REQUIRED)
find_package(Boost COMPONENTS log python system filesystem REQUIRED)

if(NOT ${PYTHONLIBS_FOUND})
    message( FATAL_ERROR "Python library not found." )
else()
    message( "python version: ${PYTHONLIBS_VERSION_STRING}" )
    message( "library: ${PYTHON_LIBRARIES}" )
    message( "include: ${PYTHON_INCLUDE_DIRS}" )
endif()

set(BOOST_INCLUDE_DIRS ${boost_installation_prefix}/include)

include_directories(SYSTEM ${BOOST_INCLUDE_DIR} ${PYTHON_INCLUDE_DIRS})

# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif()

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BOOST_ALL_DYN_LINK           ON)   # force dynamic linking for all libraries
set(Boost_NO_BOOST_CMAKE         ON)

PYTHON_ADD_MODULE(path MODULE ${python_src})

target_compile_definitions(path PRIVATE "BOOST_ALL_DYN_LINK=1")
target_compile_definitions(path PRIVATE "BOOST_LOG_DYN_LINK=1")

set_property(TARGET path PROPERTY CXX_STANDARD 14)
set_property(TARGET path PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(path
            ${ydk_location}
            ${xml2_location}
            ${curl_location}
            ${ssh_threads_location}
            ${pcre_location}
            ${xslt_location}
            ${ssh_location}
            ${PYTHON_LIBRARIES}
            ${Boost_LIBRARIES}            
            -rdynamic
            )
