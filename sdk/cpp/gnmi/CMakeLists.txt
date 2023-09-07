cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0048 NEW)
project(ydk_gnmi VERSION 0.4.0 LANGUAGES C CXX)

set(YDK_DESCRIPTION "YANG Development Kit Library. The gNMI library for YDK API.")

# CPack Dynamic Settings
set (CPACK_PACKAGE_NAME "libydk_gnmi")
set (CPACK_PACKAGE_RELEASE "5")
set (CPACK_PACKAGE_VERSION "${PROJECT_VERSION}.${CPACK_PACKAGE_RELEASE}")
set (CPACK_LIBRARY_NAME "ydk_gnmi-${CPACK_PACKAGE_VERSION}")
set (CPACK_LIBRARY_TARGET "lib${CPACK_LIBRARY_NAME}.a")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "YDK gNMI service Library")

# CPack Static Settings
set (CPACK_PACKAGE_DESCRIPTION "YDK library to support gNMI services.
It must be installed on top of core YDK package.")

set (CPACK_PACKAGE_CONTACT "ydk-admin@cisco.com")
set (CPACK_PACKAGE_VENDOR "Cisco")
set (CPACK_SET_DESTDIR true)

# MAC OS -- not working
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message("CMake System: MacOS")

elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_program(LSB_RELEASE lsb_release)
    execute_process(COMMAND ${LSB_RELEASE} -is
        OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Linux Debian
    if (${LSB_RELEASE_ID_SHORT} MATCHES "Ubuntu")
        message("CMake System: Ubuntu")
        set (CPACK_GENERATOR "DEB")

        set (CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
        set (CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
        set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
        set (CPACK_DEBIAN_PACKAGE_DEPENDS
            "libcurl4-openssl-dev,
            libpcre3-dev,
            libssh-dev,
            libxml2-dev,
            libxslt1-dev,
            libtool-bin,
            cmake (>=3),
            git,
            pkg-config")
        set (CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})
        set (CPACK_DEBIAN_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
        set (CPACK_DEBIAN_PACKAGE_SECTION "devel")
        set (CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
        set (CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/CiscoDevNet/ydk-cpp")

        # <foo>_<VersionNumber>-<DebianRevisionNumber>_<DebianArchitecture>.deb
        # libydk_0.6.0-0.1.alpha_amd64.deb   // Debian
        set (CPACK_PACKAGE_FILE_NAME
            "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")

    # Linux Fedora
    elseif(${LSB_RELEASE_ID_SHORT} MATCHES "CentOS")
        message("CMake System: CentOS")

        set (CPACK_GENERATOR "RPM")

        set (CPACK_RPM_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
        set (CPACK_RPM_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
        set (CPACK_RPM_PACKAGE_RELEASE ${CPACK_PACKAGE_RELEASE})
        set (CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
        set (CPACK_RPM_PACKAGE_SUMMARY ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
        set (CPACK_RPM_PACKAGE_DESCRIPTION ${CPACK_PACKAGE_DESCRIPTION})

        set (CPACK_RPM_PACKAGE_LICENSE "Apache 2.0")
        set (CPACK_RPM_PACKAGE_GROUP "Development/Libraries")
        set (CPACK_RPM_PACKAGE_VENDOR ${CPACK_PACKAGE_VENDOR})
        set (CPACK_RPM_PACKAGE_URL "https://github.com/CiscoDevNet/ydk-cpp")
        set (CPACK_RPM_PACKAGE_REQUIRES "python-pip, python-devel, libxml2-devel, libxslt-devel, libssh-devel, libcurl-devel, libtool, cmake3, pcre-devel")

        # <name>-<version>-<release>.<architecture>.(src).rpm
        # libydk-0.6.0-0.1.alpha.x86_64.rpm  // RPM
        set (CPACK_PACKAGE_FILE_NAME
            "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.${CPACK_RPM_PACKAGE_ARCHITECTURE}")
    endif()
endif()

include(CPack)
#include(FindProtobuf)

# include custom Modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

if(NOT LIB_INSTALL_DIR)
    set(LIB_INSTALL_DIR lib)
endif()

if(NOT INCLUDE_INSTALL_DIR)
    set(INCLUDE_INSTALL_DIR include/ydk)
endif()

# set default build type if not specified by user
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif()

set(HOME_DIRECTORY $ENV{HOME})

set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

if(COVERAGE STREQUAL True)
    set(CMAKE_CXX_FLAGS_DEBUG   "-g3 -O0 -fprofile-arcs -ftest-coverage")
    set(CMAKE_BUILD_TYPE debug)
else()
    set(CMAKE_CXX_FLAGS_DEBUG   "-g3 -O0")
endif()

if(CMAKE_BUILD_TYPE STREQUAL debug)
    option(ENABLE_BUILD_TESTS "Build tests" ON)
    option(ENABLE_VALGRIND_TESTS "Build tests with valgrind" ON)
else()
    option(ENABLE_BUILD_TESTS "Build tests" OFF)
    option(ENABLE_VALGRIND_TESTS "Build tests with valgrind" OFF)
endif()

set(pb_grpc_sources
    ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated/gnmi.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated/gnmi.pb.cc)

# Set LibYDK variables
set(libydk_gnmi_src
    src/gnmi_session.cpp
    src/gnmi_client.cpp
    src/gnmi_provider.cpp
    src/gnmi_service.cpp
    src/gnmi_util.cpp
    src/ydk_gnmi.cpp
    )

set(libydk_gnmi_install_headers
    src/gnmi_provider.hpp
    src/gnmi_client.hpp
    src/gnmi_service.hpp
    src/gnmi_util.hpp
    src/gnmi_path_api.hpp
    src/ydk_gnmi.h
    )

# Set gNMI variables
set(pb_grpc_headers
    ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated/gnmi.grpc.pb.h
    ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated/gnmi.pb.h)

set(protobuf_MODULE_COMPATIBLE TRUE)

# Find dependent libraries
find_library(curl_location curl)
find_library(ssh_threads_location ssh_threads)
find_library(xslt_location xslt)
find_library(pcre_location pcre)
find_library(pthread_location pthread)
find_library(dl_location dl)
find_library(m_location m)
find_package(LibXml2 REQUIRED)
find_package(LibSSH REQUIRED)
find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

# Add LibgNMI
set(libgnmi_headers_location ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated)

include(ExternalProject)
ExternalProject_Add(
    download_generate_proto_cpp
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi"
    DOWNLOAD_DIR "${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi"
    DOWNLOAD_NAME "gnmi.proto"
    DOWNLOAD_COMMAND curl https://raw.githubusercontent.com/openconfig/gnmi/8a14ac0e9ed67e08988f9913243d89f398454824/proto/gnmi/gnmi.proto > gnmi.proto
    CONFIGURE_COMMAND ""
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi"
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi"
    BUILD_COMMAND mkdir -p generated && ${PROTOBUF_PROTOC_EXECUTABLE} --cpp_out generated/ gnmi.proto && ${PROTOBUF_PROTOC_EXECUTABLE} --grpc_out generated/ --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} gnmi.proto
    INSTALL_COMMAND ""
)

add_custom_command(OUTPUT ${pb_grpc_sources}
  DEPENDS download_generate_proto_cpp
  COMMENT "Generating GNMI code"
  VERBATIM
)

include_directories(
    ${LIBSSH_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${Protobuf_INCLUDE_DIR}
    ${libgnmi_headers_location}
    ${PROJECT_SOURCE_DIR}/src
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${CPACK_LIBRARY_NAME} STATIC ${libydk_gnmi_src} ${pb_grpc_sources})

target_link_libraries(
    ${CPACK_LIBRARY_NAME}
    ${GRPC_GRPC++_LIBRARY}
    ${PROTOBUF_LIBRARY}
)

set_property(TARGET ${CPACK_LIBRARY_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${CPACK_LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

install(TARGETS ${CPACK_LIBRARY_NAME} DESTINATION ${LIB_INSTALL_DIR})
install(FILES ${libydk_gnmi_install_headers} DESTINATION ${INCLUDE_INSTALL_DIR})
install(FILES ${pb_grpc_headers} DESTINATION ${INCLUDE_INSTALL_DIR})

SET( legacy_link   ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libydk_gnmi.a)
SET( legacy_target ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/${CPACK_LIBRARY_TARGET})
ADD_CUSTOM_COMMAND( TARGET ${CPACK_LIBRARY_NAME}
                    COMMAND echo "Creating symbolic link ${legacy_link}"
                    COMMAND sudo rm -f ${legacy_link}
                    COMMAND sudo ln -s ${CPACK_LIBRARY_TARGET} ${legacy_link}
                    DEPENDS install ${CPACK_LIBRARY_NAME}
                  )
ADD_CUSTOM_TARGET( install_legacy DEPENDS ${legacy_target} )

# generate doxygen documentation for ydk_gnmi API
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
