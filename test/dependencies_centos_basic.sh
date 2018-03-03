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
    echo -e "${RED}*** $(date) *** dependencies_centos.sh | $1${NOCOLOR}"
}

function install_dependencies {
    print_msg "Installing dependencies"

    yum update -y > /dev/null
    yum install epel-release -y > /dev/null
    yum install https://centos7.iuscommunity.org/ius-release.rpm -y > /dev/null
    yum install git which libxml2-devel libxslt-devel libssh-devel libtool gcc-c++ pcre-devel \
                cmake3 wget curl-devel unzip python-devel python-pip make go java sudo \
                python36u-devel python36u-pip  rpm-build redhat-lsb -y  \
                > /dev/null

     print_msg "Installing gcc5"
     yum install centos-release-scl -y > /dev/null
     yum install devtoolset-4-gcc* -y > /dev/null

     ln -sf /opt/rh/devtoolset-4/root/usr/bin/gcc /usr/bin/cc
     ln -sf /opt/rh/devtoolset-4/root/usr/bin/g++ /usr/bin/c++

     which gcc
     gcc --version
     print_msg "Done installing gcc5"
}



########################## EXECUTION STARTS HERE #############################

install_dependencies
