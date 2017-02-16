cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(ydk_test)
message ("Generating project ${PROJECT_NAME}")

enable_testing()
set(ROOT ${CMAKE_SOURCE_DIR}) 
set(YDK_DIR ${CMAKE_SOURCE_DIR}/ydk)
set(TESTS_DIR ${CMAKE_SOURCE_DIR}/tests)
set(BUILDS_DIR ${CMAKE_SOURCE_DIR}/builds)

find_package(unittest REQUIRED NAMES UnitTest++)
IF(unittest_FOUND)
	message("found unittest dir: " ${unittest_DIR})
ELSE(NOT unittest_FOUND)
	message("unittest-cpp NOT FOUND")
	message("  install package from https://github.com/unittest-cpp/unittest-cpp")
ENDIF()

# May be a solution if unittest-cpp was installed with brew (instead of github clone) and cannot be found
# find_library(unittest_location UnitTest++)
find_library(ydk_client_location ydk_client HINT ${BUILDS_DIR}/ydk)
find_library(netconf_location netconf)
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
	${ROOT}
	${TESTS_DIR}
	${YDK_DIR}
)

add_executable (test_netconf_client ${TESTS_DIR}/test_netconf_client.cpp)

target_link_libraries(test_netconf_client
	# May be a solution if unittest-cpp was installed with brew (instead of github clone) and cannot be found
	# ${unittest_location}
	UnitTest++
	${ydk_client_location}
	${netconf_location}
	${python_location}
	${ssh_location}
	${xml2_location}
	${curl_location}
	${ssh_threads_location}
	${xslt_location}
	${pthread_location}
	${dl_location}
)

target_compile_features(test_netconf_client PRIVATE cxx_range_for)
add_test(NAME netconf_client COMMAND test_netconf_client)
