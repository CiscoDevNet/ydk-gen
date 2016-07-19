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

# Colors.
RED="\033[0;31m"
NOCOLOR="\033[0m"

# Environment Paths.
ROOT=/root
CONFD_RC=/root/confd/confdrc
YDKTEST_FXS=/root/confd/etc/ydktest
AUGMENTSTION_FXS=/root/confd/etc/augmentation
DEVIATION_FXS=/root/confd/etc/deviation
DEVIATION_SOURCE_FXS=/root/confd/src/confd/yang/ydktest/fxs/deviation

# Wrapper functions to run scripts.
## Wrapper function to exec script.
function run_exec_test {
    $@
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    echo_run_msg "TEST: Finished exec script: $1"
    return $status
}
## Wrapper functions using python to run script.
function run_test_no_coverage {
    python $@
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    echo_run_msg "TEST: Finished running script: $1"
    return $status
}
## Wrapper function collecting coverage info for script.
function run_test {
    coverage run --source=ydkgen,sdk -a $@
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    echo_run_msg "TEST: Finished running script with coverage: $1"
    return $status
}


# Wrapper functions to display logging message.
## Wrapper function to display message with date prefix.
function echo_run_msg {
    echo -e "*** $(date) $1"
}
## Wrapper fcuntion to display message with date prefix in red.
function echo_msg {
    echo -e "${RED}*** $(date) $1${NOCOLOR}"
}


# Clone repository.
function clone_repo {
    cd $ROOT
    git clone -b $BRANCH $REPO
    echo_msg "SETUP: Finished cloning repository."
}


# Setup up ydk gen environment.
## Setup ydk gen environment.
function setup_ydkgen_env {
    clone_repo
    cd ydk-gen
    sudo apt-get update
    sudo apt-get --assume-yes install python-pip zlib1g-dev python-lxml libxml2-dev libxslt1-dev python-dev libboost-dev libboost-python-dev libssh-dev libcurl4-openssl-dev libtool
    source install.sh
    pip install coverage
    echo_msg "SETUP: Finished setting up ydkgen environment."
}

## Teardown activate virtualenv.
function teardown_env {
    deactivate
    echo_msg "SETUP: Finished tearing down virtualenv."
}


# ConfD
## Compile YANG files in $1 to fxs files
function compile_yang_to_fxs {
    cd $1
    for YANG_FILE in *.yang; do
        if [[ ${YANG_FILE} != *"submodule"* && ${YANG_FILE} != *"deviation"* ]];then
            confdc -c $YANG_FILE
            echo_msg "CONFD: Finished compiling ${YANG_FILE} to fxs."
        fi
    done
    move_fxs $1 $2
    cd $YDKGEN_HOME
}

## Move fxs files from $1 to $2
function move_fxs {
    mv $(find $1/*.fxs) $2
    echo_msg "CONFD: Finished moving fxs files from $1 to $2."
}

## Initiate and start ConfD session.
function setup_confd {
    # $1: pre-processing function.
    # $2: source directory for YANG models or pre-compiled fxs files.
    # $3, $4, $5(if any): destination directory for compiled fxs files,
    #                     use $2 as destination directory if no $3, $4...
    DEST_FXS_DIRS=${@:3:$#}
    if [[ -z DEST_FXS_DIRS ]]
    then
        DEST_FXS_DIR=$2

    else
        DEST_FXS_DIR=$3
    fi

    source $CONFD_RC
    confd --stop

    for DEST_DIR in ${DEST_FXS_DIRS};do
        $1 $2 $DEST_DIR
    done

    cd $DEST_FXS_DIR
    confd -c confd.conf
    echo_msg "CONFD: Finished initializing ConfD in ${DEST_FXS_DIR}."
    cd $YDKGEN_HOME
}


# Generator tests.
## PyGen Test, compare genearted API with expected APIs for python bindings.
function run_pygen_test {
    cd $YDKGEN_HOME
    export PYTHONPATH=$YDKGEN_HOME
    run_test test/pygen_tests.py --aug-base profiles/profile-with-dependencies/ietf.json \
    --aug-contrib profiles/profile-with-dependencies/ydktest-aug-ietf-1.json profiles/profile-with-dependencies/ydktest-aug-ietf-2.json profiles/profile-with-dependencies/ydktest-aug-ietf-4.json \
    --aug-compare profiles/profile-with-dependencies/ydktest-aug-ietf.json \
    -v
    echo_msg "TEST: Finished running PyGen test."
}

## CppGen test.
function run_cpp_gen_tests {
    run_test generate.py --profile profiles/test/ydktest.json --cpp --verbose
    echo_msg "YDKGEN: Finished generating ydktest C++ model APIs"
    cd gen-api/cpp
    make
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    echo_msg "TEST: Finished running cpp gen tests."
    cd -
}


# Sanity tests.
## Sanity test for ydktest package.
### Generate ydktest package.
function generate_ydktest_package {
    run_test generate.py --profile profiles/test/ydktest.json --python --verbose --groupings-as-class
    echo_msg "YDKGEN: Finished generating ydktest model APIs with grouping classes."
    run_test generate.py --profile profiles/test/ydktest.json --python --verbose --generate-doc
    echo_msg "YDKGEN: Finished generating ydktest model APIs with documentation."
}
### Install ydktest package.
function install_ydktest_package {
    PKG=$(find gen-api/python/dist -name "ydk*.tar.gz")
    pip install $PKG
    echo_msg "INSTALL: Finished installing ${PKG}."
}
### Run sanity tests for ydktest package.
function run_sanity_ncclient_tests {
    run_test gen-api/python/tests/test_sanity_types.py
    run_test gen-api/python/tests/test_sanity_errors.py
    run_test gen-api/python/tests/test_sanity_filters.py
    run_test gen-api/python/tests/test_sanity_levels.py
    run_test gen-api/python/tests/test_sanity_filter_read.py
    run_test gen-api/python/tests/test_sanity_netconf.py
    run_test gen-api/python/tests/test_sanity_rpc.py
    run_test gen-api/python/tests/test_sanity_delete.py
    run_test gen-api/python/tests/test_sanity_service_errors.py
    echo_msg "TEST: Finished running sanity tests on NCClient client."
}
### Run sanity tests for ydktest package with native client.
function run_sanity_native_tests {
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
    echo_msg "TEST: Finished running sanity tests on native client."
}
### Run sanity tests for ydktest package.
function run_sanity_tests {
    run_test gen-api/python/tests/test_sanity_codec.py
    run_sanity_ncclient_tests
    # run_sanity_native_tests
    run_test gen-api/python/ydk/tests/import_tests.py
    echo_msg "TEST: Finished running sanity tests for ydktest package."
}
### cpp sanity tests
function run_cpp_sanity_tests {
    sudo apt install libunittest++-dev
    cd $YDKGEN_HOME/sdk/cpp/tests
    run_exec_test make clean all
    echo_msg "TEST: Finished running cpp sanity tests."
    cd $YDKGEN_HOME
}

## Sanity test for ydktest augmentation.
### Generate ydktest augmentation packages.
function generate_ydktest_augm_packages {
    rm -rf gen-api/python/*
    run_test generate.py --core
    run_test generate.py --bundle profiles/profile-with-dependencies/ietf.json --verbose
    run_test generate.py --bundle profiles/profile-with-dependencies/ydktest-aug-ietf-1.json --verbose
    run_test generate.py --bundle profiles/profile-with-dependencies/ydktest-aug-ietf-2.json --verbose
    run_test generate.py --bundle profiles/profile-with-dependencies/ydktest-aug-ietf-4.json --verbose
    echo_msg "YDKGEN: Finished generating ydktest augmentation packages."
}
### Install ydktest augmentation packages.
function install_ydktest_augm_packages {
    CORE_PKG=$(find gen-api/python/ydk/dist -name "ydk*.tar.gz")
    AUGM_BASE_PKG=$(find gen-api/python/ietf/dist -name "ydk*.tar.gz")
    AUGM_CONTRIB_PKGS=$(find gen-api/python/ydktest_aug_ietf_*/dist -name "ydk*.tar.gz")
    pip install $CORE_PKG
    echo_msg "INSTALL: Finished installing ${CORE_PKG}."
    pip install $AUGM_BASE_PKG
    echo_msg "INSTALL: Finished installing ${AUGM_BASE_PKG}."
    for PKG in $AUGM_CONTRIB_PKGS;do
        pip install $PKG
        echo_msg "INSTALL: Finished installing ${PKG}."
    done
    echo_msg "INSTALL: Finished installing ydktest augmentation packages."
}
### Run sanity tests for ydktest augmentation package.
function run_sanity_ydktest_augm_tests {
    # TODO: test case wrapper.
    run_test gen-api/python/ydk/tests/test_sanity_bundle_aug.py
    echo_msg "TEST: Finished running sanity tests for ydktest augmentation package."
}

## Santi tests for deviation package.
### Generate bgp deviation packages.
function generate_bgp_deviation_package {
    run_test generate.py --python --profile profiles/test/deviation/deviation.json
    echo_msg "YDKGEN: Finished generating bgp deviation package."
}
### Install bgp deviation packages.
function install_bgp_deviation_package {
    pip install gen-api/python/dist/ydk*.tar.gz
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py
    echo_msg "TEST: Finished running bgp deviation sanity test."
}
### Run bgp deviation sanity test.
function run_sanity_bgp_deviation_tests {
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py
    echo_msg "TEST: Finished running bgp deviation sanity test."
}
### Run ydktest deivation sanity test.
function run_sanity_ydktest_deviation_tests {
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation.py
    echo_msg "TEST: Finished running ydktest deviation sanity test."
}


# Coverage.
## Submit coverage.
function submit_coverage {
    if [[ "$BRANCH" == "master" ]] &&  [[ "$REPO" == *"CiscoDevNet/ydk-gen"* ]]
    then
        coverage report
        pip install coveralls
        export COVERALLS_REPO_TOKEN=MO7qRNCbd9uovAEK2w8Z41lRUgVMi0tbF
        coveralls
    fi
    echo_msg "COVERAGE: Finished submitting coverage."
}


###############################################################################

# Prase arguments
REPO=$1
BRANCH=master

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

# Clone ydk-gen repository, setup environment variabls.
setup_ydkgen_env

# Run generator tests not depends on ConfD session.
run_pygen_test
run_cpp_gen_tests

# Run tests depends on YANG models in $YDKGEN_HOME/yang/ydktest.
setup_confd compile_yang_to_fxs $YDKGEN_HOME/yang/ydktest $YDKTEST_FXS $DEVIATION_FXS $AUGMENTSTION_FXS
generate_ydktest_package
install_ydktest_package
run_sanity_ydktest_tests
submit_coverage
run_cpp_sanity_tests

# Run tests with additional dependency on YANG models in $YDKGEN_HOME/yang/ydktest-aug-ietf.
setup_confd compile_yang_to_fxs $YDKGEN_HOME/yang/ydktest-aug-ietf $AUGMENTSTION_FXS
generate_ydktest_augm_packages
install_ydktest_augm_packages
run_sanity_ydktest_augm_tests

# Run tests with additional dependency on fxs files in $DEVIATION_FXS.
setup_confd move_fxs $DEVIATION_SOURCE_FXS $DEVIATION_FXS
generate_ydktest_package
install_ydktest_package
run_sanity_ydktest_deviation_tests
generate_bgp_deviation_package
install_bgp_deviation_package
run_sanity_bgp_deviation_tests

# Clean up
teardown_env
