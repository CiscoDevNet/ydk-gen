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

    cd $ROOT
    printf "\nCloning from: %s, branch: %s\n" "$REPO" "$BRANCH"
    git clone -b $BRANCH $REPO

    printf "\nSetting up YDKGEN_HOME\n"
    cd ydk-gen
    export YDKGEN_HOME=`pwd`

    printf "\nInstalling packages...\n"
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

# compile YANG files from $1 to fxs
function compile_yang_to_fxs {
    source $CONFD_RC
    cd $1
    for YANG_FILE in *.yang
    do
        if [[ ${YANG_FILE} != *"submodule"* ]];then
            printf "\nCompiling %s to fxs" "$YANG_FILE"
            confdc -c $YANG_FILE
        fi
    done
    cd $YDKGEN_HOME
}

# move fxs files from $1 to $2
function cp_fxs {
    cp $1/*.fxs $2
}

# init confd using confd.conf in $1
function init_confd {
    source $CONFD_RC
    confd --stop
    cd $1
    printf "\nInitializing confd...\n"
    confd -c confd.conf
    cd $YDKGEN_HOME
}

# pygen test
function run_pygen_test {
    cd $YDKGEN_HOME
    export PYTHONPATH=$YDKGEN_HOME
    run_test test/pygen_tests.py --aug-base profiles/test-augmentation/ietf.json \
    --aug-contrib profiles/test-augmentation/ydktest-aug-ietf-1.json profiles/test-augmentation/ydktest-aug-ietf-2.json profiles/test-augmentation/ydktest-aug-ietf-4.json \
    --aug-compare profiles/test-augmentation/ydktest-aug-ietf.json \
    -v
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
    run_test gen-api/python/tests/test_sanity_types.py
    run_test gen-api/python/tests/test_sanity_errors.py
    run_test gen-api/python/tests/test_sanity_filters.py
    run_test gen-api/python/tests/test_sanity_levels.py
    run_test gen-api/python/tests/test_sanity_filter_read.py
    run_test gen-api/python/tests/test_sanity_netconf.py
    run_test gen-api/python/tests/test_sanity_rpc.py
    run_test gen-api/python/tests/test_sanity_delete.py
    run_test gen-api/python/tests/test_sanity_service_errors.py
}

function run_sanity_native_tests {
    printf "\nRunning sanity tests on native client\n"
    run_test gen-api/python/tests/test_sanity_types.py native
    run_test gen-api/python/tests/test_sanity_errors.py native
    run_test gen-api/python/tests/test_sanity_filters.py native
    run_test gen-api/python/tests/test_sanity_levels.py native
    run_test gen-api/python/tests/test_sanity_filter_read.py native
    run_test gen-api/python/tests/test_sanity_netconf.py native
    run_test gen-api/python/tests/test_sanity_rpc.py native
    run_test gen-api/python/tests/test_sanity_delete.py native
    run_test gen-api/python/tests/test_sanity_service_errors.py native
    run_test gen-api/python/tests/test_ydk_client.py
}

function run_sanity_tests {
    pip install gen-api/python/dist/ydk*.tar.gz

    printf "\nRunning sanity tests\n"
    run_test gen-api/python/tests/test_sanity_codec.py

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
    cd $YDKGEN_HOME/sdk/cpp/tests
    run_exec_test make clean all
    cd $YDKGEN_HOME
}

# cmake tests
function run_cmake_tests {
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
    cd $YDKGEN_HOME
    rm -rf gen-api/python/*
    # ydktest deviation
    cp_fxs $YDKTEST_DEVIATION_SOURCE_FXS $YDKTEST_DEST_FXS
    init_confd $YDKTEST_DEST_FXS
    printf "\nGenerating ydktest model APIs with grouping classes\n"
    run_test_no_coverage generate.py --profile profiles/test/ydktest.json --python --verbose
    pip install gen-api/python/dist/*.tar.gz
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation.py
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation.py native

    # bgp deviation
    cp_fxs $BGP_DEVIATION_SOURCE_FXS $DEVIATION_DEST_FXS
    init_confd $DEVIATION_DEST_FXS
    printf "\nGenerating ydktest deviation model APIs\n"
    run_test_no_coverage generate.py --python --profile profiles/test/deviation/deviation.json
    pip install gen-api/python/dist/ydk*.tar.gz
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py native

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
    run_test gen-api/python/ydk/tests/test_sanity_bundle_aug.py
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

print_msg "In Method: setup_env"
setup_env
print_msg "In Method: compile_yang_to_fxs"
compile_yang_to_fxs $YDKGEN_HOME/yang/ydktest
print_msg "In Method: cp_fxs"
cp_fxs $YDKGEN_HOME/yang/ydktest $YDKTEST_DEST_FXS
print_msg "In Method: cp_fxs"
cp_fxs $YDKGEN_HOME/yang/ydktest $DEVIATION_DEST_FXS
print_msg "In Method: cp_fxs"
cp_fxs $YDKGEN_HOME/yang/ydktest $AUGMENTATION_DEST_FXS
print_msg "In Method: init_confd"
init_confd $YDKTEST_DEST_FXS
print_msg "In Method: run_pygen_test"
run_pygen_test
print_msg "In Method: generate_ydktest_package"
generate_ydktest_package
print_msg "In Method: run_sanity_tests"
run_sanity_tests
print_msg "In Method: submit_coverage"
submit_coverage
print_msg "In Method: run_cpp_gen_tests"
run_cpp_gen_tests
print_msg "In Method: run_cpp_sanity_tests"
run_cpp_sanity_tests
print_msg "In Method: run_cmake_tests"
run_cmake_tests

print_msg "In Method: cp_fxs"
cp_fxs $DEVIATION_SOURCE_FXS $DEVIATION_DEST_FXS
print_msg "In Method: run_deviation_sanity"
run_deviation_sanity

print_msg "In Method: compile_yang_to_fxs"
compile_yang_to_fxs $YDKGEN_HOME/yang/ydktest-aug-ietf
print_msg "In Method: cp_fxs"
cp_fxs $YDKGEN_HOME/yang/ydktest-aug-ietf $AUGMENTATION_DEST_FXS
print_msg "In Method: init_confd"
init_confd $AUGMENTATION_DEST_FXS
print_msg "In Method: generate_ydktest_augm_fxs"
generate_ydktest_augm_packages
print_msg "In Method: install_ydktest_augm_packages"
install_ydktest_augm_packages
print_msg "In Method: run_sanity_ydktest_augm_tests"
run_sanity_ydktest_augm_tests

print_msg "In Method: teardown_env"
teardown_env

exit
