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
# This file has been modified by Yan Gorelik, YDK Solutions.
# All modifications in original under CiscoDevNet domain
# introduced since October 2019 are copyrighted.
# All rights reserved under Apache License, Version 2.0.
# ------------------------------------------------------------------
#
# Script for running YDK gNMI tests on travis-ci.org
#
# gnmi_tests.sh
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
    echo -e "${MSG_COLOR}*** $(date): gnmi_tests.sh | $* ${NOCOLOR}"
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

function run_test_no_coverage {
    print_msg "Executing: ${PYTHON_BIN} $*"
    ${PYTHON_BIN} $*
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Exiting '${PYTHON_BIN} $*' with status=$status"
        exit $status
    fi
    return $status
}

function run_test {
    if [[ $(command -v coverage) && $run_with_coverage ]]; then
        print_msg "Executing with coverage: $*"
        coverage run --omit=/usr/* --branch --parallel-mode $* > /dev/null
        local status=$?
        print_msg "Returned status is ${status}"
        if [ $status -ne 0 ]; then
            MSG_COLOR=$RED
            print_msg "Exiting 'coverage run $*' with status=$status"
            exit $status
        fi
        return $status
    fi
    run_test_no_coverage $*
    local status=$?
    return $status
}

function pip_check_install {
    if [[ $(uname) == "Linux" ]] && [[ ${os_info} == *"fedora"* && ${PYTHON_VERSION} == "2"* ]] ; then
        print_msg "Custom pip install of $* for CentOS"
        ${PIP_BIN} install --install-option="--install-purelib=/usr/lib64/python${PYTHON_VERSION}/site-packages" --no-deps $*
    else
        ${PIP_BIN} install --no-deps $*
    fi
}

######################################################################
# Environment setup-teardown functions
######################################################################

function check_python_installation {

  if [[ ${os_type} == "Darwin" ]] ; then
    PYTHON_VERSION=3
    PYTHON_BIN=python3
    PIP_BIN=pip3
  else
    PYTHON_BIN=python${PYTHON_VERSION}
    if [[ ${PYTHON_VERSION} = "2"* ]]; then
      PIP_BIN=pip
    elif [[ ${PYTHON_VERSION} = "3.5"* ]]; then
      PIP_BIN=pip3
    else
      PIP_BIN=pip${PYTHON_VERSION}
    fi
  fi

  if [[ ! $run_with_coverage ]]; then
    if [[ -z ${PYTHON_VENV} ]]; then
      PYTHON_VENV=${HOME}/venv
      print_msg "Python virtual environment location is set to ${PYTHON_VENV}"
    fi
    if [[ ! -d ${PYTHON_VENV} ]]; then
      print_msg "Creating Python3 virtual environment in ${PYTHON_VENV}"
      run_cmd python3 -m venv ${PYTHON_VENV}
    fi
    run_cmd source ${PYTHON_VENV}/bin/activate
  fi

  print_msg "Checking installation of ${PYTHON_BIN}"
  ${PYTHON_BIN} -V
  status=$?
  if [ $status -ne 0 ]; then
    MSG_COLOR=$RED
    print_msg "Could not locate ${PYTHON_BIN}"
    exit $status
  fi
  print_msg "Checking installation of ${PIP_BIN}"
  ${PIP_BIN} -V
  status=$?
  if [ $status -ne 0 ]; then
    MSG_COLOR=$RED
    print_msg "Could not locate ${PIP_BIN}"
    exit $status
  fi
  print_msg "Python location: $(which ${PYTHON_BIN})"
  print_msg "Pip location: $(which ${PIP_BIN})"
}

function init_py_env {
  check_python_installation
  print_msg "Initializing Python requirements"
  ${PIP_BIN} install -r requirements.txt
  if [[ $run_with_coverage ]] ; then
    ${PIP_BIN} install coverage
  fi
}

function init_go_env {
    print_msg "Initializing Go environment"

    if [[ $(uname) == "Darwin" ]]; then
        if [[ $GOPATH. == "." ]]; then
            export GOPATH="$HOME/go"
        fi
        print_msg "GOROOT: $GOROOT"
        print_msg "GOPATH: $GOPATH"
    else
        if [[ $GOROOT. == "." ]]; then
            export GOROOT=/usr/local/go
            print_msg "Setting GOROOT to $GOROOT"
        else
            print_msg "GOROOT: $GOROOT"
        fi
        export PATH=$GOROOT/bin:$PATH

        if [[ $GOPATH. == "." ]]; then
            export GOPATH="$HOME/go"
            mkdir -p $GOPATH
            print_msg "Setting GOPATH to $GOPATH"
        else
            print_msg "GOPATH: $GOPATH"
        fi
    fi
    go_version=$(echo `go version` | awk '{ print $3 }' | cut -d 'o' -f 2)
    print_msg "Current Go version is $go_version"

    if [ ! -d $GOPATH/src/github.com/stretchr/testify ]; then
        go get github.com/stretchr/testify
        go get github.com/davecgh/go-spew/spew
        go get github.com/pmezard/go-difflib/difflib
        go get gopkg.in/yaml.v3
        cd $GOPATH/src/github.com/stretchr/testify
        git checkout tags/v1.6.1
        cd -
    fi

    export CGO_ENABLED=1
    export CGO_LDFLAGS_ALLOW="-fprofile-arcs|-ftest-coverage|--coverage"
    if [[ $go_version > "1.11." ]]; then
        go env -w GO111MODULE=off
    fi
}

######################################################################
# C++ Core and bundle installation functions
######################################################################

function install_cpp_core {
    print_msg "Installing C++ core library"

    cd $YDKGEN_HOME
    mkdir -p $YDKGEN_HOME/sdk/cpp/core/build
    cd $YDKGEN_HOME/sdk/cpp/core/build

    if [[ $run_with_coverage ]] ; then
      print_msg "Compiling with coverage"
      run_cmd ${CMAKE_BIN} -DCOVERAGE=True ..
    else
      run_cmd ${CMAKE_BIN} ..
    fi
    run_cmd make &> /dev/null
    sudo make install
}

function run_cpp_core_test {
    print_msg "Running C++ core tests"
    cd $YDKGEN_HOME/sdk/cpp/core/build

    #make test

    ./tests/ydk_core_test -d yes
    local status=$?
    if [ $status -ne 0 ]; then
        # If the tests fail, try to run them in verbose mode to get more details
        #./tests/ydk_core_test -d yes
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
    run_cmd make &> /dev/null
    sudo make install
    cd -
}

function build_gnmi_cpp_core_library {
    print_msg "Building C++ core gnmi library"
    cd $YDKGEN_HOME/sdk/cpp/gnmi
    mkdir -p build
    cd build
    if [[ $run_with_coverage ]] ; then
      run_cmd ${CMAKE_BIN} -DCOVERAGE=True ..
    else
      run_cmd ${CMAKE_BIN} ..
    fi
    run_cmd make &> /dev/null
    sudo make install
    cd $YDKGEN_HOME
}

function build_and_run_cpp_gnmi_tests {
    print_msg "Building gnmi tests"
    cd $YDKGEN_HOME/sdk/cpp/gnmi/tests
    mkdir -p build
    cd build
    if [[ $run_with_coverage ]] ; then
      run_cmd ${CMAKE_BIN} -DCOVERAGE=True ..
    else
      run_cmd ${CMAKE_BIN} ..
    fi
    run_cmd make &> /dev/null

    cd $YDKGEN_HOME/sdk/cpp/gnmi/tests/build
    run_cmd ./ydk_gnmi_test -d yes

    collect_cpp_coverage
}

function run_cpp_gnmi_tests {
    build_gnmi_cpp_core_library

    start_gnmi_server

    build_and_run_cpp_gnmi_tests

    if [[ $(uname) == "Linux" ]]; then
       run_cpp_gnmi_memcheck_tests
    fi

    stop_gnmi_server
}

function collect_cpp_coverage {
  if [[ $run_with_coverage ]] ; then
    print_msg "Collecting coverage for C++"
    cd ${YDKGEN_HOME}/sdk/cpp/gnmi/build
    lcov --directory . --capture --output-file coverage.info &> /dev/null # capture coverage info
    lcov --remove coverage.info '/usr/*' '/Applications/*' '/opt/*' '*/json.hpp' '*/catch.hpp' '*/network_topology.cpp' '*/spdlog/*' --output-file coverage.info # filter out system
    lcov --list coverage.info #debug info
    cp coverage.info ${YDKGEN_HOME}
  fi
}

function run_cpp_gnmi_memcheck_tests {
    print_msg "Building gnmi sample tests"
    cd $YDKGEN_HOME/sdk/cpp/gnmi/samples
    mkdir -p build
    cd build
    run_cmd ${CMAKE_BIN} ..
    run_cmd make &> /dev/null

    print_msg "Running gnmi sample tests with memcheck"
    valgrind --leak-check=summary ./bgp_gnmi_subscribe ssh://admin:admin@127.0.0.1:50051
}

function start_gnmi_server {
    current_dir="$(pwd)"
    cd $YDKGEN_HOME/test/gnmi_server
    if [ ! -x ./build/gnmi_server ]; then
        print_msg "Building YDK gNMI server"
        mkdir -p build && cd build
        run_cmd ${CMAKE_BIN} ..
        run_cmd make &> /dev/null
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
    print_msg "Installing Go core packages"
    cd $YDKGEN_HOME
    run_test generate.py -i --core --go
}

function install_go_bundle {
    print_msg "Generating/installing Go 'ysanity' package"
    cd $YDKGEN_HOME
    run_test  generate.py -i --bundle profiles/test/ydktest-cpp.json --go
}

function install_go_gnmi {
    print_msg "Installing Go gNMI package"
    cd $YDKGEN_HOME
    run_test generate.py -i --service profiles/services/gnmi-0.4.0.json --go
}

function run_go_gnmi_tests {
    start_gnmi_server

    print_msg "Running Go gNMI tests"

    cd $YDKGEN_HOME/sdk/go/gnmi/tests
    run_cmd go test

    run_go_gnmi_samples

    stop_gnmi_server

    cd $YDKGEN_HOME
}

function run_go_gnmi_samples {
    print_msg "Running Go gNMI samples"

    cd $YDKGEN_HOME/sdk/go/gnmi/samples
    run_cmd go run service_subscribe_poll.go < $YDKGEN_HOME/test/gnmi_subscribe_poll_input.txt
    run_cmd go run session_subscribe_poll.go < $YDKGEN_HOME/test/gnmi_subscribe_poll_input.txt
}


######################################################################
# Python core and ydktest bundle installation functions
######################################################################

function install_py_core {
    print_msg "Building and installing Python core package"
    cd $YDKGEN_HOME/sdk/python/core
    if [[ $run_with_coverage ]] ; then
      export YDK_COVERAGE=1
    fi
    ${PYTHON_BIN} setup.py sdist
    ${PIP_BIN} install dist/ydk*.tar.gz

    print_msg "Verifying Python YDK core package installation"
    ${PYTHON_BIN} -c "from ydk.path import NetconfSession"
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Verification failed for the Python core package 'ydk_.so'"
        exit $status
    fi
}

function install_py_ydktest_bundle {
    print_msg "Building and installing Python ydktest bundle"
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydktest-cpp.json
    pip_check_install gen-api/python/ydktest-bundle/dist/ydk*.tar.gz

    print_msg "Verifying Python 'ydk-models-ydktest' bundle installation"
    ${PYTHON_BIN} -c "from ydk.models.ydktest import openconfig_bgp"
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Verification failed for the Python 'ydk-models-ydktest' bundle"
        exit $status
    fi
}

function build_and_run_python_gnmi_tests {
    build_python_gnmi_package
    run_python_gnmi_tests
}

function build_python_gnmi_package {
    print_msg "Installing gNMI package for Python"

    cd $YDKGEN_HOME
    run_test generate.py --service profiles/services/gnmi-0.4.0.json
    run_cmd ${PIP_BIN} install --no-deps gen-api/python/ydk-service-gnmi/dist/ydk*.tar.gz

    print_msg "Verifying Python gNMI package installation"
    ${PYTHON_BIN} -c "from ydk.gnmi.path import gNMISession"
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Verification failed for the Python gNMI package 'ydk_gnmi_.so'"
        exit $status
    fi
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
args=$(getopt p:d $*)
set -- $args
PYTHON_VERSION=${2}

######################################
# Set up env

os_type=$(uname)
if [[ ${os_type} == "Linux" ]] ; then
    os_info=$(cat /etc/*-release)
else
    os_info="darwin"
fi

print_msg "Running OS type: $os_type"
print_msg "OS info: $os_info"

if [[ ${os_type} == "Linux" && ${os_info} != *"trusty"* && ${os_info} != *"fedora"* ]] ; then
    run_with_coverage=1
fi

export YDKGEN_HOME="$(pwd)"

CMAKE_BIN=cmake
which cmake3
status=$?
if [[ ${status} == 0 ]] ; then
    CMAKE_BIN=cmake3
fi

if [[ $(uname) == "Linux" && ${os_info} == *"fedora"* ]] ; then
  if [[ $LD_LIBRARY_PATH != *"/usr/local/lib:/usr/local/lib64:/usr/lib64"* ]]; then
    export /usr/local/lib:/usr/local/lib64:/usr/lib64:$LD_LIBRARY_PATH
    print_msg "LD_LIBRARY_PATH is set to: $LD_LIBRARY_PATH"
  fi
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

init_go_env
install_go_core
install_go_bundle

install_go_gnmi
run_go_gnmi_tests

######################################
# Install and run Python tests
#
install_py_core
install_py_ydktest_bundle

build_and_run_python_gnmi_tests

######################################
# Cleanup and Combine coverage

cd $YDKGEN_HOME
find . -name '*gcda*'|xargs rm -f
find . -name '*gcno*'|xargs rm -f
find . -name '*gcov*'|xargs rm -f

if [[ $run_with_coverage ]] ; then
  print_msg "Combining C++, Python and Go coverage"
  coverage combine > /dev/null || echo "Coverage not combined"
fi
