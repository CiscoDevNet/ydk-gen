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

PY_GENERATE="python2"
PY_TEST="python3"

######################################################################
# Utility functions
######################################################################

function print_msg {
    echo -e "${RED}*** $(date) *** tests.sh | $1${NOCOLOR}"
}

function run_exec_test {
    $@
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    return $status
}

function run_test_no_coverage {
    print_msg "executing: $@"
    python $@
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    return $status
}

function run_test {
    coverage_found=`coverage --version &> /dev/null`
    print_msg "Coverage found: ${coverage_found}"
    if [[ $coverage_found != 0 ]]; then
        run_test_no_coverage $@
        local status=$?
        return $status
    fi
    print_msg "executing with coverage: $@"
    coverage run --source=ydkgen,sdk,generate --branch --parallel-mode $@ > /dev/null
    local status=$?
    print_msg "status is ${status}"
    if [ $status -ne 0 ]; then
        exit $status
    fi
    return $status
}

######################################################################
# Environment setup-teardown functions
######################################################################

function init_confd {
    cd $1
    print_msg "Initializing confd in $(pwd)"
    source $YDKGEN_HOME/../confd/confdrc
    run_exec_test make stop > /dev/null
    run_exec_test make clean > /dev/null
    run_exec_test make all > /dev/null
    run_exec_test make start
    cd -
}

function init_confd_ydktest {
    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/ydktest
}

function init_rest_server {
    print_msg "starting rest server"
    ./test/start_rest_server.sh
    print_msg "Rest server started"
}

function init_tcp_server {
    print_msg "starting tcp proxy server"
    ./test/tcp_proxy_server.py -b 12307 -c 2023 &> /dev/null &
    local status=$?
    if [ $status -ne 0 ]; then
        print_msg "Could not start tcp server"
        exit $status
    fi
}

function init_py_env {
    print_msg "Initializing python env"
    pip install -r requirements.txt coverage
}

function init_go_env {
    print_msg "Initializing Go env"

    print_msg "${GOPATH}"
    print_msg "${GOROOT}"

    export PATH=$PATH:$GOPATH/bin
    export PATH=$PATH:$GOROOT/bin

    cd $YDKGEN_HOME
    export GOPATH="`pwd`/golang":$GOPATH

    print_msg "new: ${GOPATH}"

    go get github.com/stretchr/testify
}

function teardown_env {
    print_msg "teardown_env"
    deactivate
    cd $YDKGEN_HOME && rm -rf gen_env test_env
}

######################################################################
# Core install / test functions
######################################################################

function install_test_cpp_core {
    print_msg "Installing / testing cpp core"
    install_cpp_core
    run_cpp_core_test
}

function install_cpp_core {
    print_msg "Installing cpp core"

    cd $YDKGEN_HOME
    mkdir -p $YDKGEN_HOME/sdk/cpp/core/build
    cd $YDKGEN_HOME/sdk/cpp/core/build
    
    cmake .. && sudo make install
}

function run_cpp_core_test {
    print_msg "Running cpp core test"

    make test
    local status=$?
    if [ $status -ne 0 ]; then
    # If the tests fail, try to run them in verbose to get more details for debug
        ./tests/ydk_core_test -d yes
        exit $status
    fi
    cd $YDKGEN_HOME
}

function install_go_core {
    print_msg "Installing go core"
    cd $YDKGEN_HOME

    mkdir -p $YDKGEN_HOME/golang/src/github.com/CiscoDevNet/ydk-go/ydk
    cp -r sdk/go/core/ydk/* $YDKGEN_HOME/golang/src/github.com/CiscoDevNet/ydk-go/ydk/
}

function install_py_core {
    print_msg "Installing py core"
    cd $YDKGEN_HOME
    cd $YDKGEN_HOME/sdk/python/core
    python setup.py sdist
    pip install dist/ydk*.tar.gz
    cd $YDKGEN_HOME
}

######################################################################
# C++ ydktest bundle install and test functions
######################################################################

function run_cpp_bundle_tests {
    print_msg "Generating and testing C++ bundle"

    cpp_sanity_ydktest_gen_install
    cpp_sanity_ydktest_test
    teardown_env
}

function generate_install_specified_cpp_bundle {
   bundle_profile=$1
   bundle_name=$2

   run_test generate.py --bundle $bundle_profile --cpp --generate-doc &> /dev/null
   cd gen-api/cpp/$2/build
   run_exec_test sudo make install
   cd -
}

function cpp_sanity_ydktest_gen_install {
    print_msg "Generating and installing C++ ydktest bundle"
    generate_install_specified_cpp_bundle profiles/test/ydktest-cpp.json ydktest-bundle

    print_msg "Generating and installing new C++ ydktest bundle"
    generate_install_specified_cpp_bundle profiles/test/ydktest-cpp-new.json ydktest_new-bundle
}

function cpp_sanity_ydktest_test {
    print_msg "Running cpp bundle tests"

    print_msg "Initializing ssh keys for key-based authentication"
    sudo mkdir -p /var/confd/homes/admin/.ssh
    sudo touch /var/confd/homes/admin/.ssh/authorized_keys
    sudo sh -c 'cat $YDKGEN_HOME/sdk/cpp/tests/ssh_host_rsa_key.pub >> /var/confd/homes/admin/.ssh/authorized_keys'

    print_msg "Building and running cpp bundle tests"
    mkdir -p $YDKGEN_HOME/sdk/cpp/tests/build && cd sdk/cpp/tests/build
    run_exec_test cmake ..
    run_exec_test make
    make test
    local status=$?
    if [ $status -ne 0 ]; then
    # If the tests fail, try to run them in verbose to get more details for  # debug
        ./ydk_bundle_test -d yes
        exit $status
    fi
}

function cpp_test_gen_test {
    print_msg "cpp_test_gen_test"

    cd $YDKGEN_HOME
    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/testgen/confd
    mkdir -p gen-api/cpp/models_test-bundle/ydk/models/models_test/test/build
    cd gen-api/cpp/models_test-bundle/ydk/models/models_test/test/build
    run_exec_test cmake ..
    run_exec_test make
    ctest --output-on-failure
}

function cpp_test_gen {
    print_msg "cpp_test_gen"

    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydk-models-test.json --generate-tests --cpp &> /dev/null
    cd gen-api/cpp/models_test-bundle/build/
    run_exec_test sudo make install

    # cpp_test_gen_test
}

######################################################################
# Go ydktest bundle install and test functions
######################################################################

function run_go_bundle_tests {
    print_msg "Generating/installing go sanity bundle tests"
    # TODO: go get
    cd $YDKGEN_HOME
    run_exec_test ./generate.py --bundle profiles/test/ydktest-cpp.json --go
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
    run_exec_test go run bgp_create/bgp_create.go
    run_exec_test go run bgp_read/bgp_read.go
    run_exec_test go run bgp_delete/bgp_delete.go
    cd -
}

function run_go_sanity_tests {
    print_msg "Running go sanity tests"
    cd $YDKGEN_HOME/sdk/go/core/tests
    run_exec_test go test
    cd -
}

######################################################################
# Python ydktest bundle install and test functions
######################################################################

function run_python_bundle_tests {
    py_sanity_ydktest
    py_sanity_deviation
    py_sanity_augmentation
    py_sanity_common_cache
    py_sanity_one_class_per_module
    teardown_env
}

#--------------------------
# Python ydktest bundle
#--------------------------

function py_sanity_ydktest {
    print_msg "Generating, installing and testing python ydktest bundle"

    py_sanity_ydktest_gen
    py_sanity_ydktest_install
    py_sanity_ydktest_test
}

function py_sanity_ydktest_gen {
    print_msg "Generating python ydk core and ydktest bundle"

    cd $YDKGEN_HOME

    print_msg "py_sanity_ydktest_gen: testing grouping as class"
    run_test generate.py --bundle profiles/test/ydktest.json --python --groupings-as-class

    print_msg "py_sanity_ydktest_gen: testing bundle and documentation generation"
    run_test generate.py --bundle profiles/test/ydktest-cpp.json --python --generate-doc &> /dev/null

    print_msg "py_sanity_ydktest_gen: testing core and documentation generation"
    run_test generate.py --core
}

function py_sanity_ydktest_install {
    print_msg "py_sanity_ydktest_install"
    print_msg "Installing"
    cd $YDKGEN_HOME
    pip install gen-api/python/ydktest-bundle/dist/ydk*.tar.gz
}

function py_sanity_ydktest_test {
    print_msg "py_sanity_ydktest_test"

    cd $YDKGEN_HOME && cp -r gen-api/python/ydktest-bundle/ydk/models/* sdk/python/core/ydk/models

    print_msg "running import tests"
    run_test gen-api/python/ydktest-bundle/ydk/models/ydktest/test/import_tests.py

    export OLDPYTHONPATH=$PYTHONPATH
    export PYTHONPATH=$PYTHONPATH:sdk/python/core

    print_msg "Copy cpp-wrapper to sdk directory"
    cd sdk/python/ydk/ && python setup.py build
    cp build/lib*/*.so .

    run_test sdk/python/core/tests/test_sanity_codec.py

    py_sanity_ydktest_test_tcp
    py_sanity_ydktest_test_netconf_ssh

    export PYTHONPATH=$OLDPYTHONPATH

    cd $YDKGEN_HOME
}

function py_sanity_ydktest_test_netconf_ssh {
    print_msg "py_sanity_ydktest_test_netconf_ssh"

    run_test sdk/python/core/tests/test_netconf_operations.py
    run_test sdk/python/core/tests/test_opendaylight.py
    run_test sdk/python/core/tests/test_restconf_provider.py
    run_test sdk/python/core/tests/test_sanity_delete.py
    run_test sdk/python/core/tests/test_sanity_errors.py
    run_test sdk/python/core/tests/test_sanity_filter_read.py
    run_test sdk/python/core/tests/test_sanity_filters.py
    run_test sdk/python/core/tests/test_sanity_levels.py
    run_test sdk/python/core/tests/test_sanity_netconf.py
    run_test sdk/python/core/tests/test_sanity_path.py
    run_test sdk/python/core/tests/test_netconf_provider.py
    run_test sdk/python/core/tests/test_sanity_service_errors.py
    run_test sdk/python/core/tests/test_sanity_type_mismatch_errors.py
    run_test sdk/python/core/tests/test_sanity_types.py
    run_test_no_coverage sdk/python/core/tests/test_sanity_executor_rpc.py

    run_test sdk/python/core/tests/test_netconf_operations.py --non-demand
    run_test sdk/python/core/tests/test_sanity_delete.py --non-demand
    run_test sdk/python/core/tests/test_sanity_errors.py --non-demand
    run_test sdk/python/core/tests/test_sanity_filter_read.py --non-demand
    run_test sdk/python/core/tests/test_sanity_filters.py --non-demand
    run_test sdk/python/core/tests/test_sanity_levels.py --non-demand
    run_test sdk/python/core/tests/test_sanity_netconf.py --non-demand
    run_test sdk/python/core/tests/test_sanity_path.py --non-demand
    run_test sdk/python/core/tests/test_netconf_provider.py --non-demand
    run_test sdk/python/core/tests/test_sanity_service_errors.py --non-demand
    run_test sdk/python/core/tests/test_sanity_type_mismatch_errors.py --non-demand
    run_test sdk/python/core/tests/test_sanity_types.py --non-demand
    run_test_no_coverage sdk/python/core/tests/test_sanity_executor_rpc.py --non-demand
}

function py_sanity_ydktest_test_tcp {    
    run_test sdk/python/core/tests/test_sanity_netconf.py tcp://admin:admin@127.0.0.1:12307
    run_test sdk/python/core/tests/test_sanity_netconf.py tcp://admin:admin@127.0.0.1:12307 --non-demand
}

#--------------------------
# Python deviation bundle
#--------------------------

function py_sanity_deviation {
    print_msg "py_sanity_deviation"

    py_sanity_deviation_ydktest_gen
    py_sanity_deviation_ydktest_install
    py_sanity_deviation_ydktest_test

    py_sanity_deviation_bgp_gen
    py_sanity_deviation_bgp_install
    py_sanity_deviation_bgp_test
}

function py_sanity_deviation_ydktest_gen {
    print_msg "py_sanity_deviation_ydktest_gen"

    rm -rf gen-api/python/*
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydktest-cpp.json --python
}

function py_sanity_deviation_ydktest_install {
    print_msg "py_sanity_deviation_ydktest_install"

    pip uninstall ydk-models-ydktest -y && pip install gen-api/python/ydktest-bundle/dist/ydk*.tar.gz
}

function py_sanity_deviation_ydktest_test {
    print_msg "py_sanity_deviation_ydktest_test"

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/deviation
    run_test sdk/python/core/tests/test_sanity_deviation.py
    run_test sdk/python/core/tests/test_sanity_deviation.py --non-demand
}

function py_sanity_deviation_bgp_gen {
    print_msg "py_sanity_deviation_bgp_gen"

    rm -rf gen-api/python/*
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/deviation.json --verbose
}

function py_sanity_deviation_bgp_install {
    print_msg "py_sanity_deviation_bgp_install"

    cd $YDKGEN_HOME
    pip install gen-api/python/deviation-bundle/dist/*.tar.gz
}

function py_sanity_deviation_bgp_test {
    print_msg "py_sanity_deviation_bgp_test"

    run_test sdk/python/core/tests/test_sanity_deviation_bgp.py
    run_test sdk/python/core/tests/test_sanity_deviation_bgp.py --non-demand
}

#--------------------------
# Python augmentation bundle
#--------------------------
function py_sanity_augmentation {
    print_msg "py_sanity_augmentation"

    py_sanity_augmentation_gen
    py_sanity_augmentation_install
    py_sanity_augmentation_test
}

function py_sanity_augmentation_gen {
    print_msg "py_sanity_augmentation_gen"

    cd $YDKGEN_HOME && rm -rf gen-api/python/*
    run_test generate.py --core
    run_test generate.py --bundle profiles/test/ydktest-augmentation.json
}

function py_sanity_augmentation_install {
    print_msg "py_sanity_augmentation_install"

    cd $YDKGEN_HOME
    pip uninstall ydk -y
    pip install gen-api/python/ydk/dist/ydk*.tar.gz
    pip install gen-api/python/augmentation-bundle/dist/*.tar.gz
}

function py_sanity_augmentation_test {
    print_msg "py_sanity_augmentation_test"

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/augmentation

    run_test sdk/python/core/tests/test_sanity_augmentation.py
    run_test sdk/python/core/tests/test_sanity_augmentation.py --non-demand
    run_test sdk/python/core/tests/test_on_demand.py
}

function py_sanity_common_cache {
    print_msg "py_sanity_common_cache"

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/deviation
    run_test sdk/python/core/tests/test_sanity_deviation.py --common-cache
    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/augmentation
    run_test sdk/python/core/tests/test_sanity_augmentation.py --common-cache
    
    run_test sdk/python/core/tests/test_sanity_levels.py --common-cache
    run_test sdk/python/core/tests/test_sanity_types.py --common-cache
}

function py_sanity_one_class_per_module {
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydktest.json -o
    pip install gen-api/python/ydktest-bundle/dist/ydktest*.tar.gz
    run_test sdk/python/core/tests/test_sanity_levels.py
    run_test sdk/python/core/tests/test_sanity_types.py
}

#-------------------------------------
# Python generated model tests bundle
#-------------------------------------

function test_gen_tests {
    print_msg "test_gen_tests"

    cd $YDKGEN_HOME
    git clone https://github.com/psykokwak4/ydk-test-yang.git sdk/cpp/core/tests/confd/testgen

    py_test_gen
    cpp_test_gen
}

function py_test_gen_test {
    print_msg "py_test_gen_test"

    cd $YDKGEN_HOME
    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/testgen/confd
    cd gen-api/python/models_test-bundle/ydk/models/models_test/test/
    python import_tests.py
    cd models_test/
    python -m unittest discover
}

function py_test_gen {
    print_msg "py_test_gen"

    cd $YDKGEN_HOME
    run_test generate.py --core --python
    run_test generate.py --bundle profiles/test/ydk-models-test.json  --generate-tests --python &> /dev/null
    pip install gen-api/python/ydk/dist/ydk*.tar.gz
    pip install gen-api/python/models_test-bundle/dist/ydk*.tar.gz

    # py_test_gen_test
}

########################## EXECUTION STARTS HERE #############################

######################################
# Set up env
######################################
export YDKGEN_HOME="`pwd`"

init_py_env
init_confd_ydktest
init_rest_server
init_tcp_server
init_go_env

######################################
# Install/test core
######################################
install_test_cpp_core
install_go_core
install_py_core

######################################
# Install/test bundles
######################################
run_cpp_bundle_tests
run_go_bundle_tests
#run_python_bundle_tests
# test_gen_tests

#cd $YDKGEN_HOME
#print_msg "gathering cpp coverage"
#print_msg "combining python coverage"
#coverage combine
