#!/bin/bash
#  -----------------------------------------------------------------------
# Copyright 2020 Yan Gorelik, YDK Solutions
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
# ------------------------------------------------------------------------
#
# Bash script to install YDK-PY packages and run unit tests
#
# ------------------------------------------------------------------------

function usage {
    MSG_COLOR=$NOCOLOR
    echo "usage: run_py_tests.sh [-c|--core] [-b|--bundle] [-g|--gnmi] [-h|--help]
Options and arguments:
  -c|--core         build core package and run core tests;
  -b|--bundle       build test bundles and run bundle tests
  -g|--gnmi         build gnmi package and run gNMI tests
  -h|--help         show script usage
If no arguments are specified, build all the packages and run all the tests"
}

function print_msg {
    echo -e "\n${MSG_COLOR}*** $(date): run_py_test.sh: $* ${NOCOLOR}"
}

function run_cmd {
    $*
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Command '$*' FAILED with status=$status"
        exit $status
    fi
    return $status
}

function reset_yang_repository {
    if [[ ! -d $HOME/.ydk/127.0.0.1 ]]; then
      mkdir -p $HOME/.ydk
      mkdir -p $HOME/.ydk/127.0.0.1
    fi
    rm -f $HOME/.ydk/127.0.0.1/*

    # Correct issue with confd 7.3
    if [[ $confd_version. > "7.2." ]]; then
      cp ${YDKGEN_HOME}/sdk/cpp/core/tests/models/ietf-interfaces.yang $HOME/.ydk/127.0.0.1/
    fi
}

function init_confd_rc {
    print_msg "Initializing confd"

    confd_rc=$(find $HOME/confd -name confdrc)
    if [[ -z $confd_rc ]]; then
        MSG_COLOR=$RED
        print_msg "Cannot find confdrc resource file in user file system. Exiting"
        exit 1
    fi
    print_msg "Found confd resource file: $confd_rc"
    run_cmd source $confd_rc
    confd_version=$(confd --version)
}

function init_confd {
    print_msg "Initializing confd in $1"

    cd $1
    run_cmd make stop > /dev/null
    run_cmd make clean > /dev/null
    run_cmd make all > /dev/null
    run_cmd make start
    cd - > /dev/null

    reset_yang_repository
}

function init_confd_ydktest {
    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/ydktest
}

function init_rest_server {
    print_msg "Starting rest server"
    $YDKGEN_HOME/test/start_rest_server.sh
    export REST_SERVER_PID=$moco_pid
}

function init_tcp_server {
    print_msg "Starting TCP server"
    $YDKGEN_HOME/test/start_tcp_server.sh
    export TCP_SERVER_PID=$tcp_pid
}

function stop_tcp_server {
    if [[ ! -z $TCP_SERVER_PID ]]; then
        print_msg "Stopping TCP server PID $TCP_SERVER_PID"
        kill $TCP_SERVER_PID
    fi
}

function stop_rest_server {
    if [[ ! -z $REST_SERVER_PID ]]; then
        print_msg "Stopping REST server PID $REST_SERVER_PID"
        kill $REST_SERVER_PID
        unset REST_SERVER_PID
    fi
}

function run_test {
    print_msg "Executing: $*"
    ${PYTHON_BIN} $*
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Command '${PYTHON_BIN} $*' FAILED with status=$status"
        exit $status
    fi
    return $status
}

function init_test_env {
    init_confd_rc
    init_confd_ydktest
    init_rest_server
    init_tcp_server
}

# ------------------------------------------------------------------
# Python specific functions
# ------------------------------------------------------------------

function init_python_env {
  PYTHON_BIN=python3
  PIP_BIN=pip3

  print_msg "Checking python version and installation"
  $PYTHON_BIN --version
  status=$?
  if [ $status -ne 0 ]; then
    MSG_COLOR=$RED
    print_msg "Could not locate python3 interpretor"
    exit $status
  fi
  print_msg "Checking pip version and installation"
  $PIP_BIN -V
  status=$?
  if [ $status -ne 0 ]; then
    MSG_COLOR=$RED
    print_msg "Could not locate ${PIP_BIN}"
    exit $status
  fi
  print_msg "Python location: $(which ${PYTHON_BIN})"
  print_msg "Pip location: $(which ${PIP_BIN})"
}

function pip_check_install {
    sudo -H ${PIP_BIN} install $* -U
}

function install_py_core {
    print_msg "Installing Python YDK core package"
    cd $YDKGEN_HOME/sdk/python/core
    rm -rf dist
    ${PYTHON_BIN} setup.py sdist
    ${PIP_BIN} install dist/ydk*.tar.gz -U
    cd $YDKGEN_HOME

    #print_msg "Generating py binaries"
    #sudo -H ./generate_python_binary.sh
    #cd $YDKGEN_HOME
}

function run_python_bundle_tests {
    print_msg "Removing cache directory ./gen-api/.cache"
    sudo rm -rf $YDKGEN_HOME/gen-api/.cache
    py_sanity_ydktest
    py_sanity_deviation
    py_sanity_augmentation
    py_sanity_common_cache
    py_sanity_one_class_per_module
}

#--------------------------
# Python ydktest bundle
#--------------------------

function py_sanity_ydktest {
    py_sanity_ydktest_gen
    py_sanity_ydktest_test

    #py_sanity_doc_gen
}

function py_sanity_doc_gen {
    print_msg "Generating docs"
    #run_test generate.py --core --cpp --generate-doc &> /dev/null
    #run_test generate.py --core --go --generate-doc &> /dev/null
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydktest-cpp.json
    run_test generate.py --core --python --generate-doc
}

function py_sanity_ydktest_gen {
    print_msg "Generating and installing YDK bundle ydk-models-ydktest"
    cd $YDKGEN_HOME

    run_test generate.py --python --bundle profiles/test/ydktest-cpp.json -i
}

function py_sanity_ydktest_test {
    init_test_env
    print_msg "Running test_sanity_codec tests"

    cd $YDKGEN_HOME
    run_test sdk/python/core/tests/test_sanity_codec.py

    py_sanity_ydktest_test_netconf_ssh

    if [ ! -f /.dockerenv ]; then
      py_sanity_ydktest_test_tcp	# This test fails in docker
    fi
}

function py_sanity_ydktest_test_netconf_ssh {
    print_msg "Running py_sanity_ydktest_test_netconf_ssh"

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
    run_test sdk/python/core/tests/test_non_top_operations.py
    run_test sdk/python/core/tests/test_sanity_executor_rpc.py
}

function py_sanity_ydktest_test_tcp {
    print_msg "Running py_sanity_ydktest_test_tcp"
    run_test sdk/python/core/tests/test_sanity_netconf.py tcp://admin:admin@127.0.0.1:12307
    # init_confd_ydktest
    # print_msg "Running py_sanity_ydktest_test_tcp with on-demand=false"
    # run_test sdk/python/core/tests/test_sanity_netconf.py tcp://admin:admin@127.0.0.1:12307 --non-demand
}

#--------------------------
# Python deviation tests
#--------------------------

function py_sanity_deviation {
    print_msg "Running py_sanity_deviation"

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/deviation

    py_sanity_deviation_ydktest_test

    py_sanity_deviation_bgp_test
}

function py_sanity_deviation_ydktest_test {
    print_msg "Running py_sanity_deviation_ydktest_test"
    run_test generate.py --bundle profiles/test/ydktest-cpp.json -i
    run_test sdk/python/core/tests/test_sanity_deviation.py
    run_test sdk/python/core/tests/test_sanity_deviation.py --non-demand
}

function py_sanity_deviation_bgp_test {
    print_msg "Running py_sanity_deviation_bgp_test"
    run_test generate.py --bundle profiles/test/deviation.json -i
    run_test sdk/python/core/tests/test_sanity_deviation_bgp.py
    run_test sdk/python/core/tests/test_sanity_deviation_bgp.py --non-demand
}

#--------------------------
# Python augmentation tests
#--------------------------

function py_sanity_augmentation {
    print_msg "Running py_sanity_augmentation_test"
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydktest-augmentation.json -i
    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/augmentation

    run_test sdk/python/core/tests/test_sanity_augmentation.py
    run_test sdk/python/core/tests/test_sanity_augmentation.py --non-demand
    run_test sdk/python/core/tests/test_on_demand.py
}

function py_sanity_common_cache {
    print_msg "Running py_sanity_common_cache"

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/deviation
    run_test sdk/python/core/tests/test_sanity_deviation.py --common-cache

    init_confd $YDKGEN_HOME/sdk/cpp/core/tests/confd/augmentation
    run_test sdk/python/core/tests/test_sanity_augmentation.py --common-cache

    init_confd_ydktest
    run_test sdk/python/core/tests/test_sanity_levels.py --common-cache
    run_test sdk/python/core/tests/test_sanity_types.py --common-cache
}

function py_sanity_one_class_per_module {
    print_msg "Running Running one class per module tests"
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydktest.json -oi

    init_confd_ydktest
    run_test sdk/python/core/tests/test_sanity_levels.py
    run_test sdk/python/core/tests/test_sanity_types.py

    run_test generate.py --bundle profiles/test/ydktest-cpp.json -i
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
    ${PYTHON_BIN} import_tests.py
    cd models_test/
    ${PYTHON_BIN} -m unittest discover
}

function py_test_gen {
    print_msg "py_test_gen"

    cd $YDKGEN_HOME
    run_test generate.py -is --core --python
    run_test generate.py --bundle profiles/test/ydk-models-test.json  --generate-tests --python &> /dev/null
    pip_check_install gen-api/python/models_test-bundle/dist/ydk*.tar.gz

    # py_test_gen_test
}

function run_python_oc_nis_tests {
    print_msg "Installing ydktest_oc_nis bundle"
    cd $YDKGEN_HOME
    run_test generate.py --bundle profiles/test/ydktest-oc-nis.json
    pip_check_install gen-api/python/ydktest_oc_nis-bundle/dist/ydk*.tar.gz

    print_msg "Running ydktest_oc_nis bundle tests"
    run_test sdk/python/core/tests/test_oc_nis.py
}

function build_and_run_python_gnmi_tests {
    build_python_gnmi_package
    run_python_gnmi_tests
}

function build_python_gnmi_package {
    print_msg "Installing gNMI package for Python"
    cd $YDKGEN_HOME
    run_test generate.py -i --service profiles/services/gnmi-0.4.0.json
}

function run_python_gnmi_tests {
    print_msg "Run Python gNMI tests"

    start_gnmi_server

    run_test sdk/python/gnmi/tests/test_gnmi_session.py
    run_test sdk/python/gnmi/tests/test_gnmi_crud.py
    run_test sdk/python/gnmi/tests/test_gnmi_service.py < $YDKGEN_HOME/test/gnmi_subscribe_poll_input.txt

    stop_gnmi_server
}

function start_gnmi_server {
    current_dir="$(pwd)"
    cd $YDKGEN_HOME/test/gnmi_server
    if [ ! -x ./build/gnmi_server ]; then
        print_msg "Building YDK gNMI server"
        mkdir -p build && cd build
        ${CMAKE_BIN} .. && make
    fi

    print_msg "Starting YDK gNMI server"
    cd $YDKGEN_HOME/test/gnmi_server/build
    ./gnmi_server &
    local status=$?
    if [ $status -ne 0 ]; then
        print_msg "Could not start gNMI server"
        exit $status
    fi
    cd $current_dir
}

function stop_gnmi_server {
    print_msg "Stopping gNMI server"
    pkill -f gnmi_server
}

#-------------------------------------
# Meta-data test
#-------------------------------------

function run_py_metadata_test {
    print_msg "Building ydktest bundle with metadata"
    cd $YDKGEN_HOME
    run_test generate.py -i --bundle profiles/test/ydktest-cpp.json --generate-meta

    print_msg "Running metadata test"
    run_test sdk/python/core/tests/test_meta.py
}

#-------------------------------------
# Script environment setup
#-------------------------------------

function init_script_env {
    curr_dir="$(pwd)"
    os_type=$(uname)
    if [[ ${os_type} == "Linux" ]]; then
        os_info=$(cat /etc/*-release)
    elif [[ ${os_type} == "Darwin" ]]; then
        os_info=$(sw_vers)
    else
        MSG_COLOR=${RED}
        print_msg "Unsupported OS type '${os_type}' detected"
        exit 1
    fi
    print_msg "Running OS type: $os_type"
    print_msg "OS info: $os_info"

    CMAKE_BIN=cmake
    which cmake3
    status=$?
    if [[ ${status} == 0 ]] ; then
        CMAKE_BIN=cmake3
    fi

    script_dir=$(cd $(dirname ${BASH_SOURCE}) && pwd)
    if [ -z ${YDKGEN_HOME} ] || [ ! -d ${YDKGEN_HOME} ]; then
        export YDKGEN_HOME=$(cd $script_dir/.. && pwd)
        print_msg "YDKGEN_HOME is set to ${YDKGEN_HOME}"
    fi

    if [[ $(uname) == "Linux" && ${os_info} == *"fedora"* ]]; then
      if [[ $LD_LIBRARY_PATH != *"/usr/local/lib:/usr/local/lib64:/usr/lib64"* ]]; then
        export /usr/local/lib:/usr/local/lib64:/usr/lib64:$LD_LIBRARY_PATH
        print_msg "LD_LIBRARY_PATH is set to: $LD_LIBRARY_PATH"
      fi
      centos_version=$(echo `lsb_release -r` | awk '{ print $2 }' | cut -d '.' -f 1)
    fi

    cd $YDKGEN_HOME
}

########################## EXECUTION STARTS HERE #############################
#
RED='\033[0;31m'
GREEN='\033[1;32m'
NOCOLOR='\033[0m'
YELLOW='\033[1;33m'
MSG_COLOR=$YELLOW

if [[ $# -eq 0 ]]; then
  print_msg "No arguments specified; will build all the packages and run all the tests!"
  build_run_core=1
  build_run_bundle=1
  build_run_gnmi=1
fi

# As long as there is at least one more argument, keep looping
while [[ $# -gt 0 ]]; do
    key="$1"
    case "$key" in
        # This is a flag type option. Will catch either -f or --foo
        -c|--core)
            build_run_core=1
            ;;
        -b|--bundle)
            build_run_bundle=1
            ;;
        -g|--gnmi)
            build_run_gnmi=1
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option '$key'"
            usage
            exit 1
            ;;
    esac
    # Shift after checking all the cases to get the next option
    shift
done

init_script_env
init_python_env

if [ $build_run_core ]; then
  install_py_core
fi

if [ $build_run_bundle ]; then
  run_python_bundle_tests
  run_py_metadata_test
fi

if [ $build_run_gnmi ]; then
  build_and_run_python_gnmi_tests
fi

$script_dir/clean_test_env.sh

cd $curr_dir
