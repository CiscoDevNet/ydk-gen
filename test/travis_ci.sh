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

# Terminal colors
RED="\033[0;31m"
NOCOLOR="\033[0m"

# Environment Paths
ROOT=/root
CONFD_RC=/root/confd/confdrc
YDKTEST_DEST_FXS=/root/confd/etc/ydktest
YDKTEST_MODEL_FXS=/root/confd/etc/ydk_model_test
YDKTEST_MODEL_DEST_FXS=/root/confd/etc/model_test_fxs
AUGMENTATION_DEST_FXS=/root/confd/etc/augmentation
DEVIATION_DEST_FXS=/root/confd/etc/deviation
YDKTEST_DEVIATION_SOURCE_FXS=/root/confd/src/confd/yang/ydktest/fxs/ydktest_deviation
BGP_DEVIATION_SOURCE_FXS=/root/confd/src/confd/yang/ydktest/fxs/bgp_deviation

function print_msg {
    echo -e "${RED}*** $(date) $1${NOCOLOR}"
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
    coverage run --source=ydkgen,sdk -a $@
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    return $status
}


function setup_env {
    print_msg "In Method: setup_env"
    cd $ROOT
    printf "\nCloning from: %s, branch: %s\n" "$REPO" "$BRANCH"
    git clone -b $BRANCH $REPO

    printf "\nSetting up YDKGEN_HOME\n"
    cd ydk-gen
    export YDKGEN_HOME=`pwd`

    printf "\nInstalling packages...\n"
    sudo apt-get update
    sudo apt-get --assume-yes install python-pip zlib1g-dev python-lxml libxml2-dev libxslt1-dev python-dev libboost-dev libboost-python-dev libtool libssh-dev libcurl4-gnutls-dev cmake

    cd ~
    git clone https://github.com/unittest-cpp/unittest-cpp.git
    git clone https://github.com/abhikeshav/libnetconf


    printf "\nMaking unittest-cpp...\n"
    cd ~/unittest-cpp/builds
    git checkout 510903c880bc595cc6a2085acd903f3c3d956c54
    cmake ..
    cmake --build ./ --target install

    printf "\nMaking libnetconf...\n"
    cd ~/libnetconf
    ./configure && make && make install

    cd $YDKGEN_HOME
    virtualenv myenv
    source myenv/bin/activate
    pip install coverage
    pip install -r requirements.txt
}

function teardown_env {
    print_msg "In Method: teardown_env"
    deactivate
}

# clone YDK model test YANG models
function compile_model_test_yang_to_fxs {
    rm -rf $YDKTEST_MODEL_FXS
    mkdir $YDKTEST_MODEL_FXS
    cd $YDKTEST_MODEL_FXS
    source $CONFD_RC

    git clone https://github.com/abhikeshav/ydk-test-yang.git
    cd ydk-test-yang/yang
    printf "\n"
    for YANG_FILE in *.yang
    do
        `grep "^submodule.*{" $YANG_FILE &> /dev/null`
        local status=$?
        if [ $status -eq 1 ]; then
            printf "Compiling %s to fxs\n" "$YANG_FILE"
            confdc -c $YANG_FILE
        fi
    done

    mv *.fxs $YDKTEST_MODEL_DEST_FXS

    cd $YDKGEN_HOME
}

# compile YANG files from $1 to fxs
function compile_yang_to_fxs {
    source $CONFD_RC
    cd $1
    print_msg "In Method: compile_yang_to_fxs"
    for YANG_FILE in *.yang
    do
        if [[ ${YANG_FILE} != *"submodule"* ]];then
            printf "\nCompiling %s to fxs" "$YANG_FILE"
            confdc -c $YANG_FILE
        fi
    done
    cd -
}

# move fxs files from $1 to $2
function cp_fxs {
    cp $1/*.fxs $2
    print_msg "In Method: cp_fxs"
}

# init confd using confd.conf in $1
function init_confd {
    source $CONFD_RC
    confd --stop
    cd $1
    print_msg "In Method: init_confd"
    printf "\nInitializing confd...\n"
    confd -c confd.conf
    cd $YDKGEN_HOME
}

# pygen test
function run_pygen_test {
    print_msg "In Method: run_pygen_test"
    cd $YDKGEN_HOME
    export PYTHONPATH=$YDKGEN_HOME
    run_test test/pygen_tests.py --aug-base profiles/test-augmentation/ietf.json \
    --aug-contrib profiles/test-augmentation/ydktest-aug-ietf-1.json profiles/test-augmentation/ydktest-aug-ietf-2.json profiles/test-augmentation/ydktest-aug-ietf-4.json \
    --aug-compare profiles/test-augmentation/ydktest-aug-ietf.json \
    -v
}

# generate ydktest package based on proile
function generate_ydktest_package {
    print_msg "In Method: generate_ydktest_package"
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
    print_msg "In Method: run_sanity_tests"
    pip install gen-api/python/dist/ydk*.tar.gz

    printf "\nRunning sanity tests\n"
    export PYTHONPATH=./sdk/python:$PYTHONPATH
    cp -r gen-api/python/tests sdk/python/tests
    run_test sdk/python/tests/test_sanity_codec.py

    run_sanity_ncclient_tests
#    run_sanity_native_tests

    export PYTHONPATH=./gen-api/python:$PYTHONPATH
    run_test gen-api/python/ydk/tests/import_tests.py
}

# ydk model tests
function run_ydk_model_tests {
    print_msg "In Method: run_ydk_model_tests"
    cd $YDKGEN_HOME

    virtualenv myenv
    source myenv/bin/activate
    pip install coverage
    pip install -r requirements.txt

    printf "\nGenerating ydk model APIs for testing\n"
    python generate.py --profile profiles/test/ydk-models-test.json --verbose
    cd $YDKGEN_HOME/gen-api/python
    pip uninstall -y ydk
    pip install dist/*.tar.gz
    export PYTHONPATH=.:$PYTHONPATH
    for file in $(find ydk/tests -name '*.py'); do
        source $CONFD_RC
        confd --status &> /dev/null
        local status=$?
        if [ $status -ne 0 ]; then
            printf "\nRestarting confd\n"
            source $CONFD_RC
            cd $YDKTEST_MODEL_DEST_FXS
            confd -c confd.conf
            local c_status=$?
            if [ $c_status -ne 0 ]; then
                return $c_status
            fi
            cd -
        fi
        printf "\nRunning %s python model test\n" "$file"
        python $file
    done

    cd $YDKGEN_HOME
}

# cpp tests
function run_cpp_gen_tests {
    print_msg "In Method: run_cpp_gen_tests"
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
    print_msg "In Method: run_cpp_sanity_tests"
    cd $YDKGEN_HOME/sdk/cpp/tests
    run_exec_test make clean all
    cd $YDKGEN_HOME
}

# cmake tests
function run_cmake_tests {
    print_msg "In Method: run_cmake_tests"
    printf "\nRunning CMake\n"
    cd $YDKGEN_HOME/sdk/cpp/builds
    cmake ..
    make install

    cmake .. -DBUILD_TESTS=ON
    make install
    make test
}

# sanity deviation
function run_deviation_sanity {
    print_msg "In Method: run_deviation_sanity"
    cd $YDKGEN_HOME
    rm -rf gen-api/python/*
    # ydktest deviation
    cp_fxs $YDKTEST_DEVIATION_SOURCE_FXS $YDKTEST_DEST_FXS
    init_confd $YDKTEST_DEST_FXS
    printf "\nGenerating ydktest model APIs with grouping classes\n"
    run_test_no_coverage generate.py --profile profiles/test/ydktest.json --python --verbose
    pip install gen-api/python/dist/*.tar.gz
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation.py
#    run_test_no_coverage gen-api/python/tests/test_sanity_deviation.py native

    # bgp deviation
    cp_fxs $BGP_DEVIATION_SOURCE_FXS $DEVIATION_DEST_FXS
    init_confd $DEVIATION_DEST_FXS
    printf "\nGenerating ydktest deviation model APIs\n"
    run_test_no_coverage generate.py --python --profile profiles/test/deviation/deviation.json
    pip install gen-api/python/dist/ydk*.tar.gz
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py
#    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py 
#    native

    pip uninstall ydk -y
}

# generate ydktest augmentation packages
function generate_ydktest_augm_packages {
    rm -rf gen-api/python/*
    run_test generate.py --core
    run_test generate.py --bundle profiles/test-augmentation/ietf.json --verbose
    run_test generate.py --bundle profiles/test-augmentation/ydktest-aug-ietf-1.json --verbose
    run_test generate.py --bundle profiles/test-augmentation/ydktest-aug-ietf-2.json --verbose
    run_test generate.py --bundle profiles/test-augmentation/ydktest-aug-ietf-4.json --verbose
}
# install ydktest augmentation packages
function install_ydktest_augm_packages {
    print_msg "In Method: install_ydktest_augm_packages"
    pip uninstall ydk -y
    CORE_PKG=$(find gen-api/python/ydk/dist -name "ydk*.tar.gz")
    AUGM_BASE_PKG=$(find gen-api/python/ietf/dist -name "ydk*.tar.gz")
    AUGM_CONTRIB_PKGS=$(find gen-api/python/ydktest_aug_ietf_*/dist -name "ydk*.tar.gz")
    pip install $CORE_PKG
    pip install $AUGM_BASE_PKG
    for PKG in $AUGM_CONTRIB_PKGS;do
        pip install $PKG
    done
}
# run sanity tests for ydktest augmentation package
function run_sanity_ydktest_augm_tests {
    # TODO: test case wrapper.
    print_msg "In Method: run_sanity_ydktest_augm_tests"
    run_test_no_coverage gen-api/python/ydk/tests/test_sanity_bundle_aug.py
}


# submit coverage
function submit_coverage {
    print_msg "In Method: submit_coverage"
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


setup_env

# ydktest
compile_yang_to_fxs $YDKGEN_HOME/yang/ydktest
cp_fxs $YDKGEN_HOME/yang/ydktest $YDKTEST_DEST_FXS
cp_fxs $YDKGEN_HOME/yang/ydktest $DEVIATION_DEST_FXS
cp_fxs $YDKGEN_HOME/yang/ydktest $AUGMENTATION_DEST_FXS
cp_fxs $YDKGEN_HOME/yang/ydktest $YDKTEST_MODEL_DEST_FXS
init_confd $YDKTEST_DEST_FXS
run_pygen_test
generate_ydktest_package
run_sanity_tests
submit_coverage
run_cpp_gen_tests
run_cpp_sanity_tests
run_cmake_tests

# deviaiton
cp_fxs $DEVIATION_SOURCE_FXS $DEVIATION_DEST_FXS
run_deviation_sanity

# ydk model test
compile_model_test_yang_to_fxs
init_confd $YDKTEST_MODEL_DEST_FXS
#run_ydk_model_tests

# ydk namespace package augmentation
compile_yang_to_fxs $YDKGEN_HOME/yang/ydktest-aug-ietf
cp_fxs $YDKGEN_HOME/yang/ydktest-aug-ietf $AUGMENTATION_DEST_FXS
init_confd $AUGMENTATION_DEST_FXS
generate_ydktest_augm_packages
install_ydktest_augm_packages
run_sanity_ydktest_augm_tests

teardown_env

exit
