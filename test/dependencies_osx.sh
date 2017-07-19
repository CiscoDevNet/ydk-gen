#!/bin/bash
#  ----------------------------------------------------------------
# Copyright 2016 Cisco Systems
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ------------------------------------------------------------------
#
# Script for running ydk CI on docker via travis-ci.org
#
# ------------------------------------------------------------------

RED="\033[0;31m"
NOCOLOR="\033[0m"

function print_msg {
    echo -e "${RED}*** $(date) *** dependencies_osx.sh | $1${NOCOLOR}"
}

function install_dependencies {
    print_msg "install_dependencies"

    brew install autoconf \
                 automake \
                 curl \
                 doxygen \
                 gflags \
                 lcov \
                 libssh \
                 libtool \
                 pcre \
                 protobuf \
                 shtool \
                 wget \
                 xml2 

    brew install libssh
    brew link libssh
}

function install_confd {
    print_msg "install_confd"

    wget https://github.com/CiscoDevNet/ydk-gen/files/562559/confd-basic-6.2.darwin.x86_64.zip
    unzip confd-basic-6.2.darwin.x86_64.zip
    ./confd-basic-6.2.darwin.x86_64.installer.bin ../confd
}

function download_moco {
    print_msg "Downloading moco"
    cd test
    wget https://repo1.maven.org/maven2/com/github/dreamhead/moco-runner/0.11.0/moco-runner-0.11.0-standalone.jar
    cd -
}

function install_protobuf {
    print_msg "Installing protobuf and protoc"

    wget https://github.com/google/protobuf/releases/download/v3.3.0/protobuf-cpp-3.3.0.zip
    unzip protobuf-cpp-3.3.0.zip
    cd protobuf-cpp-3.3.0
    ./configure
    make
    make check
    sudo make install
}

function install_grpc {
    print_msg "Installing grpc"

    LIBTOOL=glibtool LIBTOOLIZE=glibtoolize make
    git clone -b $(curl -L https://grpc.io/release) https://github.com/grpc/grpc
    cd grpc
    git submodule update --init
    make
    sudo make install
}

########################## EXECUTION STARTS HERE #############################

install_dependencies
install_confd
download_moco
install_protobuf
install_grpc