cmake_minimum_required(VERSION 2.8)

project(ydk_cpp)

set(CMAKE_CXX_FLAGS "-Wall -std=c++0x")

add_executable(netconf_client ydk/netconf_client.cpp)

install(TARGETS netconf_client DESTINATION .)