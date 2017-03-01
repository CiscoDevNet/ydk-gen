cmake_minimum_required(VERSION 2.8.9)
cmake_policy(SET CMP0048 NEW)
project(ydk_@BRIEF_NAME@ VERSION @VERSION@ LANGUAGES C CXX)

# include custom Modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

set(YDK_YANG_ARCHIVE @BRIEF_NAME@\@@VERSION@)

if(NOT LIB_INSTALL_DIR)
	set(LIB_INSTALL_DIR lib)
endif()

if(NOT INCLUDE_INSTALL_DIR)
	set(INCLUDE_INSTALL_DIR include/ydk_@BRIEF_NAME@)
endif()

if(NOT YANG_INSTALL_DIR)
    set(YANG_INSTALL_DIR share/@BRIEF_NAME@\@@VERSION@)
endif()

# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE release)
endif()

if(SHOW_WARNINGS)
  set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
else(SHOW_WARNINGS)
  set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -w")
endif(SHOW_WARNINGS)

set(CMAKE_CXX_FLAGS_RELEASE "-Os -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0")


set(libydk_@BRIEF_NAME@_src ydk/models/generated_entity_lookup.cpp @SOURCE_FILES@)

set(libydk_@BRIEF_NAME@_headers ydk/models/bundle_info.hpp @HEADER_FILES@)

set(libydk_@BRIEF_NAME@_yang @YANG_FILES@)

set (libydk_yang_dir ${CMAKE_CURRENT_BINARY_DIR}/@BRIEF_NAME@\@@VERSION@)

list(APPEND libydk_YANG_FILES "")
foreach(yang_file IN LISTS libydk_@BRIEF_NAME@_yang)
    list(APPEND libydk_YANG_FILES ${libydk_yang_dir}/${yang_file})
endforeach(yang_file)

file(MAKE_DIRECTORY ${libydk_yang_dir})

add_custom_command(OUTPUT ${libydk_YANG_FILES}  
  COMMAND ${CMAKE_COMMAND} -E tar xzf "${CMAKE_SOURCE_DIR}/${YDK_YANG_ARCHIVE}.tar.gz"
  #COMMAND ${CMAKE_COMMAND} -E touch ${libydk_YANG_FILES}
  WORKING_DIRECTORY ${libydk_yang_dir}
  DEPENDS ${CMAKE_SOURCE_DIR}/${YDK_YANG_ARCHIVE}.tar.gz
  COMMENT "Unpacking ${YDK_YANG_ARCHIVE}.tar.gz"
  VERBATIM
)

add_custom_target(libydk_yang_untar ALL DEPENDS ${libydk_YANG_FILES})

find_package(LibYDK REQUIRED)

include_directories(
    ${LIBYDK_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/ydk/models
)

configure_file("${CMAKE_SOURCE_DIR}/bundle_info.hpp.in" "${CMAKE_SOURCE_DIR}/ydk/models/bundle_info.hpp" ESCAPE_QUOTES @ONLY)

add_library(ydk_@BRIEF_NAME@ STATIC ${libydk_@BRIEF_NAME@_src})

target_link_libraries(ydk_@BRIEF_NAME@
    ${LIBYDK_LIBRARIES}
)

set_property(TARGET ydk_@BRIEF_NAME@ PROPERTY CXX_STANDARD 11)
set_property(TARGET ydk_@BRIEF_NAME@ PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS ydk_@BRIEF_NAME@ DESTINATION ${LIB_INSTALL_DIR})
install(FILES ${libydk_@BRIEF_NAME@_headers} DESTINATION ${INCLUDE_INSTALL_DIR})
install(FILES ${libydk_YANG_FILES} DESTINATION ${YANG_INSTALL_DIR})


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


