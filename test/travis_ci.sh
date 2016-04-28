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
# Script for running ydk CI on docker
#
# ------------------------------------------------------------------


ROOT=/root
CONFD=/root/confd
CONFD_TARGET_DIR=$CONFD/etc/confd
FXS_DIR=$CONFD/src/confd/yang/ydktest/fxs/
YDKTEST_FXS=$FXS_DIR/ydktest/
BGP_DEVIATION_FXS=$FXS_DIR/bgp_deviation/
YDKTEST_DEVIATION_FXS=$FXS_DIR/ydktest_deviation/

function run_test {
    "$@"
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
    source install.sh
    export PYTHONPATH=.:$PYTHONPATH
    run_test python test/pygen_tests.py
}

# generate ydktest package based on proile
function generate_ydktest_package {
    printf "\nGenerating ydktest model APIs with grouping classes\n"
    run_test python generate.py --profile profiles/test/ydktest.json --python --verbose --groupings-as-class

    printf "\nGenerating ydktest model APIs with documentation\n"
    run_test python generate.py --profile profiles/test/ydktest.json --python --verbose --generate-doc
    deactivate
}

# sanity tests
function run_sanity_tests {
    virtualenv myenv
    source myenv/bin/activate
    pip install gen-api/python/dist/ydk*.tar.gz
    source gen-api/python/env.sh
    cd gen-api/python

    printf "\nRunning sanity tests\n"
    run_test python tests/test_sanity_codec.py
    run_test python tests/test_sanity_types.py
    run_test python tests/test_sanity_filters.py
    run_test python tests/test_sanity_levels.py
    run_test python tests/test_sanity_filter_read.py
    run_test python tests/test_sanity_netconf.py
    run_test python tests/test_sanity_rpc.py
    run_test python tests/test_sanity_delete.py
    cd ydk/tests
    run_test python import_tests.py
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
    cd gen-api/python
    export PYTHONPATH=.:$PYTHONPATH
    run_test python tests/test_sanity_deviation.py

    # bgp deviation
    deactivate
    cd $YDK_ROOT
    source mypython/bin/activate
    printf "\nGenerating ydktest deviation model APIs\n"
    python generate.py --python --profile profiles/test/deviation/deviation.json
    deactivate
    source myenv/bin/activate
    pip install gen-api/python/dist/ydk*.tar.gz
    source gen-api/python/env.sh
    cd gen-api/python
    run_test python tests/test_sanity_deviation_bgp.py
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
compile_yang_to_fxs
init_confd
run_pygen_test
generate_ydktest_package
run_sanity_tests
setup_deviation_sanity_models
run_deviation_sanity

exit

