cmake_minimum_required(VERSION 2.8)
# project(ydk_cpp_test)
# message ("Generating project ${PROJECT_NAME}")

enable_testing()
# include(CheckIncludeFileCXX)

# set(CMAKE_CXX_FLAGS "-Wall -std=c++0x -I./../../")
set(BUILDS_DIR ${CMAKE_SOURCE_DIR}/builds)

find_package(unittest REQUIRED NAMES UnitTest++)
IF(unittest_FOUND)
	message("unittest dir: " ${unittest_DIR})
ELSE(NOT unittest_FOUND)
	message("unittest-cpp NOT FOUND")
	message("  install package from https://github.com/unittest-cpp/unittest-cpp")
ENDIF()

# May be a solution if unittest-cpp was installed with brew (instead of github clone) and cannot be found
# find_library(unittest_location UnitTest++)
find_library(ydk_client_location ydk_client HINT ${BUILDS_DIR}/ydk)
find_library(netconf_location netconf)
find_library(boost_python_location boost_python)
find_library(python_location python2.7)
find_library(ssh_location ssh)
find_library(xml2_location xml2)
find_library(curl_location curl)
find_library(ssh_threads_location ssh_threads)
find_library(xslt_location xslt)
find_library(pthread_location pthread)
find_library(dl_location dl)

include_directories(
	# May be a solution if unittest-cpp was installed with brew (instead of github clone) and cannot be found
	# /usr/local/include/UnitTest++
	${CMAKE_SOURCE_DIR}
	${TESTS_DIR}
	${YDK_DIR}
)
message("ydk dir: " ${YDK_DIR})

add_executable (test_ydk ${TESTS_DIR}/test_netconf_client.cpp)

target_link_libraries(test_ydk
	# May be a solution if unittest-cpp was installed with brew (instead of github clone) and cannot be found
	# ${unittest_location}
	UnitTest++
	${ydk_client_location}
	${netconf_location}
	${boost_python_location}
	${python_location}
	${ssh_location}
	${xml2_location}
	${curl_location}
	${ssh_threads_location}
	${xslt_location}
	${pthread_location}
	${dl_location}
)

target_compile_features(test_ydk PRIVATE cxx_range_for)
add_test(NAME test_netconf_client COMMAND ${TESTS_DIR}/test_netconf_client.cpp)

# install(TARGETS test_ydk DESTINATION .)

# execute_process(COMMAND test_ydk)

# ************************** new stuff ************************** #
# add_executable (Test test_netconf_client.cpp)
# set(TESTS
# 	test_netconf_client)

# foreach(test ${TESTS})
# 	add_executable(test-${test} ${TESTS_DIR}/${test}.cpp)
# 	target_link_libraries(test-${test} ydk_cpp ${LIBRARIES})
# 	target_include_directories(test-${test} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
# 	list(APPEND TEST_TARGETS test-${test})
# endforeach()

# add_test(netconf_client ${TESTS_DIR}/test_netconf_client.cpp)

