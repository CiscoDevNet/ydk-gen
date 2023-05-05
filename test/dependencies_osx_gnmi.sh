#!/bin/bash
#  ----------------------------------------------------------------
# Copyright 2018 Cisco Systems
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
# This file has been modified by Yan Gorelik, YDK Solutions.
# All modifications in original under CiscoDevNet domain
# introduced since October 2019 are copyrighted.
# All rights reserved under Apache License, Version 2.0.
# ------------------------------------------------------------------
#
# dependencies_osx_gnmi.sh
# Script for running ydk CI on docker via travis-ci.org
#
# ------------------------------------------------------------------

function print_msg {
    echo -e "${MSG_COLOR}*** $(date) *** dependencies_osx_gnmi.sh | $@ ${NOCOLOR}"
}

function install_protobuf {
  if [[ ! -d protobuf-3.5.0 ]]; then
    print_msg "Downloading protobuf and protoc"
    wget https://github.com/google/protobuf/releases/download/v3.5.0/protobuf-cpp-3.5.0.zip > /dev/null
    unzip protobuf-cpp-3.5.0.zip > /dev/null
    rm -f protobuf-cpp-3.5.0.zip
  fi
  if [[ ! -x /usr/local/lib/libprotoc.dylib ]]; then
    cd protobuf-3.5.0
    print_msg "Configuring protobuf and protoc"
    ./configure > /dev/null
    print_msg "Compiling protobuf and protoc"
    make > /dev/null
    print_msg "Installing protobuf and protoc"
    sudo make install
    cd $curr_dir
  fi
}

function install_grpc {
  if [[ ! -d grpc ]]; then
    print_msg "Installing grpc"
    #LIBTOOL=glibtool LIBTOOLIZE=glibtoolize make
    git clone -b v1.9.1 https://github.com/grpc/grpc
    cd grpc
    git submodule update --init
    cd -
  fi
  if [[ ! -x /usr/local/lib/libgrpc.a ]]; then
    cd grpc
    make > /dev/null
    sudo make install
    cd $curr_dir
  fi
}

########################## EXECUTION STARTS HERE #############################

# Terminal colors
NOCOLOR="\033[0m"
YELLOW='\033[1;33m'
MSG_COLOR=$YELLOW

curr_dir="$(pwd)"
install_protobuf
install_grpc
