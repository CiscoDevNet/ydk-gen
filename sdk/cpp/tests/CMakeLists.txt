cmake_minimum_required(VERSION 2.8)

project(ydk_cpp_test)

set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -I./../../ -L./../unittest-cpp/builds -lunittest++ -L./../../ -lydk_client -lnetconf")

add_executable(test_ydk test_netconf_client.cpp)

install(TARGETS test_ydk DESTINATION .)

execute_process(COMMAND test_ydk)