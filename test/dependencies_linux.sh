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

    sudo apt-get update > /dev/null
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
                            libtool \
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
                            cmake > /dev/null

    # gcc-5 and g++5 for modern c++
    sudo add-apt-repository ppa:ubuntu-toolchain-r/test -y
    sudo apt-get update
    sudo apt-get install gcc-5 g++-5 -y
    sudo ln -f -s /usr/bin/g++-5 /usr/bin/g++
    sudo ln -f -s /usr/bin/gcc-5 /usr/bin/gcc
}

function install_confd {
    print_msg "Installing confd"

    wget https://github.com/CiscoDevNet/ydk-gen/files/562538/confd-basic-6.2.linux.x86_64.zip
    unzip confd-basic-6.2.linux.x86_64.zip
    ./confd-basic-6.2.linux.x86_64.installer.bin ../confd
}

function download_moco {
    print_msg "Downloading moco"
    cd test
    wget https://repo1.maven.org/maven2/com/github/dreamhead/moco-runner/0.11.0/moco-runner-0.11.0-standalone.jar
    cd -
}

########################## EXECUTION STARTS HERE #############################

install_dependencies
install_confd
download_moco
