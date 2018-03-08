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
    echo -e "${RED}*** $(date) *** dependencies_linux.sh | $1${NOCOLOR}"
}

function install_dependencies {
    print_msg "Installing dependencies"

    apt update -y > /dev/null
    apt install sudo -y > /dev/null
    sudo apt-get update > /dev/null
    sudo apt-get install libtool-bin -y > /dev/null
    local status=$?
    if [[ ${status} != 0 ]]; then
        sudo apt-get install libtool -y > /dev/null
    fi
    sudo apt-get install -y bison \
                            curl \
                            doxygen \
                            flex \
                            git \
                            libcmocka0 \
                            libcurl4-openssl-dev \
                            libpcre3-dev \
                            libpcre++-dev \
                            libssh-dev \
                            libxml2-dev \
                            libxslt1-dev \
                            pkg-config \
                            python-dev \
                            python-pip \
                            python3-dev \
                            python-lxml \
                            python3-lxml \
                            python-virtualenv \
                            software-properties-common \
                            unzip \
                            wget \
                            zlib1g-dev\
                            lcov \
                            openjdk-8-jre \
                            golang \
                            cmake \
                            gdebi-core\
                            lcov > /dev/null

    # gcc-5 and g++5 for modern c++
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
    sudo apt-get update > /dev/null
    sudo apt-get install gcc-5 g++-5 -y > /dev/null
    sudo ln -f -s /usr/bin/g++-5 /usr/bin/c++
    sudo ln -f -s /usr/bin/gcc-5 /usr/bin/cc
}

########################## EXECUTION STARTS HERE #############################

install_dependencies
