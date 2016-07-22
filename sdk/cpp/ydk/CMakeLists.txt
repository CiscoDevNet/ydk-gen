cmake_minimum_required(VERSION 3.6)
cmake_policy(SET CMP0048 NEW)
project(ydk_cpp VERSION 0.4.2)
message ("Generating project ${PROJECT_NAME} ${PROJECT_VERSION}")

set (CMAKE_CXX_FLAGS "-Wall -std=c++0x")
set (YDK_DIR ${CMAKE_SOURCE_DIR}/ydk)
set (src_files ${YDK_DIR}/netconf_client.h ${YDK_DIR}/netconf_client.cpp)

find_package(Boost COMPONENTS python REQUIRED)
find_package(PythonLibs 2.7 REQUIRED)

find_library(netconf_location netconf)
find_library(python_location python2.7)
find_library(boost_python_location boost_python)
find_library(xml2_location xml2)
find_library(curl_location curl)
find_library(ssh_location ssh)
find_library(ssh_threads_location ssh_threads)
find_library(xslt_location xslt)

add_library(ydk_client STATIC ${src_files})

include_directories(
	${Boost_INCLUDE_DIRS}
	${PYTHON_INCLUDE_DIRS}
)

target_link_libraries(ydk_client 
	${Boost_LIBRARIES}
	${PYTHON_LIBRARIES}
	${netconf_location}
	${python_location}
	${boost_python_location}
	${xml2_location}
	${curl_location}
	${ssh_location}
	${ssh_threads_location}
	${xslt_location}
)

install(TARGETS ydk_client DESTINATION .)
