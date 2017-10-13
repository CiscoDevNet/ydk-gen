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

    brew install curl \
                 doxygen \
                 libssh \
                 pcre \
                 wget \
                 xml2 \
                 lcov \
                 pybind11 > /dev/null
    brew install libssh
    brew link libssh
    brew rm -f --ignore-dependencies python python3
    wget https://www.python.org/ftp/python/3.6.3/python-3.6.3-macosx10.6.pkg
    sudo installer -pkg python-3.6.3-macosx10.6.pkg  -target /

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

########################## EXECUTION STARTS HERE #############################

install_dependencies
install_confd
download_moco

sudo easy_install pip
sudo pip install virtualenv
