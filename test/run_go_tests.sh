#!/bin/bash
#  -----------------------------------------------------------------------------
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
# ------------------------------------------------------------------------------
#
# Bash script to install YDK-GO packages and run unit tests
# ------------------------------------------------------------------------------

function usage {
    echo "usage: run_go_tests.sh [-c|--core] [-b|--bundle] [-g|--gnmi] [-h|--help]
Options and arguments:
  -c|--core         build core package and run core tests;
  -b|--bundle       build test bundles and run bundle tests;
  -g|--gnmi         build gnmi package and run gNMI tests
  -h|--help         show script usage
If no arguments are specified, build all the packages and run all the tests"
}

function print_msg {
    echo -e "${MSG_COLOR}*** $(date): run_go_tests.sh: $* ${NOCOLOR}"
}

function run_cmd {
    print_msg "Running: $*"
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

# ------------------------------------------------------------------
# Golang specific functions
#------------------------------------------------------------------

function init_go_env {
    print_msg "Initializing Go environment"

    if [ -z $GOPATH ]; then
       export GOPATH="${HOME}/go"
    fi
    if [ -z $GOROOT ]; then
      export GOROOT="/usr/local/go"
    fi

    print_msg "GOPATH = ${GOPATH}"
    print_msg "GOROOT = ${GOROOT}"

    export PATH=$PATH:$GOPATH/bin
    export PATH=$PATH:$GOROOT/bin

    cd $YDKGEN_HOME

    export CXX=/usr/bin/c++
    export CC=/usr/bin/cc

    print_msg "CC: ${CC}"
    print_msg "CXX: ${CXX}"

    go_version=$(echo `go version` | awk '{ print $3 }' | cut -d 'o' -f 2)
    print_msg "Current Go version is $go_version"

    if [ ! -d $GOPATH/src/github.com/stretchr/testify ]; then
      print_msg "Installing 'testify' package"
      run_cmd go get github.com/stretchr/testify
      run_cmd go get github.com/davecgh/go-spew/spew
      run_cmd go get github.com/pmezard/go-difflib/difflib
      run_cmd go get gopkg.in/yaml.v3
      cd $GOPATH/src/github.com/stretchr/testify
      git checkout tags/v1.6.1
      cd -
    fi

    export CGO_ENABLED=1
    export CGO_LDFLAGS_ALLOW="-fprofile-arcs|-ftest-coverage|--coverage"
    if [[ $go_version > "1.11." ]]; then
        go env -w GO111MODULE=off
    fi
}

function install_go_core {
    print_msg "Installing Go core"
    cd $YDKGEN_HOME

    mkdir -p $GOPATH/src/github.com/CiscoDevNet/ydk-go/ydk
    cp -r sdk/go/core/ydk/* $GOPATH/src/github.com/CiscoDevNet/ydk-go/ydk/
}

function install_go_bundle {
    print_msg "Generating/installing go bundle ydktest"
    cd $YDKGEN_HOME
    run_cmd python3 generate.py --bundle profiles/test/ydktest-cpp.json --go -i
}

function run_go_bundle_tests {
    run_cmd $script_dir/init_test_env.sh
    confd_version=$($HOME/confd/bin/confd --version)

    reset_yang_repository

    run_go_samples
    run_go_sanity_tests
}

function run_go_sanity_tests {
    print_msg "Running go sanity tests"
    cd $YDKGEN_HOME/sdk/go/core/tests
    run_cmd go test
    cd - > /dev/null
}

function run_go_samples {
    print_msg "Running Go samples"

    cd $YDKGEN_HOME/sdk/go/core/samples
    run_cmd go run cgo_path/cgo_path.go
    run_cmd go run bgp_create/bgp_create.go -device ssh://admin:admin@localhost:12022
    run_cmd go run bgp_read/bgp_read.go -device ssh://admin:admin@localhost:12022
    run_cmd go run bgp_delete/bgp_delete.go -device ssh://admin:admin@localhost:12022 -v
    cd -
}

#-----------------------------------------------------------------------------
#  gNMI Tests
#-----------------------------------------------------------------------------

function init_gnmi_server {
    print_msg "Starting YDK gNMI server"
    mkdir -p $YDKGEN_HOME/test/gnmi_server/build && cd $YDKGEN_HOME/test/gnmi_server/build
    $CMAKE_BIN .. && make clean && make
    ./gnmi_server &
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Could not start gNMI server"
        exit $status
    fi
    cd -
}

function stop_gnmi_server {
    print_msg "Stopping gNMI server"
    pkill -f gnmi_server
}

function install_go_gnmi {
    print_msg "Installing Go gNMI package"
    cd $YDKGEN_HOME

    run_cmd python3 generate.py --service profiles/services/gnmi-0.4.0.json --go -i
}

function run_go_gnmi_tests {
    init_gnmi_server

    print_msg "Running Go gNMI tests"

    cd $YDKGEN_HOME/sdk/go/gnmi/tests
    #run_cmd go test
    run_cmd go test -run GnmiServiceProviderTestSuite
    run_cmd go test -run TestGnmiServiceTestSuite
    run_cmd go test -run TestGnmiSessionTestSuite

    run_go_gnmi_samples

    stop_gnmi_server

    cd $YDKGEN_HOME
}

function run_go_gnmi_samples {
    print_msg "Running Go gNMI samples"

    cd $YDKGEN_HOME/sdk/go/gnmi/samples
    run_cmd go run service_subscribe_poll.go < $YDKGEN_HOME/test/gnmi_subscribe_poll_input.txt
    run_cmd go run session_subscribe_poll.go < $YDKGEN_HOME/test/gnmi_subscribe_poll_input.txt
}

########################## EXECUTION STARTS HERE #############################

# Terminal colors
RED='\033[0;31m'
GREEN='\033[1;32m'
NOCOLOR='\033[0m'
YELLOW='\033[1;33m'
MSG_COLOR=$YELLOW

if [[ $# -eq 0 ]]; then
  print_msg "No arguments specified; will run all the tests!"
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

os_type=$(uname)
if [[ ${os_type} == "Linux" ]] ; then
  os_info=$(cat /etc/*-release)
else
  os_info=$(sw_vers)
fi
print_msg "Running OS type: $os_type"
print_msg "OS info: $os_info"

script_dir=$(cd $(dirname ${BASH_SOURCE}) > /dev/null && pwd)

if [ -z ${YDKGEN_HOME} ] || [ ! -d ${YDKGEN_HOME} ]; then
  YDKGEN_HOME=$(cd "$script_dir/../" > /dev/null && pwd)
  print_msg "YDKGEN_HOME is set to ${YDKGEN_HOME}"
fi

if [[ $(uname) == "Linux" && ${os_info} == *"fedora"* ]]; then
  if [[ $LD_LIBRARY_PATH != *"/usr/local/lib:/usr/local/lib64:/usr/lib64"* ]]; then
    export /usr/local/lib:/usr/local/lib64:/usr/lib64:$LD_LIBRARY_PATH
    print_msg "LD_LIBRARY_PATH is set to: $LD_LIBRARY_PATH"
  fi
  centos_version=$(echo `lsb_release -r` | awk '{ print $2 }' | cut -d '.' -f 1)
fi

CMAKE_BIN=cmake
command -v cmake3
status=$?
if [[ ${status} == 0 ]] ; then
    CMAKE_BIN=cmake3
fi

curr_dir="$(pwd)"

cd $YDKGEN_HOME

init_go_env

if [ $build_run_core ]; then
  install_go_core
fi

if [ $build_run_bundle ]; then
  install_go_bundle
  run_go_bundle_tests
fi

if [ $build_run_gnmi ]; then
  install_go_gnmi
  run_go_gnmi_tests
fi

$script_dir/clean_test_env.sh

cd $curr_dir
