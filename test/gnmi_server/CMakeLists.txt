cmake_minimum_required(VERSION 3.3)
project(ydk_gnmi)

include(ExternalProject)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

find_package(Protobuf REQUIRED)
find_package(GRPC REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE debug)
endif()

# Add LibgNMI
set(libgnmi_headers
    ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated/gnmi.grpc.pb.h
    ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated/gnmi.pb.h)

set(libgnmi_sources
    ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated/gnmi.grpc.pb.cc
    ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated/gnmi.pb.cc)

set(libgnmi_headers_location ${CMAKE_CURRENT_BINARY_DIR}/project_libgnmi/generated)

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

message("oops ${libgnmi_sources}")

add_custom_command(OUTPUT ${libgnmi_sources}
  DEPENDS download_generate_proto_cpp
  COMMENT "Generating GNMI code"
  VERBATIM
)

add_custom_target(ydk_gnmi ALL DEPENDS ${libgnmi_sources})

include_directories(
    ${Protobuf_INCLUDE_DIR}
    ${libgnmi_headers_location}
)

add_executable(gnmi_server "gnmi_server.cpp" ${libgnmi_sources})

set(IS_SECURE True)
if(${IS_SECURE})
#    target_link_directories(gnmi_server PUBLIC /usr/local/lib)
    target_link_libraries(gnmi_server ${PROTOBUF_LIBRARY} grpc++)
else(NOT ${IS_SECURE})
    target_link_libraries(gnmi_server ${PROTOBUF_LIBRARY} grpc++_unsecure)
endif()
