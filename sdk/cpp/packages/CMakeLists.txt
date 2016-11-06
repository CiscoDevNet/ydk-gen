cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(ydk_@BRIEF_NAME@ VERSION 0.5.1)



set(YDK_@BRIEF_NAME_CAP@_DESCRIPTION "openconfig ydk bundle")

# set version
set(YDK_@BRIEF_NAME_CAP@_MAJOR_VERSION 0)
set(YDK_@BRIEF_NAME_CAP@_MINOR_VERSION 5)
set(YDK_@BRIEF_NAME_CAP@_SERVICE_VERSION 1)
set(YDK_@BRIEF_NAME_CAP@_VERSION ${YDK_@BRIEF_NAME_CAP@_MAJOR_VERSION}.${YDK_@BRIEF_NAME_CAP@_MINOR_VERSION}.${YDK_@BRIEF_NAME_CAP@_SERVICE_VERSION})
set(YDK_@BRIEF_NAME_CAP@_CORE_SOVERSION ${YDK_@BRIEF_NAME_CAP@_MAJOR_VERSION}.${YDK_@BRIEF_NAME_CAP@_MINOR_VERSION})

# include custom Modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

if(NOT LIB_INSTALL_DIR)
	set(LIB_INSTALL_DIR lib)
endif()

if(NOT INCLUDE_INSTALL_DIR)
	set(INCLUDE_INSTALL_DIR include/ydk_@BRIEF_NAME@)
endif()

# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE release)
endif()

set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0")


set(libydk_@BRIEF_NAME@_src ydk/models/entity_lookup.cpp @SOURCE_FILES@)
set(libydk_@BRIEF_NAME@_headers @HEADER_FILES@)

find_package(LibYDK REQUIRED)

include_directories(
	${Boost_INCLUDE_DIRS}
    ${LIBYDK_INCLUDE_DIRS}
)

add_library(ydk_@BRIEF_NAME@ STATIC ${libydk_@BRIEF_NAME@_src})

target_link_libraries(ydk_@BRIEF_NAME@
    ${LIBYDK_LIBRARIES}
)

set_property(TARGET ydk_@BRIEF_NAME@ PROPERTY CXX_STANDARD 14)
set_property(TARGET ydk_@BRIEF_NAME@ PROPERTY CXX_STANDARD_REQUIRED ON)

# find YDK library

install(TARGETS ydk_@BRIEF_NAME@ DESTINATION ${LIB_INSTALL_DIR})
install(FILES ${libydk_@BRIEF_NAME@_headers} DESTINATION ${INCLUDE_INSTALL_DIR})


# generate doxygen documentation for ydk_core API
find_package(Doxygen)
if(DOXYGEN_FOUND)
	set(DOXYGEN_SKIP_DOT TRUE)
	add_custom_target(doc
			COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
	configure_file(Doxyfile.in Doxyfile)
endif()


# clean cmake cache
add_custom_target(cclean
		COMMAND make clean
		COMMAND find . -iname '*cmake*' -not -name CMakeLists.txt -not -path './CMakeModules*' -exec rm -rf {} +
		COMMAND rm -rf Makefile Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


