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


ROOT=/root
CONFD=/root/confd
CONFD_TARGET_DIR=$CONFD/etc/confd
FXS_DIR=$CONFD/src/confd/yang/ydktest/fxs/
YDKTEST_FXS=$FXS_DIR/ydktest/
BGP_DEVIATION_FXS=$FXS_DIR/bgp_deviation/
YDKTEST_DEVIATION_FXS=$FXS_DIR/ydktest_deviation/

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
    coverage run --source=ydkgen,sdk -a $@ 
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    return $status
}

# clone repo
function clone_repo {
    cd $ROOT
    printf "\nCloning from: %s, branch: %s\n" "$REPO" "$BRANCH"
    git clone -b $BRANCH $REPO 
}

function set_root {
    cd ydk-gen
    YDK_ROOT=`pwd`
    export YDKGEN_HOME=`pwd`
}

function setup_env {
    sudo apt-get update
    sudo apt-get --assume-yes install python-pip zlib1g-dev python-lxml libxml2-dev libxslt1-dev python-dev libboost-dev libboost-python-dev libcurl4-openssl-dev libtool

    cd ~
    git clone https://github.com/Kitware/CMake.git 
    git clone https://github.com/unittest-cpp/unittest-cpp.git
    git clone https://git.libssh.org/projects/libssh.git libssh
    git clone https://github.com/CESNET/libnetconf

    printf "\nMaking CMake...\n"
    cd CMake
    git checkout 8842a501cffe67a665b6fe70956e207193b3f76d
    ./bootstrap && make && make install

    printf "\nMaking unittest-cpp...\n"
    cd ~/unittest-cpp/builds
    git checkout 510903c880bc595cc6a2085acd903f3c3d956c54
    cmake ..
    cmake --build ./ --target install

    printf "\nMaking libssh...\n"
    cd ~/libssh
    git checkout 47d21b642094286fb22693cac75200e8e670ad78
    mkdir builds
    cd builds
    cmake ..
    make install

    printf "\nMaking libnetconf...\n"
    cd ~/libnetconf
    git checkout d4585969d71b7d7dec181955a6753b171b4a8424
    ./configure && make && make install

    cd $YDKGEN_HOME
    virtualenv myenv
    source myenv/bin/activate
    pip install coverage
    pip install -r requirements.txt 
}

function teardown_env {
    deactivate
}

# compile yang to fxs
function compile_yang_to_fxs {
    rm -f $YDKTEST_FXS/*.fxs
    source $CONFD/confdrc
    cd $YDK_ROOT/yang/ydktest

    printf "\n"
    for YANG_FILE in *.yang
    do
        if [[ ${YANG_FILE} != *"submodule"* ]];then
            printf "Compiling %s to fxs\n" "$YANG_FILE"
            confdc -c $YANG_FILE
        fi
    done

    mv *.fxs $YDKTEST_FXS

    cd $YDK_ROOT
}

# init confd for ydktest
function init_confd {
    cp $YDKTEST_FXS/* $CONFD_TARGET_DIR
    source $CONFD/confdrc
    cd $CONFD_TARGET_DIR
    confd -c confd.conf
    printf "\nInitializing confd\n"
}

# pygen test
function run_pygen_test {
    cd $YDK_ROOT
    # export PYTHONPATH=.:$PYTHONPATH
    # run_test test/pygen_tests.py
}

# generate ydktest package based on proile
function generate_ydktest_package {
    printf "\nGenerating ydktest model APIs with grouping classes\n"
    run_test generate.py --profile profiles/test/ydktest.json --python --verbose --groupings-as-class

    printf "\nGenerating ydktest model APIs with documentation\n"
    run_test generate.py --profile profiles/test/ydktest.json --python --verbose --generate-doc
}

# sanity tests
function run_sanity_ncclient_tests {
    printf "\nRunning sanity tests on NCClient client\n"
    run_test sdk/python/tests/test_sanity_types.py
    run_test sdk/python/tests/test_sanity_errors.py
    run_test sdk/python/tests/test_sanity_filters.py
    run_test sdk/python/tests/test_sanity_levels.py
    run_test sdk/python/tests/test_sanity_filter_read.py
    run_test sdk/python/tests/test_sanity_netconf.py
    run_test sdk/python/tests/test_sanity_rpc.py
    run_test sdk/python/tests/test_sanity_delete.py
    run_test sdk/python/tests/test_sanity_service_errors.py
}

function run_sanity_native_tests {
    printf "\nRunning sanity tests on native client\n"
    run_test sdk/python/tests/test_sanity_types.py native
    run_test sdk/python/tests/test_sanity_errors.py native
    run_test sdk/python/tests/test_sanity_filters.py native
    run_test sdk/python/tests/test_sanity_levels.py native
    run_test sdk/python/tests/test_sanity_filter_read.py native
    run_test sdk/python/tests/test_sanity_netconf.py native
    run_test sdk/python/tests/test_sanity_rpc.py native
    run_test sdk/python/tests/test_sanity_delete.py native
    run_test sdk/python/tests/test_sanity_service_errors.py native
    run_test sdk/python/tests/test_ydk_client.py
}

function run_sanity_tests {
    pip install gen-api/python/dist/ydk*.tar.gz
    source sdk/python/env.sh

    printf "\nRunning sanity tests\n"
    export PYTHONPATH=./sdk/python:$PYTHONPATH
    cp -r gen-api/python/ydk/models/* sdk/python/ydk/models
    run_test sdk/python/tests/test_sanity_codec.py

    run_sanity_ncclient_tests
    run_sanity_native_tests

    export PYTHONPATH=./gen-api/python:$PYTHONPATH
    run_test gen-api/python/ydk/tests/import_tests.py
}

# cpp tests
function run_cpp_gen_tests {
    printf "\nGenerating ydktest C++ model APIs\n"
    run_test generate.py --profile profiles/test/ydktest.json --cpp --verbose
    cd gen-api/cpp
    make
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    cd -
}

# cpp sanity tests
function run_cpp_sanity_tests {
    cd $YDK_ROOT/sdk/cpp/tests
    run_exec_test make clean all
    cd $YDK_ROOT
}

# cmake tests
function run_cmake_tests {
    printf "\nRunning CMake\n"
    cd $YDK_ROOT/sdk/cpp/builds
    cmake ..
    make install
    
    cmake .. -DBUILD_TESTS=ON
    make install
    make test
}

# deviation tests
# modify confd instance
function setup_deviation_sanity_models {
    source $CONFD/confdrc
    confd --stop

    printf "\nSetting up deviation sanity models\n"
    cp $BGP_DEVIATION_FXS/* $CONFD_TARGET_DIR
    cp $YDKTEST_DEVIATION_FXS/* $CONFD_TARGET_DIR
    cd $CONFD_TARGET_DIR
    confd -c confd.conf
}

# sanity deviation
function run_deviation_sanity {
    cd $YDK_ROOT
    source gen-api/python/env.sh
    export PYTHONPATH=./gen-api/python:$PYTHONPATH
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation.py
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation.py native

    # bgp deviation
    printf "\nGenerating ydktest deviation model APIs\n"
    python generate.py --python --profile profiles/test/deviation/deviation.json
    pip install gen-api/python/dist/ydk*.tar.gz
    source gen-api/python/env.sh
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py native
}

# submit coverage
function submit_coverage {
    if [[ "$BRANCH" == "master" ]] &&  [[ "$REPO" == *"CiscoDevNet/ydk-gen"* ]]
    then
        coverage report
        pip install coveralls
        export COVERALLS_REPO_TOKEN=MO7qRNCbd9uovAEK2w8Z41lRUgVMi0tbF
        coveralls
    fi
}

# Execution of the script starts here

REPO=$1
BRANCH=master
#TODO ADD Argument check

while getopts "r:b:" o; do
    case "${o}" in
        r)
            REPO=${OPTARG}
            ;;
        b)
            BRANCH=${OPTARG}
            ;;
    esac
done

clone_repo
printf "\nIn Method set_root\n"
set_root
printf "\nIn Method setup_env\n"
setup_env
printf "\nIn Method compile_yang_to_fxs\n"
compile_yang_to_fxs
printf "\nIn Method init_confd\n"
init_confd
printf "\nIn Method run_pygen_test\n"
run_pygen_test
printf "\nIn Method generate_ydktest_package\n"
generate_ydktest_package
printf "\nIn Method run_sanity_tests\n"
run_sanity_tests
printf "\nIn Method submit_coverage\n"
submit_coverage
printf "\nIn Method run_cpp_gen_tests\n"
run_cpp_gen_tests
printf "\nIn Method run_cpp_sanity_tests\n"
run_cpp_sanity_tests
printf "\nIn Method run_cmake_tests\n"
run_cmake_tests

printf "\nIn Method setup_deviation_sanity_models\n"
setup_deviation_sanity_models
printf "\nIn Method run_deviation_sanity\n"
run_deviation_sanity
printf "\nIn Method teardown_env\n"
teardown_env

exit
