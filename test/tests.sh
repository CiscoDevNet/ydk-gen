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
    python $@
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    return $status
}

function run_test {
    coverage run --source=ydkgen,sdk,generate --branch --parallel-mode $@ > /dev/null
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    return $status
}

function init_env {
    print_msg "init_env"

    export YDKGEN_HOME=`pwd` && cd $YDKGEN_HOME

    PY_GENERATE="$1"
    PY_TEST="$2"

    YDK_GEN_ENV=`which $PY_GENERATE`
    YDK_TEST_ENV=`which $PY_TEST`

    print_msg "init_env: Generating interpreter $YDK_GEN_ENV"
    print_msg "init_env: Testing interpreter $YDK_TEST_ENV"

    virtualenv -p $PY_GENERATE gen_env
    virtualenv -p $PY_TEST test_env

    source test_env/bin/activate
    pip install -r requirements.txt coverage > /dev/null

    source gen_env/bin/activate
    pip install -r requirements.txt coverage > /dev/null
}

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

function init_rest_server {
    cd $YDKGEN_HOME/test
    print_msg "starting rest server"
    rest_server_id=$(./start_rest_server.sh)
    cd -
}

function py_sanity_ydktest {
    print_msg "Generating, installing and testing python ydktest bundle"

    py_sanity_ydktest_gen
    py_sanity_ydktest_install
    py_sanity_ydktest_test
}

function py_sanity_ydktest_gen {
    print_msg "Generating python ydk core and ydktest bundle"

    cd $YDKGEN_HOME && source gen_env/bin/activate

    print_msg "py_sanity_ydktest_gen: testing grouping as class"
    run_test generate.py --bundle profiles/test/ydktest.json --python --groupings-as-class

    print_msg "py_sanity_ydktest_gen: testing bundle and documentation generation"
    run_test generate.py --bundle profiles/test/ydktest.json --python --generate-doc

    print_msg "py_sanity_ydktest_gen: testing core and documentation generation"
    run_test generate.py --core
}

function py_sanity_ydktest_install {
    print_msg "py_sanity_ydktest_install"
    print_msg "Installing"
    cd $YDKGEN_HOME && source test_env/bin/activate
    pip install gen-api/python/ydk/dist/ydk*.tar.gz
    pip install gen-api/python/ydktest-bundle/dist/ydk*.tar.gz
}

function py_sanity_ydktest_test {
    print_msg "py_sanity_ydktest_test"

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/ydktest

    cd $YDKGEN_HOME && cp -r gen-api/python/ydktest-bundle/ydk/models/* sdk/python/core/ydk/models

    run_test gen-api/python/ydktest-bundle/ydk/tests/import_tests.py

    print_msg "deactivate virtualenv to gather coverage"
    deactivate
    pip install -r requirements.txt
    pip install coverage
    export PYTHONPATH=$PYTHONPATH:sdk/python/core

    print_msg "Copy cpp-wrapper to sdk directory"
    cd gen-api/python/ydk/ && python setup.py build && cd -
    cp gen-api/python/ydk/build/lib*/*.so sdk/python/core

    run_test sdk/python/core/tests/test_sanity_codec.py

    py_sanity_ydktest_test_ncclient

    git checkout .
    export PYTHONPATH=

    print_msg "reactivate virtualenv"
    source test_env/bin/activate
}

function py_sanity_ydktest_test_ncclient {
    print_msg "py_sanity_ydktest_test_ncclient"
    run_test sdk/python/core/tests/test_sanity_types.py
    run_test sdk/python/core/tests/test_sanity_errors.py
    run_test sdk/python/core/tests/test_sanity_filters.py
    run_test sdk/python/core/tests/test_sanity_levels.py
    run_test sdk/python/core/tests/test_sanity_filter_read.py
    run_test sdk/python/core/tests/test_sanity_netconf.py
    run_test sdk/python/core/tests/test_sanity_rpc.py
#    run_test sdk/python/core/tests/test_sanity_path.py
    run_test sdk/python/core/tests/test_sanity_delete.py
    run_test sdk/python/core/tests/test_sanity_service_errors.py
}

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
    cd $YDKGEN_HOME && source gen_env/bin/activate
    run_test generate.py --bundle profiles/test/ydktest.json --python
}

function py_sanity_deviation_ydktest_install {
    print_msg "py_sanity_deviation_ydktest_install"

    source test_env/bin/activate
    pip uninstall ydk-models-ydktest -y && pip install gen-api/python/ydktest-bundle/dist/ydk*.tar.gz
}

function py_sanity_deviation_ydktest_test {
    print_msg "py_sanity_deviation_ydktest_test"

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/deviation
    run_test sdk/python/core/tests/test_sanity_deviation.py
}

function py_sanity_deviation_bgp_gen {
    print_msg "py_sanity_deviation_bgp_gen"

    rm -rf gen-api/python/*
    cd $YDKGEN_HOME && source gen_env/bin/activate
    run_test generate.py --bundle profiles/test/deviation.json --verbose
}

function py_sanity_deviation_bgp_install {
    print_msg "py_sanity_deviation_bgp_install"

    cd $YDKGEN_HOME && source test_env/bin/activate
    pip install gen-api/python/deviation-bundle/dist/*.tar.gz
}

function py_sanity_deviation_bgp_test {
    print_msg "py_sanity_deviation_bgp_test"

    run_test sdk/python/core/tests/test_sanity_deviation_bgp.py
}

function py_sanity_augmentation {
    print_msg "py_sanity_augmentation"

    py_sanity_augmentation_gen
    py_sanity_augmentation_install
    py_sanity_augmentation_test
}

function py_sanity_augmentation_gen {
    print_msg "py_sanity_augmentation_gen"

    cd $YDKGEN_HOME && rm -rf gen-api/python/*
    source gen_env/bin/activate
    run_test generate.py --core
    run_test generate.py --bundle profiles/test-augmentation/ietf.json
    run_test generate.py --bundle profiles/test-augmentation/ydktest-aug-ietf-1.json
    run_test generate.py --bundle profiles/test-augmentation/ydktest-aug-ietf-2.json
    run_test generate.py --bundle profiles/test-augmentation/ydktest-aug-ietf-4.json
}

function py_sanity_augmentation_install {
    print_msg "py_sanity_augmentation_install"

    cd $YDKGEN_HOME && source test_env/bin/activate
    pip uninstall ydk -y
    pip install gen-api/python/ydk/dist/*.tar.gz \
                gen-api/python/ietf*/dist/*.tar.gz \
                gen-api/python/ydktest_aug_ietf_1*/dist/ydk*.tar.gz \
                gen-api/python/ydktest_aug_ietf_2*/dist/ydk*.tar.gz \
                gen-api/python/ydktest_aug_ietf_4*/dist/ydk*.tar.gz
}

function py_sanity_augmentation_test {
    print_msg "py_sanity_augmentation_test"

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/augmentation
    run_test sdk/python/core/tests/test_sanity_bundle_aug.py
}

function cpp_sanity_core_gen_install {
    print_msg "cpp_sanity_core_gen_install"

    cd $YDKGEN_HOME && source gen_env/bin/activate
    cd $YDKGEN_HOME/sdk/cpp/core
    mkdir -p build && cd build
    run_exec_test cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..
    run_exec_test make install
    cd $YDKGEN_HOME
}

function cpp_sanity_core_test {
    print_msg "Running cpp core test"

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/ydktest
    cd $YDKGEN_HOME/sdk/cpp/core/build
    run_exec_test make test
    cd $YDKGEN_HOME
}

function cpp_sanity_ydktest {
    print_msg "Generating and testing bundle"

    cpp_sanity_ydktest_gen_install
    cpp_sanity_ydktest_test
}

function cpp_sanity_ydktest_gen_install {
    print_msg "Generating and installing ydktest bundle"

    cd $YDKGEN_HOME && source gen_env/bin/activate
    run_test generate.py --bundle profiles/test/ydktest-cpp.json --cpp --generate-doc
    cd gen-api/cpp/ydktest-bundle/build
    run_exec_test make install
    cd -
}

function cpp_sanity_ydktest_test {
    print_msg "Running cpp bundle tests"

    mkdir -p $YDKGEN_HOME/sdk/cpp/tests/build && cd sdk/cpp/tests/build
    run_exec_test cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..
    run_exec_test make
    make test
    local status=$?
    if [ $status -ne 0 ]; then
    # If the tests fail, try to run them individually to get more details for debug
        for test_name in $(ls test*);
        do
            echo "Running $test_name"
            ./$test_name -l all > output
            local test_status=$?
            if [ $test_status -ne 0 ]; then
                cat output
                exit $test_status
            fi
        done
        exit $status
    fi
}

function teardown_env {
    print_msg "teardown_env"
    deactivate
    cd $YDKGEN_HOME && rm -rf gen_env test_env
}

function py_tests {
    GEN_ENV="python3"
    TEST_ENV="python3"

    init_env $GEN_ENV $TEST_ENV
    
    # Install ydk-cpp core before starting tests
    cpp_sanity_core_gen_install
    
    py_sanity_ydktest
    py_sanity_deviation
    py_sanity_augmentation
    teardown_env
}

function cpp_tests {
    init_env "python" "python"
    cpp_sanity_core_test
    cpp_sanity_ydktest
    teardown_env
}

function cpp_test_gen_test {
    print_msg "cpp_test_gen_test"

    cd $YDKGEN_HOME
    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/testgen/confd
    mkdir -p gen-api/cpp/models_test-bundle/ydk/tests/build
    cd gen-api/cpp/models_test-bundle/ydk/tests/build
    run_exec_test cmake -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++ ..
    run_exec_test make
    ctest --output-on-failure
}

function cpp_test_gen {
    print_msg "cpp_test_gen"

    cd $YDKGEN_HOME
    cpp_sanity_core_gen_install
    run_test generate.py --bundle profiles/test/ydk-models-test.json --generate-tests --cpp
    cd gen-api/cpp/models_test-bundle/build/
    run_exec_test make install

    # cpp_test_gen_test
}

function py_test_gen_test {
    print_msg "py_test_gen_test"

    cd $YDKGEN_HOME
    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/testgen/confd
    cd gen-api/python/models_test-bundle/ydk/tests/models_test/
    python -m unittest discover
}

function py_test_gen {
    print_msg "py_test_gen"

    cd $YDKGEN_HOME
    run_test generate.py --core --python
    run_test generate.py --bundle profiles/test/ydk-models-test.json  --generate-tests --python
    pip install gen-api/python/ydk/dist/ydk*.tar.gz
    pip install gen-api/python/models_test-bundle/dist/ydk*.tar.gz

    # py_test_gen_test
}

function test_gen_tests {
    print_msg "test_gen_tests"

    init_env "python" "python"
    cd $YDKGEN_HOME && source gen_env/bin/activate
    git clone https://github.com/abhikeshav/ydk-test-yang.git sdk/cpp/core/tests/confd/testgen

    py_test_gen
    cpp_test_gen
}

########################## EXECUTION STARTS HERE #############################

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/..

py_tests
init_rest_server
cpp_tests
test_gen_tests
cd $YDKGEN_HOME
print_msg "gathering cpp coverage"
print_msg "combining python coverage"
coverage combine
