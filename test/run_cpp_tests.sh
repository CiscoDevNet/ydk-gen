#!/bin/bash
#  -----------------------------------------------------------------------------
# Copyright 2020 Yan Gorelik, YDK Solutions
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
# ------------------------------------------------------------------------------
#
# Bash script to install YDK-CPP and run unit tests
# ------------------------------------------------------------------------------

function print_msg {
    echo -e "${MSG_COLOR}*** $(date): run_cpp_test.sh: $* ${NOCOLOR}"
}

function run_cmd {
    $*
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Exiting '$*' with status=$status"
        exit $status
    fi
    return $status
}

# ------------------------------------------------------------------
# C++ specific functions
# ------------------------------------------------------------------

function install_test_cpp_core {
    print_msg "Installing / testing cpp core"
    install_cpp_core
    run_cpp_core_test
}

function install_cpp_core {
    print_msg "Installing cpp core"

    cd $YDKGEN_HOME
    rm -rf sdk/cpp/core/build > /dev/null
    mkdir -p $YDKGEN_HOME/sdk/cpp/core/build
    cd $YDKGEN_HOME/sdk/cpp/core/build
    
    run_cmd ${CMAKE_BIN} ..
    run_cmd make
    run_cmd sudo make install
}

function run_cpp_core_test {
    print_msg "Running cpp core test"

    make test
    local status=$?
    if [ $status -ne 0 ]; then
        # If the tests fail, repeat them in verbose mode
        ./tests/ydk_core_test -d yes
        MSG_COLOR=$RED
        print_msg "Exiting 'run_cpp_core_test' with status $status"
        exit $status
    fi
    cd $YDKGEN_HOME
}

function run_cpp_bundle_tests {
    print_msg "Generating and testing C++ bundle"

    cpp_sanity_ydktest_gen_install
    cpp_sanity_ydktest_test
}

function generate_install_specified_cpp_bundle {
   bundle_profile=$1
   bundle_name=$2
   print_msg "Generating and installing C++ ydktest bundle $bundle_name"
   cd $YDKGEN_HOME
   sudo rm -rf ./gen-api/cpp/$bundle_name
   run_cmd python3 generate.py --bundle $bundle_profile --cpp -v
   cd gen-api/cpp/$2/build
   run_cmd make
   run_cmd sudo make install
   cd - > /dev/null
}

function cpp_sanity_ydktest_gen_install {
    sudo rm -rf $YDKGEN_HOME/gen-api/.cache

    generate_install_specified_cpp_bundle profiles/test/ydktest-cpp.json ydktest-bundle
}

function cpp_sanity_ydktest_test {
    print_msg "Initializing ssh keys for key-based authentication"
    sudo mkdir -p /var/confd/homes/admin/.ssh
    sudo touch /var/confd/homes/admin/.ssh/authorized_keys
    cd $YDKGEN_HOME
    sudo sh -c 'cat sdk/cpp/tests/ssh_host_rsa_key.pub >> /var/confd/homes/admin/.ssh/authorized_keys'
    cd - > /dev/null

    print_msg "Building cpp bundle tests"
    rm -rf sdk/cpp/tests/build > /dev/null
    mkdir sdk/cpp/tests/build
    cd sdk/cpp/tests/build

    run_cmd ${CMAKE_BIN} ..
    run_cmd make

    run_cmd $script_dir/init_test_env.sh

    print_msg "Running cpp bundle tests"
    # Need to touch the restcom server for the first time;
    # expected to fail, but main test should pass
    ./ydk_bundle_test c_api_provider_withpath
    run_cmd ./ydk_bundle_test -d yes
}

function cpp_test_gen_test {
    print_msg "Running cpp_test_gen_test"

    cd $YDKGEN_HOME
    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/testgen/confd
    mkdir -p gen-api/cpp/models_test-bundle/ydk/models/models_test/test/build
    cd gen-api/cpp/models_test-bundle/ydk/models/models_test/test/build
    run_cmd ${CMAKE_BIN} ..
    run_cmd make
    ctest --output-on-failure

    os_type=$(uname)
    if [[ ${os_type} == "Linux" ]] ; then
        print_msg "Running tcp tests on linux"
        run_cmd ./ydk_bundle_test *tcp* -d yes
    fi
}

function cpp_test_gen {
    print_msg "Running cpp_test_gen"

    cd $YDKGEN_HOME
    run_cmd python3 generate.py --bundle profiles/test/ydk-models-test.json --generate-tests --cpp &> /dev/null
    cd gen-api/cpp/models_test-bundle/build/
    run_cmd sudo make install

    # cpp_test_gen_test
}

function init_gnmi_server {
    print_msg "Starting YDK gNMI server"
    mkdir -p $YDKGEN_HOME/test/gnmi_server/build && cd $YDKGEN_HOME/test/gnmi_server/build
    ${CMAKE_BIN} .. && make clean && make
    ./gnmi_server &
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Could not start gNMI server"
        exit $status
    fi
    cd -
}

function stop_gnmi_server {
    print_msg "Stopping gNMI server"
    pkill -f gnmi_server
}

function build_gnmi_core_library {
    print_msg "Building core gnmi library"
    cd $YDKGEN_HOME/sdk/cpp/gnmi
    mkdir -p build
    cd build
    ${CMAKE_BIN} .. && make clean && make
    sudo make install
    cd $YDKGEN_HOME
}

function build_and_run_tests {
    print_msg "Building gnmi tests"
    cd $YDKGEN_HOME/sdk/cpp/gnmi/tests
    mkdir -p build
    cd build
    ${CMAKE_BIN} .. && make clean && make

    init_gnmi_server
    ./ydk_gnmi_test -d yes
    stop_gnmi_server
}

function run_cpp_gnmi_tests {
    build_gnmi_core_library
    build_and_run_tests
}

function run_cpp_gnmi_memcheck_tests {
    print_msg "Building gnmi sample tests"
    cd $YDKGEN_HOME/sdk/cpp/gnmi/samples
    mkdir -p build
    cd build
    ${CMAKE_BIN} .. && make clean && make

    init_gnmi_server
    print_msg "Running gnmi sample tests with memcheck"
    valgrind --leak-check=summary ./bgp_gnmi_subscribe ssh://admin:admin@127.0.0.1:50051
    stop_gnmi_server
}

########################## EXECUTION STARTS HERE #############################
# ------------------------------------------------------------------
# Main part
# ------------------------------------------------------------------

# Terminal colors
RED='\033[0;31m'
GREEN='\033[1;32m'
NOCOLOR='\033[0m'
YELLOW='\033[1;33m'
MSG_COLOR=$YELLOW

os_type=$(uname)
if [[ ${os_type} == "Linux" ]] ; then
    os_info=$(cat /etc/*-release)
else
    os_info=$(sw_vers)
fi
print_msg "Running OS type: $os_type"
print_msg "OS info: $os_info"

script_dir=$(cd $(dirname ${BASH_SOURCE}) > /dev/null && pwd)

if [ -z ${YDKGEN_HOME} ] || [ ! -d ${YDKGEN_HOME} ]; then
  YDKGEN_HOME=$(cd "$script_dir/../" > /dev/null && pwd)
  print_msg "YDKGEN_HOME is set to ${YDKGEN_HOME}"
fi

CMAKE_BIN=cmake
command -v cmake3
status=$?
if [[ ${status} == 0 ]] ; then
    CMAKE_BIN=cmake3
fi

if [[ $(uname) == "Linux" && ${os_info} == *"fedora"* ]]; then
  if [[ $LD_LIBRARY_PATH != *"protobuf-3.5.0"* ]]; then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/usr/local/lib64:/usr/lib64
    print_msg "LD_LIBRARY_PATH is set to: $LD_LIBRARY_PATH"
  fi
  centos_version=$(echo `lsb_release -r` | awk '{ print $2 }' | cut -d '.' -f 1)
fi

curr_dir=$(pwd)

cd $YDKGEN_HOME

install_test_cpp_core

run_cpp_bundle_tests

run_cpp_gnmi_tests

command -v valgrind
status=$?
if [[ ${status} == 0 ]]; then
    run_cpp_gnmi_memcheck_tests
fi

$script_dir/clean_test_env.sh

cd $curr_dir
