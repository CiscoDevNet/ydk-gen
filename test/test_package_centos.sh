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
# Script for running ydk CI on travis-ci.org
#
# ------------------------------------------------------------------

# Terminal colors
RED="\033[0;31m"
NOCOLOR="\033[0m"

######################################################################
# Utility functions
######################################################################

function print_msg {
    echo -e "${RED}*** $(date): tests.sh | $1${NOCOLOR}"
}

function init_confd {
    cd $1
    print_msg "Initializing confd in $(pwd)"
    source $YDKGEN_HOME/../confd/confdrc
    make stop > /dev/null
    make clean > /dev/null
    make all > /dev/null
    make start
    cd -
}

########################## EXECUTION STARTS HERE #############################

######################################
# Set up env
######################################
export YDKGEN_HOME="$(pwd)"

./test/dependencies_centos.sh
init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/ydktest

######################################
# Install
######################################
print_msg "Installing libydk"
yum install -y libydk*.rpm

print_msg "Installing ydk-py"
cd sdk/python/core
python setup.py sdist
pip install dist/ydk*.tar.gz

######################################
# Test
######################################
print_msg "Running basic python test"
python -c 'import os;from ydk.path import Repository; from ydk.providers import NetconfServiceProvider; repo=Repository(os.getcwd()); p=NetconfServiceProvider(repo, "127.0.0.1", "admin","admin",12022)'
status=$?
if [ $status -ne 0 ]; then
    exit $status
fi
