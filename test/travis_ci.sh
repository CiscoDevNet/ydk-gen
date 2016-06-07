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
    source install.sh
    pip install coverage
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

# generate ydktest package based on profile
function generate_ydktest_package {
    # need sanity test
    printf "\nGenerating ydktest model APIs with grouping classes\n"
    run_test generate.py --profile profiles/test/ydktest.json --python --verbose --groupings-as-class

    printf "\nGenerating ydktest model APIs with documentation\n"
    run_test generate.py --profile profiles/test/ydktest.json --python --verbose --generate-doc
}

# sanity tests
function run_sanity_tests {
    pip install gen-api/python/dist/ydk*.tar.gz

    printf "\nRunning sanity tests\n"

    cd gen-api/python
    source env.sh

    for f in $(find tests/* ! -name '*deviation*.py' ! -name 'compare.py' ! -name '__init__.py')
    do
        run_test $f
    done

    run_test ydk/tests/import_tests.py

    cd $YDK_ROOT
}

# generate ydktest bundle package based on bundle files
function generate_bundle_packages {
    printf "\nTranslating profile files to bundle files\n"
    run_test ydkgen/resolver/bundle_translator.py --verbose

    # validate translated bundle files
    pip install demjson
    for f in $(find bundles -type f ! -name 'bgp_ydk_dev.json' ! -name 'ydk_0_1_0.json' -name '*.json')
    do
        printf "Linting %s\n" "$f"
        jsonlint $f > /dev/null
        if [ $? -ne 0 ]; then
            exit $?
        fi
    done

    printf "\nGenerating core library"
    run_test generate.py --python --core --verbose

    printf "\nGenerating bundle packages\n"
    run_test generate.py --python --bundle bundles/test/ydktest.json --verbose
    run_test generate.py --python --bundle bundles/ietf/ietf_models.json --verbose

}

function install_bundle_packages {
    for p in $(find gen-api/python/ydk*/dist -type f -name '*.tar.gz')
    do
        printf "\nInstalling bundle package %s\n" "$p"
        pip install $p --force-reinstall
    done
}

function run_bundle_sanity_tests {
    # sanity tests are bind with core library at the moment
    cd gen-api/python/ydk/
    source env.sh
    for f in $(find tests/* ! -name '*deviation*.py' ! -name 'compare.py' ! -name '__init__.py')
    do
        run_test $f
    done

    # import tests for bundle packages
    for f in $(find . -type f -name 'import_tests.py')
    do
        run_test $f
    done

    cd $YDK_ROOT
}

# cpp tests
function run_cpp_tests {
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

    # current ydk is core library need to regenerate sanity package
    printf "\nGenerating ydktest model APIs with documentation\n"
    export PYTHONPATH=''
    run_test_no_coverage generate.py --profile profiles/test/ydktest.json --python --verbose
    pip install gen-api/python/dist/ydk*.tar.gz --force-reinstall
    source gen-api/python/env.sh
    export PYTHONPATH=./gen-api/python:$PYTHONPATH
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation.py

    # bgp deviation
    printf "\nGenerating ydktest deviation model APIs\n"
    python generate.py --python --profile profiles/test/deviation/deviation.json
    pip install gen-api/python/dist/ydk*.tar.gz
    source gen-api/python/env.sh
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py
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
set_root
setup_env
compile_yang_to_fxs
init_confd
run_pygen_test
# profile packages
generate_ydktest_package
run_sanity_tests
# bundle packages
generate_bundle_packages
install_bundle_packages
run_bundle_sanity_tests
submit_coverage
run_cpp_tests

setup_deviation_sanity_models
run_deviation_sanity
teardown_env


exit

