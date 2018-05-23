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


function install_confd {
    print_msg "Installing confd"

    wget https://github.com/CiscoDevNet/ydk-gen/files/562538/confd-basic-6.2.linux.x86_64.zip &> /dev/null
    unzip confd-basic-6.2.linux.x86_64.zip
    ./confd-basic-6.2.linux.x86_64.installer.bin ../confd
}

function install_openssl {
    print_msg "Installing openssl 0.1.0u for confd"

    wget https://www.openssl.org/source/openssl-1.0.1u.tar.gz &> /dev/null
    tar -xvzf openssl-1.0.1u.tar.gz > /dev/null
    cd openssl-1.0.1u
    ./config shared  > /dev/null && make all > /dev/null
    cp libcrypto.so.1.0.0 ../../confd/lib
    cd -

    print_msg "Done Installing openssl 0.1.0u"
}

function install_fpm {
    print_msg "Installing fpm"
    yum install ruby-devel gcc make rpm-build rubygems -y > /dev/null
    gem install --no-ri --no-rdoc fpm
}

########################## EXECUTION STARTS HERE #############################

./test/dependencies_centos_basic.sh
install_confd
install_openssl
#install_fpm
