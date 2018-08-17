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
#
# Script for running YDK gNMI tests on travis-ci.org
#
# ------------------------------------------------------------------

# Terminal colors
RED="\033[0;31m"
NOCOLOR="\033[0m"
YELLOW='\033[1;33m'
MSG_COLOR=$YELLOW

######################################################################
# Utility functions
######################################################################

function print_msg {
    echo -e "${MSG_COLOR}*** $(date): gnmi_tests.sh | $1${NOCOLOR}"
}

function run_exec_test {
    $@
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Exiting '$@' with status=$status"
        exit $status
    fi
    return $status
}

function run_test_no_coverage {
    print_msg "Executing: $@"
    ${PYTHON_BIN} $@
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Exiting '${PYTHON_BIN} $@' with status=$status"
        exit $status
    fi
    return $status
}

function run_test {
    if [[ $(command -v coverage) && ${os_type} == "Linux" ]]; then
        print_msg "Executing with coverage: $@"
        coverage run --omit=/usr/* --branch --parallel-mode $@ > /dev/null
        local status=$?
        print_msg "Returned status is ${status}"
        if [ $status -ne 0 ]; then
            MSG_COLOR=$RED
            print_msg "Exiting 'coverage run $@' with status=$status"
            exit $status
        fi
        return $status
    fi
    run_test_no_coverage $@
    local status=$?
    return $status
}

function pip_check_install {
    if [[ $(uname) == "Linux" ]] ; then
        os_info=$(cat /etc/*-release)
        if [[ ${os_info} == *"fedora"* ]]; then
            print_msg "Custom pip install of $@ for CentOS"
            ${PIP_BIN} install --install-option="--install-purelib=/usr/lib64/python${PYTHON_VERSION}/site-packages" --no-deps $@
            return
        fi
    fi
    ${PIP_BIN} install $@
}

######################################################################
# Environment setup-teardown functions
######################################################################

function init_py_env {
    print_msg "Initializing Python environment"
    if [[ ${os_type} == "Darwin" ]] ; then
        virtualenv macos_pyenv -p python3.6
        source macos_pyenv/bin/activate
    fi
    ${PIP_BIN} install -r requirements.txt coverage pybind11==2.2.2
}

function init_go_env {
    print_msg "Initializing Go environment"

    export GOROOT="/usr/local/go"
    export PATH=$GOROOT/bin:$PATH

    print_msg "GOPATH is set to: ${GOPATH}"
    print_msg "GOROOT is set to: ${GOROOT}"

    cd $YDKGEN_HOME
    if [[ -z "${GOPATH// }" ]]; then
        export GOPATH="$(pwd)/golang"
    else
        export GOPATH="$(pwd)/golang":$GOPATH
    fi

    print_msg "Changed GOPATH setting to: ${GOPATH}"
    print_msg "Running $(go version)"

    go get github.com/stretchr/testify
}

######################################################################
# C++ Core and bundle installation functions
######################################################################

function install_cpp_core {
    print_msg "Installing CPP core"

    cd $YDKGEN_HOME
    mkdir -p $YDKGEN_HOME/sdk/cpp/core/build
    cd $YDKGEN_HOME/sdk/cpp/core/build

    print_msg "Compiling with coverage"
    run_exec_test ${CMAKE_BIN} -DCOVERAGE=True ..
    run_exec_test make
    sudo make install
}

function run_cpp_core_test {
    print_msg "Running cpp core test"
    cd $YDKGEN_HOME/sdk/cpp/core/build
    make test
    local status=$?
    if [ $status -ne 0 ]; then
        # If the tests fail, try to run them in verbose mode to get more details
        ./tests/ydk_core_test -d yes
        MSG_COLOR=$RED
        print_msg "Exiting 'run_cpp_core_test' with status=$status"
        exit $status
    fi
    cd $YDKGEN_HOME
}

function install_cpp_ydktest_bundle {
    print_msg "Generating ydktest bundle for C++"
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydktest-cpp.json --cpp
    cd gen-api/cpp/ydktest-bundle/build
    run_exec_test make
    sudo make install
    cd -
}

function build_gnmi_cpp_core_library {
    print_msg "Building core gnmi library"
    cd $YDKGEN_HOME/sdk/cpp/gnmi
    mkdir -p build
    cd build
    run_exec_test ${CMAKE_BIN} -DCOVERAGE=True ..
    run_exec_test make
    sudo make install
    cd $YDKGEN_HOME
}

function build_and_run_cpp_gnmi_tests {
    print_msg "Building gnmi tests"
    cd $YDKGEN_HOME/sdk/cpp/gnmi/tests
    mkdir -p build
    cd build
    run_exec_test ${CMAKE_BIN} -DCOVERAGE=True ..
    run_exec_test make

    start_gnmi_server

    cd $YDKGEN_HOME/sdk/cpp/gnmi/tests/build
    run_exec_test ./ydk_gnmi_test -d yes

    stop_gnmi_server

    collect_cpp_coverage
}

function run_cpp_gnmi_tests {
    if [[ $(uname) == "Linux" ]] ; then
        os_info=$(cat /etc/*-release)
        if [[ ${os_info} == *"fedora"* ]]; then
            export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$YDKGEN_HOME/grpc/libs/opt:$YDKGEN_HOME/protobuf-3.5.0/src/.libs:/usr/local/lib64
            print_msg "LD_LIBRARY_PATH is set to: $LD_LIBRARY_PATH"
        fi
    fi

    build_gnmi_cpp_core_library
    build_and_run_cpp_gnmi_tests
}

function collect_cpp_coverage {
    print_msg "Collecting coverage for C++"
    cd ${YDKGEN_HOME}/sdk/cpp/core/build
    lcov --directory . --capture --output-file coverage.info &> /dev/null # capture coverage info
    lcov --remove coverage.info '/usr/*' '/Applications/*' '/opt/*' '*/json.hpp' '*/catch.hpp' '*/network_topology.cpp' '*/spdlog/*' --output-file coverage.info # filter out system
    lcov --list coverage.info #debug info
    print_msg "Moving cpp coverage to ${YDKGEN_HOME}"
    cp coverage.info ${YDKGEN_HOME}
}

function start_gnmi_server {
    current_dir="$(pwd)"
    cd $YDKGEN_HOME/test/gnmi_server
    if [ ! -x ./build/gnmi_server ]; then
        print_msg "Building YDK gNMI server"
        mkdir -p build && cd build
        ${CMAKE_BIN} .. && make
    fi

    print_msg "Starting YDK gNMI server"
    cd $YDKGEN_HOME/test/gnmi_server/build
    ./gnmi_server &
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Could not start YDK gNMI server"
        exit $status
    fi
    cd $current_dir
}

function stop_gnmi_server {
    print_msg "Stopping YDK gNMI server"
    pkill -f gnmi_server
}

######################################################################
# Go core and ydktest bundle installation functions
######################################################################

function install_go_core {
    print_msg "Installing go core"
    cd $YDKGEN_HOME

    mkdir -p $YDKGEN_HOME/golang/src/github.com/CiscoDevNet/ydk-go/ydk
    cp -r sdk/go/core/ydk/* $YDKGEN_HOME/golang/src/github.com/CiscoDevNet/ydk-go/ydk/
}

function run_go_bundle_tests {
    print_msg "Generating/installing go sanity bundle tests"
    # TODO: go get
    cd $YDKGEN_HOME
    run_test  generate.py --bundle profiles/test/ydktest-cpp.json --go
    cp -r gen-api/go/ydktest-bundle/ydk/* $YDKGEN_HOME/golang/src/github.com/CiscoDevNet/ydk-go/ydk/

    run_go_tests
}

function run_go_tests {
    print_msg "Running go tests"
    run_go_samples
    run_go_sanity_tests
}

function run_go_samples {
    print_msg "Running go samples"

    export CXX=/usr/bin/c++
    export CC=/usr/bin/cc

    print_msg "CC: ${CC}"
    print_msg "CXX: ${CXX}"

    cd $YDKGEN_HOME/sdk/go/core/samples
    run_exec_test go run cgo_path/cgo_path.go
    run_exec_test go run bgp_create/bgp_create.go -device ssh://admin:admin@localhost:12022
    run_exec_test go run bgp_read/bgp_read.go -device ssh://admin:admin@localhost:12022
    run_exec_test go run bgp_delete/bgp_delete.go -device ssh://admin:admin@localhost:12022 -v
    cd -
}

function run_go_sanity_tests {
    print_msg "Running go sanity tests"
    cd $YDKGEN_HOME/sdk/go/core/tests
    run_exec_test go test -race -coverpkg="github.com/CiscoDevNet/ydk-go/ydk/providers","github.com/CiscoDevNet/ydk-go/ydk/services","github.com/CiscoDevNet/ydk-go/ydk/types","github.com/CiscoDevNet/ydk-go/ydk/types/datastore","github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format","github.com/CiscoDevNet/ydk-go/ydk/types/protocol","github.com/CiscoDevNet/ydk-go/ydk/types/yfilter","github.com/CiscoDevNet/ydk-go/ydk/types/ytype","github.com/CiscoDevNet/ydk-go/ydk","github.com/CiscoDevNet/ydk-go/ydk/path" -coverprofile=coverage.txt -covermode=atomic
    print_msg "Moving go coverage to ${YDKGEN_HOME}"
    mv coverage.txt ${YDKGEN_HOME}
    cd -
}

######################################################################
# Python core and ydktest bundle installation functions
######################################################################

function install_py_core {
    print_msg "Building and installing Python core package"
    cd $YDKGEN_HOME/sdk/python/core
    export YDK_COVERAGE=
    ${PYTHON_BIN} setup.py sdist
    ${PIP_BIN} install dist/ydk*.tar.gz
    cd $YDKGEN_HOME
}

function install_py_ydktest_bundle {
    print_msg "Building and installing Python ydktest bundle"
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydktest-cpp.json
    pip_check_install gen-api/python/ydktest-bundle/dist/ydk*.tar.gz
}

function build_and_run_python_gnmi_tests {
    build_python_gnmi_package
    run_python_gnmi_tests
}

function build_python_gnmi_package {
    print_msg "Installing gNMI package for Python"

    cd $YDKGEN_HOME/sdk/python/gnmi
    ${PYTHON_BIN} setup.py sdist
    ${PIP_BIN} install dist/ydk*.tar.gz
}

function run_python_gnmi_tests {
    print_msg "Runing Python gNMI tests"

    start_gnmi_server

    cd $YDKGEN_HOME/sdk/python/gnmi/tests
    run_test test_gnmi_session.py
    run_test test_gnmi_crud.py
    run_test test_gnmi_service.py < $YDKGEN_HOME/test/gnmi_subscribe_poll_input.txt

    stop_gnmi_server
}

########################## EXECUTION STARTS HERE #############################
#

######################################
# Set up env

export YDKGEN_HOME="$(pwd)"

PYTHON_VERSION=""
args=$(getopt p:d $*)
set -- $args
PYTHON_VERSION=${2}
PYTHON_BIN=python${PYTHON_VERSION}

if [[ ${PYTHON_VERSION} = *"2"* ]]; then
    PIP_BIN=pip
elif [[ ${PYTHON_VERSION} = *"3.5"* ]]; then
    PIP_BIN=pip3
else
    PIP_BIN=pip${PYTHON_VERSION}
fi

os_type=$(uname)
print_msg "Running OS type: $os_type"
print_msg "YDKGEN_HOME is set to: ${YDKGEN_HOME}"
print_msg "Python location: $(which ${PYTHON_BIN})"
print_msg "Pip location: $(which ${PIP_BIN})"
$(${PYTHON_BIN} -V)

CMAKE_BIN=cmake
which cmake3
status=$?
if [[ ${status} == 0 ]] ; then
    CMAKE_BIN=cmake3
fi

init_py_env

######################################
# Install and run C++ core tests

install_cpp_core
run_cpp_core_test

install_cpp_ydktest_bundle
run_cpp_gnmi_tests

######################################
# Install and run Go tests

#init_go_env
#install_go_core
#run_go_bundle_tests

######################################
# Install and run Python tests
#
install_py_core
install_py_ydktest_bundle

build_and_run_python_gnmi_tests