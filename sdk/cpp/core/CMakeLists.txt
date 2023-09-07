cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0048 NEW)
project(ydk VERSION 0.8.6 LANGUAGES C CXX)

set(YDK_DESCRIPTION "YANG Development Kit Library. The library for  YDK API.")

# CPack Dynamic Settings
set (CPACK_PACKAGE_NAME "libydk")
set (CPACK_PACKAGE_BUILD "5")
set (CPACK_PACKAGE_VERSION "${PROJECT_VERSION}.${CPACK_PACKAGE_BUILD}")
set (CPACK_LIBRARY_NAME "ydk-${CPACK_PACKAGE_VERSION}")
set (CPACK_LIBRARY_TARGET "lib${CPACK_LIBRARY_NAME}.a")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "YDK C++ Core Library")

# CPack Static Settings
set (CPACK_PACKAGE_DESCRIPTION "An SDK that provides API's that are modeled in YANG.
The main goal of YDK is to reduce the learning curve of YANG data
models by expressing the model semantics in an API and abstracting
protocol/encoding details. YDK is composed of a core package that
defines services and providers, plus one or more module bundles that
are based on YANG models. Each module bundle is generated using a
bundle profile and the ydk-gen tool.")

set (CPACK_PACKAGE_CONTACT "ydk-admin@cisco.com")
set (CPACK_PACKAGE_VENDOR "Cisco")
set (CPACK_SET_DESTDIR true)

# MAC OS -- not working
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message("CMake System: MacOS")
    # set(CPACK_GENERATOR "productbuild")
    # set(CPACK_COMMAND_PKGBUILD "/usr/bin/pkgbuild")

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
        set (CPACK_RPM_PACKAGE_RELEASE ${CPACK_PACKAGE_BUILD})
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

# Set LibYDK variables
set(libydk_src
    src/bits.cpp
    src/codec_provider.cpp
    src/codec_service.cpp
    src/capabilities_parser.cpp
    src/common_utilities.cpp
    src/crud_service.cpp
    src/entity.cpp
    src/entity_data_node_walker.cpp
    src/entity_lookup.cpp
    src/entity_util.cpp
    src/errors.cpp
    src/executor_service.cpp
    src/ietf_parser.cpp
    src/leaf_data.cpp
    src/logging_callback.cpp
    src/service_provider.cpp
    src/netconf_client.cpp
    src/netconf_ssh_client.cpp
    src/netconf_tcp_client.cpp
    src/netconf_provider.cpp
    src/netconf_service.cpp
    src/netconf_session.cpp
    src/network_topology.cpp
    src/opendaylight_parser.cpp
    src/opendaylight_provider.cpp
    src/restconf_client.cpp
    src/restconf_provider.cpp
    src/restconf_session.cpp
    src/validation_service.cpp
    src/value.cpp
    src/value_list.cpp
    src/json_subtree_codec.cpp
    src/xml_subtree_codec.cpp
    src/xml_util.cpp
    src/ydk_operation.cpp
    src/ydk.cpp
    src/ydk_yang.cpp
    src/path/annotation.cpp
    src/path/capability.cpp
    src/path/data_node.cpp
    src/netconf_model_provider.cpp
    src/path/path.cpp
    src/path/repository.cpp
    src/path/root_data_node.cpp
    src/path/root_schema_node.cpp
    src/path/rpc.cpp
    src/path/schema_node.cpp
    src/path/statement.cpp)


file(GLOB SPDLOG_HEADERS src/spdlog/*.h)
file(GLOB SPDLOG_DETAILS_HEADERS src/spdlog/details/*.h)
file(GLOB SPDLOG_FMT_HEADERS src/spdlog/fmt/*.h)
file(GLOB SPDLOG_FMT_BUNDLED_HEADERS src/spdlog/fmt/bundled/*.h src/spdlog/fmt/bundled/*.cc)
file(GLOB SPDLOG_SINKS_HEADERS src/spdlog/sinks/*.h)

set(spdlog_include_location include/spdlog)
set(spdlog_details_include_location include/spdlog/details)
set(spdlog_fmt_include_location include/spdlog/fmt)
set(spdlog_fmt_bundled_include_location include/spdlog/fmt/bundled)
set(spdlog_sinks_include_location include/spdlog/sinks)

set(libydk_install_headers
    src/netconf_model_provider.hpp
    src/capabilities_parser.hpp
    src/codec_provider.hpp
    src/codec_service.hpp
    src/common_utilities.hpp
    src/crud_service.hpp
    src/entity_data_node_walker.hpp
    src/entity_lookup.hpp
    src/entity_util.hpp
    src/errors.hpp
    src/executor_service.hpp
    src/filters.hpp
    src/ietf_parser.hpp
    src/json.hpp
    src/logger.hpp
    src/logging_callback.hpp
    src/netconf_client.hpp
    src/netconf_provider.hpp
    src/netconf_service.hpp
    src/network_topology.hpp
    src/opendaylight_parser.hpp
    src/opendaylight_provider.hpp
    src/path_api.hpp
    src/restconf_client.hpp
    src/restconf_provider.hpp
    src/service_provider.hpp
    src/types.hpp
    src/json_subtree_codec.hpp
    src/xml_subtree_codec.hpp
    src/xml_util.hpp
    src/validation_service.hpp
    src/ydk.h)

# Set LibNetconf variables
set(libnetconf_location ${CMAKE_CURRENT_BINARY_DIR}/project_libnetconf/src/project_libnetconf)

set(libnetconf_headers
    ${libnetconf_location}/src/callbacks.h
    ${libnetconf_location}/src/callbacks_ssh.h
    ${libnetconf_location}/src/callhome.h
    ${libnetconf_location}/src/config.h
    ${libnetconf_location}/src/config.h.in
    ${libnetconf_location}/src/datastore.h
    ${libnetconf_location}/src/datastore_xml.h
    ${libnetconf_location}/src/error.h
    ${libnetconf_location}/src/libnetconf.h
    ${libnetconf_location}/src/libnetconf_ssh.h
    ${libnetconf_location}/src/libnetconf_tls.h
    ${libnetconf_location}/src/libnetconf_xml.h
    ${libnetconf_location}/src/messages.h
    ${libnetconf_location}/src/messages_internal.h
    ${libnetconf_location}/src/messages_xml.h
    ${libnetconf_location}/src/nacm.h
    ${libnetconf_location}/src/netconf.h
    ${libnetconf_location}/src/netconf_internal.h
    ${libnetconf_location}/src/notifications.h
    ${libnetconf_location}/src/notifications_xml.h
    ${libnetconf_location}/src/session.h
    ${libnetconf_location}/src/ssh.h
    ${libnetconf_location}/src/tls.h
    ${libnetconf_location}/src/transapi.h
    ${libnetconf_location}/src/transport.h
    ${libnetconf_location}/src/url.h
    ${libnetconf_location}/src/url_internal.h
    ${libnetconf_location}/src/with_defaults.h
    ${libnetconf_location}/src/datastore/datastore_internal.h
    ${libnetconf_location}/src/datastore/edit_config.h
    ${libnetconf_location}/src/datastore/custom/datastore_custom.h
    ${libnetconf_location}/src/datastore/custom/datastore_custom_private.h
    ${libnetconf_location}/src/datastore/empty/datastore_empty.h
    ${libnetconf_location}/src/datastore/file/datastore_file.h
)

set(netconf_objs
    callbacks.o
    compat.o
    datastore.o
    datastore_custom.o
    datastore_empty.o
    datastore_file.o
    edit_config.o
    error.o
    internal.o
    messages.o
    nacm.o
    notifications.o
    session.o
    ssh.o
    transapi.o
    transport.o
    url.o
    with_defaults.o
    xmldiff.o
    yinparser.o
)

# Set LibYang variables
set(libyang_location ${CMAKE_CURRENT_BINARY_DIR}/project_libyang/src/project_libyang)
set(libyang_build_location ${CMAKE_CURRENT_BINARY_DIR}/project_libyang/src/project_libyang-build)

set(libyang_headers
    ${libyang_location}/src/common.h
    ${libyang_location}/src/context.h
    ${libyang_location}/src/dict.h
    ${libyang_location}/src/dict_private.h
    ${libyang_location}/src/extensions.h
    ${libyang_build_location}/src/extensions_config.h
    ${libyang_build_location}/src/libyang.h
    ${libyang_location}/src/parser.h
    ${libyang_location}/src/parser_yang.h
    ${libyang_location}/src/parser_yang_bis.h
    ${libyang_location}/src/parser_yang_lex.h
    ${libyang_location}/src/printer.h
    ${libyang_location}/src/resolve.h
    ${libyang_location}/src/tree_data.h
    ${libyang_location}/src/tree_internal.h
    ${libyang_location}/src/tree_schema.h
    ${libyang_location}/src/validation.h
    ${libyang_location}/src/xml.h
    ${libyang_location}/src/xml_internal.h
    ${libyang_location}/src/xpath.h
)

set(yang_objs
    common.c.o
    context.c.o
    dict.c.o
    extensions.c.o
    log.c.o
    parser.c.o
    parser_json.c.o
    parser_xml.c.o
    parser_yang.c.o
    parser_yang_bis.c.o
    parser_yang_lex.c.o
    parser_yin.c.o
    printer.c.o
    printer_info.c.o
    printer_json.c.o
    printer_tree.c.o
    printer_xml.c.o
    printer_yang.c.o
    printer_yin.c.o
    resolve.c.o
    tree_data.c.o
    tree_schema.c.o
    validation.c.o
    xml.c.o
    xpath.c.o
    yang_types.c.o
)

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

# Add LibNetconf
set(libnetconf_include_location include/libnetconf)
set(libnetconf_headers_location ${CMAKE_CURRENT_BINARY_DIR}/project_libnetconf/headers)

include(ExternalProject)
ExternalProject_Add(project_libnetconf
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/project_libnetconf"
    URL "https://github.com/ygorelik/libnetconf/archive/master.zip"
    CONFIGURE_COMMAND ./configure --enable-debug
    BUILD_COMMAND "make"
    INSTALL_DIR "${libnetconf_location}"
    INSTALL_COMMAND mkdir -p ${libnetconf_headers_location}/libnetconf && cp ${libnetconf_headers} ${libnetconf_headers_location}/libnetconf
    BUILD_IN_SOURCE 1
)
install(FILES ${libnetconf_headers} DESTINATION ${libnetconf_include_location})

ExternalProject_Get_Property(project_libnetconf install_dir)
set(libnetconf_archive ${install_dir}/.libs/libnetconf.a)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/project_libnetconf/objs
    COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/project_libnetconf/objs
)
set (libnetconf_objs ${CMAKE_CURRENT_BINARY_DIR}/project_libnetconf/objs)

list(APPEND libnetconf_inc_objs "")
foreach(netconf_obj IN LISTS netconf_objs)
    add_custom_command(
        OUTPUT ${libnetconf_objs}/${netconf_obj}
        COMMAND ar -x ${libnetconf_archive} ${netconf_obj}
        MAIN_DEPENDENCY ${libnetconf_objs}
        WORKING_DIRECTORY ${libnetconf_objs}
    )
    list(APPEND libnetconf_inc_objs ${libnetconf_objs}/${netconf_obj})
endforeach(netconf_obj)

add_library(libnetconf STATIC IMPORTED)
set_property(TARGET libnetconf PROPERTY IMPORTED_LOCATION ${libnetconf_archive})
add_dependencies(libnetconf project_libnetconf)

set(LIBYANG_EXT_PLUGINS_DIR ${LIB_INSTALL_DIR}/libyang)
message("Set libyang plugin installation directory to: ${LIBYANG_EXT_PLUGINS_DIR}")

# Add LibYang
set(libyang_include_location include/libyang)
set(libyang_headers_location ${CMAKE_CURRENT_BINARY_DIR}/project_libyang/headers)
ExternalProject_Add(project_libyang
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/project_libyang"
    URL "https://github.com/ygorelik/libyang/archive/master.zip"
    CMAKE_ARGS -DSTATIC=true -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_LIBDIR=lib
    INSTALL_DIR "${libyang_location}"
    INSTALL_COMMAND mkdir -p ${libyang_headers_location}/libyang && cp ${libyang_headers} ${libyang_headers_location}/libyang
)
install(FILES ${libyang_headers} DESTINATION ${libyang_include_location})

ExternalProject_Get_Property(project_libyang install_dir)
set(libyang_archive ${CMAKE_CURRENT_BINARY_DIR}/project_libyang/src/project_libyang-build/libyang.a)

add_library(libyang STATIC IMPORTED)
set_property(TARGET libyang PROPERTY IMPORTED_LOCATION ${libyang_archive})
add_dependencies(libyang project_libyang)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/project_libyang/objs
    COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/project_libyang/objs
)
set (libyang_objs ${CMAKE_CURRENT_BINARY_DIR}/project_libyang/objs)

list(APPEND libyang_inc_objs "")
foreach(yang_obj IN LISTS yang_objs)
    add_custom_command(
        OUTPUT ${libyang_objs}/${yang_obj}
        COMMAND ar -x ${libyang_archive} ${yang_obj}
        MAIN_DEPENDENCY ${libyang_objs}
        WORKING_DIRECTORY ${libyang_objs}
    )
    list(APPEND libyang_inc_objs ${libyang_objs}/${yang_obj})
endforeach(yang_obj)

include_directories(
    ${LIBYANG_INCLUDE_DIRS}
    ${LIBSSH_INCLUDE_DIRS}
    ${LIBXML2_INCLUDE_DIR}
    ${libnetconf_headers_location}
    ${libnetconf_location}/headers
    ${libyang_headers_location}
    ${libyang_location}/headers
    ${PROJECT_SOURCE_DIR}/src
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${CPACK_LIBRARY_NAME} STATIC ${libydk_src} ${libnetconf_inc_objs} ${libyang_inc_objs})

target_link_libraries(${CPACK_LIBRARY_NAME}
    ${LIBXML2_LIBRARIES}
    ${LIBSSH_LIBRARIES}
    ${curl_location}
    ${ssh_threads_location}
    ${pcre_location}
    ${xslt_location}
    ${pthread_location}
    libnetconf
    libyang
    -rdynamic
    ${dl_location}
)

set_property(TARGET ${CPACK_LIBRARY_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${CPACK_LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set(libyang_extension_lib
	${libyang_build_location}/src/extensions/libyang_ext_test${CMAKE_SHARED_LIBRARY_SUFFIX}
	${libyang_build_location}/src/extensions/metadata${CMAKE_SHARED_LIBRARY_SUFFIX}
	${libyang_build_location}/src/extensions/nacm${CMAKE_SHARED_LIBRARY_SUFFIX}
)

install(TARGETS ${CPACK_LIBRARY_NAME} DESTINATION ${LIB_INSTALL_DIR})
install(FILES ${libydk_install_headers} DESTINATION ${INCLUDE_INSTALL_DIR})
install(FILES ${SPDLOG_HEADERS} DESTINATION ${spdlog_include_location})
install(FILES ${SPDLOG_DETAILS_HEADERS} DESTINATION ${spdlog_details_include_location})
install(FILES ${SPDLOG_FMT_HEADERS} DESTINATION ${spdlog_fmt_include_location})
install(FILES ${SPDLOG_FMT_BUNDLED_HEADERS} DESTINATION ${spdlog_fmt_bundled_include_location})
install(FILES ${SPDLOG_SINKS_HEADERS} DESTINATION ${spdlog_sinks_include_location})
install(FILES ${libyang_extension_lib} DESTINATION ${LIBYANG_EXT_PLUGINS_DIR})

SET( legacy_link   ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/libydk.a)
SET( legacy_target ${CMAKE_INSTALL_PREFIX}/${LIB_INSTALL_DIR}/${CPACK_LIBRARY_TARGET})
ADD_CUSTOM_COMMAND( TARGET ${CPACK_LIBRARY_NAME}
                    COMMAND echo "Creating symbolic link ${legacy_link}"
                    COMMAND sudo rm -f ${legacy_link}
                    COMMAND sudo ln -s ${CPACK_LIBRARY_TARGET} ${legacy_link}
                    DEPENDS install ${CPACK_LIBRARY_NAME}
                  )
ADD_CUSTOM_TARGET( install_legacy DEPENDS ${legacy_target} )

# generate doxygen documentation for ydk_core API
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_SKIP_DOT TRUE)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    configure_file(Doxyfile.in Doxyfile)
endif()

add_subdirectory(samples)

# clean cmake cache
add_custom_target(cclean
    COMMAND make clean
    COMMAND find . -iname '*cmake*' -not -name CMakeLists.txt -not -path './CMakeModules*' -exec rm -rf {} +
    COMMAND rm -rf Makefile Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

if(ENABLE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif(ENABLE_BUILD_TESTS)
