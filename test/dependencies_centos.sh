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
# This file has been modified by Yan Gorelik, YDK Solutions.
# All modifications in original under CiscoDevNet domain
# introduced since October 2019 are copyrighted.
# All rights reserved under Apache License, Version 2.0.
# ------------------------------------------------------------------
#
# Script for running ydk CI on docker via travis-ci.org
#
# ------------------------------------------------------------------

function print_msg {
    echo -e "${MSG_COLOR}*** $(date) *** dependencies_centos.sh | $@ ${NOCOLOR}"
}

function run_cmd {
    local cmd=$@
    print_msg "Running: $cmd"
    $@
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Exiting '$@' with status=$status"
        exit $status
    fi
    return $status
}

function check_install_gcc {
  which gcc
  local status=$?
  if [[ $status == 0 ]]
  then
    gcc_version=$(echo $(gcc --version) | awk '{ print $3 }')
    print_msg "Current gcc/g++ version is $gcc_version"
  else
    print_msg "The gcc/g++ not installed"
    gcc_version="4.0.0"
  fi
  if [[ $gcc_version < "4.8.1" ]]
  then
    print_msg "Upgrading gcc/g++ to version 7"
    sudo yum install centos-release-scl -y > /dev/null
    sudo yum install devtoolset-7-gcc* -y > /dev/null
    local status2=$?
    if [[ $status2 != 0 ]]; then
      MSG_COLOR=$RED
      print_msg "Failed to install gcc; exiting"
      exit 1
    else
      sudo ln -sf /opt/rh/devtoolset-7/root/usr/bin/gcc /usr/bin/cc
      sudo ln -sf /opt/rh/devtoolset-7/root/usr/bin/g++ /usr/bin/c++

      sudo ln -sf /opt/rh/devtoolset-7/root/usr/bin/gcc /usr/bin/gcc
      sudo ln -sf /opt/rh/devtoolset-7/root/usr/bin/g++ /usr/bin/g++

#      sudo rm -rf /usr/lib64/libstdc++.so.6
#      sudo ln -sf /opt/rh/devtoolset-7/root/usr/lib/gcc/x86_64-redhat-linux/7/libstdc++.so /usr/lib64/libstdc++.so.6

      sudo ln -sf /opt/rh/devtoolset-7/root/usr/bin/gcov /usr/bin/gcov
      gcc_version=$(echo $(gcc --version) | awk '{ print $3 }')
      print_msg "Installed gcc/g++ version is $gcc_version"
    fi
  fi
}

function install_dependencies {
    print_msg "Installing dependencies"
    yum install sudo -y
    run_cmd sudo yum update -y > /dev/null
    run_cmd sudo yum install epel-release -y > /dev/null
#    run_cmd sudo yum install https://centos7.iuscommunity.org/ius-release.rpm -y > /dev/null
    run_cmd sudo yum install git which libxml2-devel libxslt-devel libssh-devel libtool gcc-c++ pcre-devel -y > /dev/null
    run_cmd sudo yum install cmake3 wget curl-devel unzip make java doxygen mlocate -y > /dev/null
#     sudo yum install python-devel python-pip -y
    run_cmd sudo yum install python3-devel python3-venv -y
    run_cmd sudo yum install rpm-build redhat-lsb lcov -y > /dev/null
    run_cmd sudo yum install valgrind -y
}

function check_install_go {
  go_exec=$(which go)
  if [[ -z ${go_exec} && -d /usr/local/go ]]; then
    go_exec=/usr/local/go/bin/go
  fi
  if [[ -x ${go_exec} ]]
  then
    go_version=$(echo `$go_exec version` | awk '{ print $3 }' | cut -d 'o' -f 2)
    print_msg "Current Go version is $go_version"
    minor=$(echo $go_version | cut -d '.' -f 2)
  else
    print_msg "The Go is not installed"
    minor=0
  fi
  if (( $minor < 9 )); then
    print_msg "Installing Golang version 1.9.2"
    run_cmd sudo wget https://storage.googleapis.com/golang/go1.9.2.linux-amd64.tar.gz &> /dev/null
    sudo tar -zxf  go1.9.2.linux-amd64.tar.gz -C /usr/local/
    rm -f go1.9.2.linux-amd64.tar.gz
    cd /usr/local/bin
    sudo ln -sf /usr/local/go/bin/go
    cd -
  fi
}

function install_confd {
  if [[ ! -s $HOME/confd/bin/confd ]]; then
    print_msg "Installing confd"
    run_cmd wget https://github.com/CiscoDevNet/ydk-gen/files/562538/confd-basic-6.2.linux.x86_64.zip &> /dev/null
    unzip confd-basic-6.2.linux.x86_64.zip
    run_cmd ./confd-basic-6.2.linux.x86_64.installer.bin $HOME/confd
    rm -f confd-basic-6.2.* ConfD*
  fi
}

function install_openssl {
  if [[ ! -s $HOME/confd/lib/libcrypto.so.1.0.0 ]]; then
    print_msg "Installing openssl 0.1.0u for confd"
    run_cmd wget https://www.openssl.org/source/openssl-1.0.1u.tar.gz &> /dev/null
    tar -xvzf openssl-1.0.1u.tar.gz > /dev/null
    rm -rf openssl-1.0.1u.tar.gz
    cd openssl-1.0.1u
    run_cmd ./config shared  > /dev/null
    run_cmd make all > /dev/null
    cp libcrypto.so.1.0.0 $HOME/confd/lib
    cd -
    rm -rf openssl-1.0.1u
  fi
}

########################## EXECUTION STARTS HERE #############################
# Terminal colors
NOCOLOR="\033[0m"
YELLOW='\033[1;33m'
MSG_COLOR=$YELLOW

install_dependencies
check_install_gcc
check_install_go

install_confd
install_openssl

updatedb
