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
    coverage run --source=ydkgen,sdk -a $@
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
    print_msg "init_confd in $(pwd)"
    source $YDKGEN_HOME/../confd/confdrc
    run_exec_test make stop > /dev/null
    run_exec_test make clean > /dev/null
    run_exec_test make all > /dev/null
    run_exec_test make start
    cd -
}

function py_sanity_ydktest {
    print_msg "py_sanity_ydktest"

    py_sanity_ydktest_gen
    py_sanity_ydktest_install
    py_sanity_ydktest_test
}

function py_sanity_ydktest_gen {
    print_msg "py_sanity_ydktest_gen"

    cd $YDKGEN_HOME && source gen_env/bin/activate

    print_msg "py_sanity_ydktest_gen: grouping as class"
    run_test generate.py --profile profiles/test/ydktest.json --python --groupings-as-class

    print_msg "py_sanity_ydktest_gen: grouping expansion, generate documentation"
    run_test generate.py --profile profiles/test/ydktest.json --python --generate-doc
}

function py_sanity_ydktest_install {
    print_msg "py_sanity_ydktest_install"

    cd $YDKGEN_HOME && source test_env/bin/activate
    pip install gen-api/python/dist/ydk*.tar.gz
}

function py_sanity_ydktest_test {
    print_msg "py_sanity_ydktest_test"

    init_confd $YDKGEN_HOME/sdk/cpp/ydk/tests/confd/ydktest

    cd $YDKGEN_HOME && cp -r gen-api/python/tests sdk/python/tests

    export PYTHONPATH=./gen-api/python:$PYTHONPATH
    run_test gen-api/python/ydk/tests/import_tests.py

    export PYTHONPATH=./sdk/python:$PYTHONPATH
    run_test sdk/python/tests/test_sanity_codec.py

    py_sanity_ydktest_test_ncclient
    # py_sanity_ydktest_test_native
}

function py_sanity_ydktest_test_ncclient {
    print_msg "py_sanity_ydktest_test_ncclient"
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

function py_sanity_ydktest_test_native {
    print_msg "py_sanity_ydktest_test_native"
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
    run_test_no_coverage generate.py --profile profiles/test/ydktest.json --python
}

function py_sanity_deviation_ydktest_install {
    print_msg "py_sanity_deviation_ydktest_install"

    source test_env/bin/activate
    pip uninstall ydk -y && pip install gen-api/python/dist/ydk*.tar.gz
}

function py_sanity_deviation_ydktest_test {
    print_msg "py_sanity_deviation_ydktest_test"

    init_confd $YDKGEN_HOME/sdk/cpp/ydk/tests/confd/deviation
    run_test_no_coverage gen-api/python/tests/test_sanity_deviation.py
}

function py_sanity_deviation_bgp_gen {
    print_msg "py_sanity_deviation_bgp_gen"

    rm -rf gen-api/python/*
    cd $YDKGEN_HOME && source gen_env/bin/activate
    run_test_no_coverage generate.py --profile profiles/test/deviation.json --verbose
}

function py_sanity_deviation_bgp_insall {
    print_msg "py_sanity_deviation_bgp_install"

    cd $YDKGEN_HOME && source test_env/bin/activate
    pip uninstall ydk -y && pip install gen-api/python/dist/*.tar.gz
}

function py_sanity_deviation_bgp_test {
    print_msg "py_sanity_deviation_bgp_test"

    run_test_no_coverage gen-api/python/tests/test_sanity_deviation_bgp.py
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

    init_confd $YDKGEN_HOME/sdk/cpp/ydk/tests/confd/augmentation
    run_test_no_coverage gen-api/python/ydk/tests/test_sanity_bundle_aug.py
}

function cpp_sanity_core {
    print_msg "cpp_sanity_core"

    cpp_sanity_core_gen_install
    cpp_sanity_core_test
}

function cpp_sanity_core_gen_install {
    print_msg "cpp_sanity_core_gen_install"

    cd $YDKGEN_HOME && source gen_env/bin/activate
    run_test_no_coverage generate.py --core --cpp --verbose --sudo
}

function cpp_sanity_core_test {
    print_msg "cpp_sanity_core_test"

    init_confd $YDKGEN_HOME/sdk/cpp/ydk/tests/confd/ydktest
    cd gen-api/cpp/ydk/build
    run_exec_test make test
}

function cpp_sanity_ydktest {
    print_msg "cpp_sanity_ydktest"

    cpp_sanity_ydktest_gen_install
    cpp_sanity_ydktest_test
}

function cpp_sanity_ydktest_gen_install {
    print_msg "cpp_sanity_ydktest_gen"

    cd $YDKGEN_HOME && source gen_env/bin/activate
    run_test generate.py --bundle profiles/test/ydktest-cpp.json --cpp --sudo
}

function cpp_sanity_ydktest_test {
    print_msg "cpp_sanity_ydktest_test"

    mkdir -p $YDKGEN_HOME/sdk/cpp/tests/build && cd sdk/cpp/tests/build
    run_exec_test cmake ..
    run_exec_test make
    run_exec_test make test
}

# # clone YDK model test YANG models
# function compile_model_test_yang_to_fxs {
#     print_msg "In Method: compile_model_test_yang_to_fxs"
#     rm -rf $YDKTEST_MODEL_FXS
#     mkdir $YDKTEST_MODEL_FXS
#     cd $YDKTEST_MODEL_FXS
#     source $CONFD_RC

#     git clone https://github.com/abhikeshav/ydk-test-yang.git
#     cd ydk-test-yang/yang
#     printf "\n"
#     for YANG_FILE in *.yang
#     do
#         `grep "^submodule.*{" $YANG_FILE &> /dev/null`
#         local status=$?
#         if [ $status -eq 1 ]; then
#             printf "Compiling %s to fxs\n" "$YANG_FILE"
#             confdc -c $YANG_FILE
#         fi
#     done

#     mv *.fxs $YDKTEST_MODEL_DEST_FXS

#     cd $YDKGEN_HOME
# }

# # ydk model tests
# function run_ydk_model_tests {
#     print_msg "run_ydk_model_tests"
#     cd $YDKGEN_HOME
#     source test_env/bin/activate

#     printf "\nGenerating ydk model APIs for testing\n"
#     python generate.py --profile profiles/test/ydk-models-test.json --verbose
#     cd $YDKGEN_HOME/gen-api/python
#     pip uninstall -y ydk
#     pip install dist/*.tar.gz
#     export PYTHONPATH=.:$PYTHONPATH
#     for file in $(find ydk/tests -name '*.py'); do
#         source $CONFD_RC
#         confd --status &> /dev/null
#         local status=$?
#         if [ $status -ne 0 ]; then
#             printf "\nRestarting confd\n"
#             source $CONFD_RC
#             cd $YDKTEST_MODEL_DEST_FXS
#             confd -c confd.conf
#             local c_status=$?
#             if [ $c_status -ne 0 ]; then
#                 return $c_status
#             fi
#             cd -
#         fi
#         printf "\nRunning %s python model test\n" "$file"
#         python $file
#     done

#     cd $YDKGEN_HOME
# }

function teardown_env {
    print_msg "teardown_env"
    deactivate
    cd $YDKGEN_HOME && rm -rf gen_env test_env
}

function submit_coverage {
    print_msg "submit_coverage"
    if [[ "$BRANCH" == "master" ]] &&  [[ "$REPO" == *"CiscoDevNet/ydk-gen"* ]]
    then
        coverage report
        pip install coveralls
        export COVERALLS_REPO_TOKEN=MO7qRNCbd9uovAEK2w8Z41lRUgVMi0tbF
        coveralls
    fi
}

function py_tests {
    GEN_ENV="python3"
    TEST_ENV="python3"

    init_env $GEN_ENV $TEST_ENV
    py_sanity_ydktest
    py_sanity_deviation
    py_sanity_augmentation
    teardown_env
    submit_coverage
}

function cpp_tests {
    init_env "python" "python"
    cpp_sanity_core
    cpp_sanity_ydktest
}


########################## EXECUTION STARTS HERE #############################

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR/..

py_tests
cpp_tests
