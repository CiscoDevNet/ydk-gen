#!/bin/bash
#  ----------------------------------------------------------------
# Copyright 2018-2019 Cisco Systems
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
# dependencies_linux_gnmi.sh
# Script to install protobuf, protoc and grpc on Ubuntu and CentOS
# for running YDK gNMI tests on docker
# and installing YDK from install_ydk.sh script
# ------------------------------------------------------------------

function print_msg {
    echo -e "${MSG_COLOR}*** $(date) *** dependencies_linux_gnmi.sh | $* ${NOCOLOR}"
}

function install_protobuf {
  if [[ ! -x /usr/local/lib/libprotoc.so.15.0.0 ]]; then
    if [[ ! -d $HOME/protobuf-3.5.0 ]]; then
      cd $HOME
      print_msg "Downloading protobuf and protoc"
      wget https://github.com/google/protobuf/releases/download/v3.5.0/protobuf-cpp-3.5.0.zip > /dev/null
      unzip protobuf-cpp-3.5.0.zip > /dev/null
      rm -f protobuf-cpp-3.5.0.zip
      cd - > /dev/null
    fi
    cd $HOME/protobuf-3.5.0
    print_msg "Configuring protobuf and protoc"
    ./configure > /dev/null
    print_msg "Compiling protobuf and protoc"
    make > /dev/null
    print_msg "Installing protobuf and protoc"
    $sudo_cmd make install
    $sudo_cmd ldconfig
    cd - > /dev/null
  fi
}

function install_grpc {
  if [[ ! -x /usr/local/lib/libgrpc.a ]]; then
    if [[ ! -d $HOME/grpc ]]; then
      cd $HOME
      print_msg "Downloading grpc"
      git clone -b v1.9.1 https://github.com/grpc/grpc
      cd grpc
      git submodule update --init
      # Correcting source code, which fails in focal with gcc-7 and jammy with gcc-11
      cp $curr_dir/3d_party/grpc/log_linux.cc src/core/lib/gpr/
      cp $curr_dir/3d_party/grpc/Makefile .
      cd $curr_dir
    fi
    print_msg "Compiling grpc"
    cd $HOME/grpc
    make > /dev/null
    local status=$?
    if [ $status -ne 0 ]; then
       print_msg "Failed to compile grpc code; exiting"
       exit $status
    fi
    $sudo_cmd make install
    $sudo_cmd ldconfig
    cd - > /dev/null
  fi
}

########################## EXECUTION STARTS HERE #############################

# Terminal colors
NOCOLOR="\033[0m"
YELLOW='\033[1;33m'
MSG_COLOR=$YELLOW

sudo_cmd=
if [ $(id -u -n) != "root" ]; then
  sudo_cmd="sudo"
fi

curr_dir=$(pwd)
os_info=$(cat /etc/*-release)
codename=$(lsb_release -c | awk '{ print $2 }')

if [[ -z ${C_INCLUDE_PATH} ]]; then
    export C_INCLUDE_PATH=/usr/local/include
fi
if [[ -z ${CPLUS_INCLUDE_PATH} ]]; then
    export CPLUS_INCLUDE_PATH=/usr/local/include
fi

export CFLAGS="${CFLAGS} -Wno-error"
export CXXFLAGS="${CXXFLAGS} -Wno-error"
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:/usr/local/lib

install_protobuf
install_grpc
