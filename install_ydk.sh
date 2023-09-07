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
# Bash script to install ydk
#
# For usage run: ./install_ydk -h
# ------------------------------------------------------------------------------

function print_msg {
    echo -e "${MSG_COLOR}*** $(date) *** install_ydk.sh | $* ${NOCOLOR}"
}

function run_cmd {
    local cmd=$*
    print_msg "Running: $cmd"
    $*
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Exiting '$*' with status=$status"
        exit $status
    fi
    return $status
}

function usage {
    MSG_COLOR=$NOCOLOR
    echo "usage: install_ydk [ {--cpp|--py|--go|--all} ] [-c] [-s gnmi] [-h] [-n] [-v]
Options and arguments:
  --cpp                 install YDK for C++ programming language;
                        requires sudo access for dependencies and libraries installation
  --go                  install YDK for Go programming language
  --py|--python         install YDK for Python programming language
  --all                 install YDK for all available programming languages;
                        requires sudo access for dependencies and libraries installation
  -v|--venv             create python virtual environment
  -c|--core             install YDK core package
  -s|--service gnmi     install gNMI service package
  -n|--no-deps          skip installation of dependencies;
                        applicable only with --cpp and --all options
  -h|--help             print this help message and exit

Environment variables:
YDKGEN_HOME         specifies location of ydk-gen git repository;
                    if not set, \$HOME/ydk-gen is assumed
PYTHON_VENV         specifies location of python virtual environment;
                    if not set, \$HOME/venv is assumed
GOROOT              specifies installation directory of go software;
                    if not set, /usr/local/go is assumed
GOPATH              specifies location of go source directory;
                    if not set, \$HOME/go is assumed
C_INCLUDE_PATH      location of C include files;
                    if not set, /usr/local/include is assumed
CPLUS_INCLUDE_PATH  location of C++ include files;
                    if not set, /usr/local/include is assumed
CMAKE_LIBRARY_PATH  Location of Python shared libraries;
                    if not set, default system library location is assumed"
}

function activate_python_venv() {
  if [[ -z ${PYTHON_VENV} ]]; then
    PYTHON_VENV=${HOME}/venv
    print_msg "Python virtual environment location is set to ${PYTHON_VENV}"
  fi
  if [[ ! -d ${PYTHON_VENV} ]]; then
    print_msg "Creating Python3 virtual environment in ${PYTHON_VENV}"
    run_cmd python3 -m venv "${PYTHON_VENV}"
  fi
  run_cmd source "${PYTHON_VENV}"/bin/activate
}

function init_py_env {
  if [ $install_venv == "yes" ]; then
    activate_python_venv
  fi
  print_msg "Checking python3 version and installation"
  python3 --version
  status=$?
  if [ $status -ne 0 ]; then
    MSG_COLOR=$RED
    print_msg "Could not locate python3 interpreter"
    exit $status
  fi

  print_msg "Checking pip3 version and installation"
  pip3 -V
  status=$?
  if [ $status -ne 0 ]; then
    MSG_COLOR=$RED
    print_msg "Could not locate pip3"
    exit $status
  fi
  export PIP_DISABLE_PIP_VERSION_CHECK=1

  if [[ $ydk_lang == "py" || $ydk_lang == "all" ]]; then
    print_msg "Checking installation of python shared libraries"
    ver=$(python3 -c "import sys;print('{}.{}'.format(sys.version_info.major,sys.version_info.minor))")
    ver_minor=$(python3 -c "import sys;print('{}'.format(sys.version_info.minor))")
    os_type=$(uname)
    if [[ ${os_type} == "Linux" ]]; then
      if [ $ver_minor -gt 7 ]; then
        ext="$ver.so"
      else
        ext="$ver"m.so
      fi
    elif [[ ${os_type} == "Darwin" ]]; then
      ext="$ver.dylib"
    fi
    lines=($(locate libpython$ext))
    if [[ ${#lines[@]} -gt 0 ]] && [[ -x ${lines[0]} || -L ${lines[0]} ]]; then
      if [[ -z $CMAKE_LIBRARY_PATH ]]; then
        export CMAKE_LIBRARY_PATH=$(dirname ${lines[0]})
        print_msg "Setting CMAKE_LIBRARY_PATH to $CMAKE_LIBRARY_PATH"
      fi
    else
      print_msg "Could not locate python shared library libpython$ext"
      print_msg "Try to update locate database, then repeat YDK installation"
      exit 1
    fi
  fi
  print_msg "Checking and installing Python requirements"
  pip3 install -r requirements.txt
}

function init_go_env {
  if [[ ${ydk_lang} == "go" || ${ydk_lang} == "all" ]]; then
    print_msg "Initializing Go environment"

    if [[ $(uname) == "Darwin" ]]; then
        if [[ $GOROOT. == "." ]]; then
            export GOROOT=/usr/local/go
        fi
        print_msg "GOROOT: $GOROOT"
    else
        if [[ $GOROOT. == "." ]]; then
            export GOROOT=/usr/local/go
            print_msg "Setting GOROOT to $GOROOT"
        else
            print_msg "GOROOT: $GOROOT"
        fi
        export PATH=$GOROOT/bin:$PATH
    fi
    if [[ -z $GOPATH ]]; then
        export GOPATH=$HOME/go
        mkdir -p $GOPATH
        print_msg "Setting GOPATH to $GOPATH"
    else
        print_msg "GOPATH: $GOPATH"
    fi
    go_version=$(echo `go version` | awk '{ print $3 }' | cut -d 'o' -f 2)
    print_msg "Current Go version is $go_version"

    if [ ! -d $GOPATH/src/github.com/stretchr/testify ]; then
        go get github.com/stretchr/testify
        cd $GOPATH/src/github.com/stretchr/testify
        git checkout tags/v1.6.1
        cd -
    fi

    export CGO_ENABLED=1
    export CGO_LDFLAGS_ALLOW="-fprofile-arcs|-ftest-coverage|--coverage"
  fi
}

function install_cpp_core {
    print_msg "Installing C++ core library"
    cd $YDKGEN_HOME
    local cpp_sudo_flag
    if [ $(id -u -n) != "root" ]; then cpp_sudo_flag="s"; fi
    run_cmd python3 generate.py --core --cpp -i$cpp_sudo_flag
}

function install_cpp_gnmi {
    print_msg "Building C++ core gnmi library"
    cd $YDKGEN_HOME
    local cpp_sudo_flag
    if [ $(id -u -n) != "root" ]; then cpp_sudo_flag="s"; fi
    run_cmd python3 generate.py --service profiles/services/gnmi-0.4.0.json --cpp -i$cpp_sudo_flag
}

function install_go_core {
    print_msg "Installing Go core packages"
    cd $YDKGEN_HOME
    run_cmd python3 generate.py -i --core --go
}

function install_go_gnmi {
    print_msg "Installing Go gNMI service package"
    cd $YDKGEN_HOME
    run_cmd python3 generate.py -i --service profiles/services/gnmi-0.4.0.json --go
}

function install_py_core {
    print_msg "Building and installing Python core package"
    cd $YDKGEN_HOME
    run_cmd python3 generate.py -i --core

    print_msg "Verifying Python YDK core package installation"
    python3 -c "from ydk.path import NetconfSession"
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Verification failed for the Python core package 'ydk_.so'"
        exit $status
    fi
}

function install_py_gnmi {
    print_msg "Installing gNMI package for Python"
    cd $YDKGEN_HOME
    run_cmd python3 generate.py -i --service profiles/services/gnmi-0.4.0.json

    print_msg "Verifying Python gNMI package installation"
    python3 -c "from ydk.gnmi.path import gNMISession"
    local status=$?
    if [ $status -ne 0 ]; then
        MSG_COLOR=$RED
        print_msg "Verification failed for the Python gNMI package 'ydk_gnmi_.so'"
        exit $status
    fi
}

function instal_dependencies {
    if [ ${os_type} == "Linux" ]; then
      if [[ ${os_info} == *"Ubuntu"* ]]; then
        run_cmd ${YDKGEN_HOME}/test/dependencies_ubuntu.sh
      else
        run_cmd ${YDKGEN_HOME}/test/dependencies_centos.sh
      fi
      if [ ${service_pkg} == "gnmi" ]; then
        run_cmd ${YDKGEN_HOME}/test/dependencies_linux_gnmi.sh
      fi
    else    # Darwin
      run_cmd ${YDKGEN_HOME}/test/dependencies_osx.sh
      if [ ${service_pkg} == "gnmi" ]; then
        run_cmd ${YDKGEN_HOME}/test/dependencies_osx_gnmi.sh
      fi
    fi
}

function test_libydk {
    [ -f "/usr/local/lib/$libydk_path" ] && [[ "$(readlink /usr/local/lib/libydk.a)" == "$libydk_path" ]] && return 0

    MSG_COLOR=$RED
    print_msg "Missing C++ core library '$libydk_path'. Install it first!"
    exit 1
}

function test_libydk_gnmi {
    [ -f "/usr/local/lib/$libydk_gnmi_path" ] && [[ "$(readlink /usr/local/lib/libydk_gnmi.a)" == "$libydk_gnmi_path" ]] && return 0

    MSG_COLOR=$RED
    print_msg "Missing C++ gNMI service library '$libydk_gnmi_path'. Install it first!"
    exit 1
}

function install_ydk_cpp {
  if [[ ${ydk_lang} == "cpp" || ${ydk_lang} == "all" ]]; then
    if [[ ${core_package} == "yes" ]]; then
        install_cpp_core
    fi
    if [[ ${service_pkg} == "gnmi" ]]; then
      if test_libydk; then
        install_cpp_gnmi
      fi
    fi
  fi
}

function install_ydk_py {
    if [[ ${ydk_lang} == "py" || ${ydk_lang} == "all" ]]; then
      if test_libydk; then
        if [[ ${core_package} == "yes" ]]; then
            install_py_core
        fi
        if [[ ${service_pkg} == "gnmi" ]]; then
          if test_libydk_gnmi; then
            install_py_gnmi
          fi
        fi
      fi
    fi
}

function install_ydk_go {
    if [[ ${ydk_lang} == "go" || ${ydk_lang} == "all" ]]; then
      if test_libydk; then
        if [[ ${core_package} == "yes" ]]; then
            install_go_core
        fi
        if [[ ${service_pkg} == "gnmi" ]]; then
          if test_libydk_gnmi; then
            install_go_gnmi
          fi
        fi
      fi
    fi
}

function write_env_file {
  print_msg "Writing .env file"
  cd "${YDKGEN_HOME}"
  rm -f .env
  echo "# ------------------------------------------------------------------
# This file has been auto-generated during YDK-$ydk_version installation.
# PLEASE, DO NOT CHANGE THIS FILE MANUALLY!
# ------------------------------------------------------------------
# Run this command once when entering YDK-GEN environment in bash:
#     source .env
# ------------------------------------------------------------------

export YDKGEN_HOME=\"${YDKGEN_HOME}\"

export C_INCLUDE_PATH=\"$C_INCLUDE_PATH\"
export CPLUS_INCLUDE_PATH=\"$CPLUS_INCLUDE_PATH\"
export PIP_DISABLE_PIP_VERSION_CHECK=1
alias python=python3
alias pip=pip3
" > .env
  if [[ -n $LD_LIBRARY_PATH ]]; then
    echo "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH
" >> .env
  fi
  if [ $install_venv == "yes" ]; then
    echo "export PYTHON_VENV=$PYTHON_VENV" >> .env
    echo "source $PYTHON_VENV/bin/activate" >> .env
  elif [[ -n $python_location ]]; then
    echo "export PATH=$python_location/bin:\$PATH
" >> .env
  fi
  if [[ -n $sudo_cmd ]]; then
    echo "export SUDO_CMD=$sudo_cmd" >> .env
  fi
  if [[ $ydk_lang == "go" || $ydk_lang == "all" ]]; then
    if [[ ${os_type} == "Linux" ]]; then
      echo "
if [ -z \$GOROOT ]; then
    export GOROOT=$GOROOT
    if [[ $PATH != *\"$GOROOT/bin\"* ]]; then
        export PATH=$GOROOT/bin:\$PATH
    fi
fi
" >> .env
    fi
    echo "
if [ -z \$GOPATH ]; then
    export GOPATH=$HOME/go
fi
export CXX=/usr/bin/c++
export CC=/usr/bin/cc
export CGO_ENABLED=1
export CGO_LDFLAGS_ALLOW=\"-fprofile-arcs|-ftest-coverage|--coverage\"
if [[ \$go_version > \"1.11.\" ]]; then
    go env -w GO111MODULE=off
fi
" >> .env
  fi
  if [[ -n $CMAKE_LIBRARY_PATH ]]; then
    echo "export CMAKE_LIBRARY_PATH=$CMAKE_LIBRARY_PATH
" >> .env
  fi
#  cat .env
  cd - >& /dev/null
}

########################## EXECUTION STARTS HERE #############################

# Terminal colors
NOCOLOR="\033[0m"
RED="\033[0;31m"
YELLOW='\033[1;33m'
MSG_COLOR=${YELLOW}

######################################
# Parse script options

ydk_lang="py"
service_pkg="no"
core_package="no"
dependencies="yes"
install_venv="no"
sudo_flag=
sudo_cmd=

# As long as there is at least one more argument, keep looping
while [[ $# -gt 0 ]]; do
    key="$1"
    case "$key" in
        # This is a flag type option. Will catch either -f or --foo
        -l|--lang)
            shift # past the key to the value
            ydk_lang="$1"
            if [[ ${ydk_lang} != "cpp" && ${ydk_lang} != "py" && ${ydk_lang} != "go" && ${ydk_lang} != "all"  ]]; then
                echo "Unknown language ${ydk_lang}"
                usage
                exit 1
            fi
            ;;
        --cpp)
            ydk_lang="cpp"
            ;;
        --go)
            ydk_lang="go"
            dependencies="no"
            ;;
        --py|--python)
            ydk_lang="py"
            dependencies="no"
            ;;
        --all)
            ydk_lang="all"
            ;;
        -n|--no-deps)
            dependencies="no"
            ;;
        -h|--help)
            usage
            exit 1
            ;;
        -c|--core)
            core_package="yes"
            ;;
        -s|--service)
            shift # past the key and to the value
            service_pkg="$1"
            if  [[ ${service_pkg} != "gnmi" ]]; then
                echo "Unknown service package specified; gnmi assumed"
                service_pkg="gnmi"
            fi
            ;;
        -v|--venv)
            install_venv="yes"
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

curr_dir=$(pwd)
script_dir=$(cd $(dirname ${BASH_SOURCE}) && pwd)
if [[ -z ${YDKGEN_HOME} || ! -d ${YDKGEN_HOME} ]]; then
    YDKGEN_HOME=$script_dir
    print_msg "YDKGEN_HOME is set to $script_dir"
fi
cd ${YDKGEN_HOME}

ydk_version=$(grep core sdk/version.json | awk '{print($2)'} | tr -d '"' | tr -d ',')
gnmi_version=$(grep gnmi-service sdk/version.json | awk '{print($2)'} | tr -d '"' | tr -d ',')
libydk_path="libydk-$ydk_version.a"
libydk_gnmi_path="libydk_gnmi-$gnmi_version.a"
echo "YDK-$ydk_version installation options:"
if [ ${install_venv} == "no" ]; then
  if [[ -n $python_location ]]; then
    echo " - use custom Python installation in ${python_location}"
  else
    echo " - use system Python installation"
  fi
else
  echo " - create Python virtual environment = ${install_venv}"
fi
echo " - programming language = ${ydk_lang}"
echo " - install dependencies = ${dependencies}"
echo " - install core package = ${core_package}"
echo " - install service package = ${service_pkg}"
if [[ ${core_package} == "no" && ${service_pkg} == "no" && ${dependencies} == "no" ]]; then
    echo "There are no components to install"
    exit 0
fi

######################################
# Set up installation environment

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
if [[ ${os_type} == "Linux" ]]; then
  if [[ ${os_info} == *"Ubuntu"* ]]; then
    if [[ ${os_info} != *"xenial"* && ${os_info} != *"bionic"* && ${os_info} != *"focal"* && ${os_info} != *"jammy"* ]]; then
        print_msg "WARNING! Unsupported Ubuntu distribution found. Will try the best efforts."
    fi
  elif [[ ${os_info} == *"fedora"* ]]; then
    rhel_version=$(echo "${os_info}" | grep -w VERSION_ID | awk -F[\"] '{print $2}')
    if [[ $rhel_version != 7 && $rhel_version != 8 ]]; then
        print_msg "WARNING! Unsupported Centos/RHEL version. Will try the best efforts."
    fi
    if [[ $rhel_version == 8 && ${os_info} == *"CentOS"* ]]; then
      os_name=$(echo "${os_info}" | grep -w NAME | awk -F[\"] '{print $2}')
      if [[ $os_name != *"Stream" ]]; then
        print_msg "Unsupported CentOS version 8 (EOL). Will try the best efforts."
      fi
    fi
  else
    MSG_COLOR=${RED}
    print_msg "Unsupported Linux distribution detected"
    exit 1
  fi
fi

if [[ -z ${C_INCLUDE_PATH} ]]; then
    export C_INCLUDE_PATH=/usr/local/include
fi
if [[ -z ${CPLUS_INCLUDE_PATH} ]]; then
    export CPLUS_INCLUDE_PATH=/usr/local/include
fi

if [[ ${os_info} == *"fedora"* || ${os_info} == *"jammy"* ]]; then
  if [[ $LD_LIBRARY_PATH != *"/usr/local/lib"* ]]; then
    export LD_LIBRARY_PATH="/usr/local/lib:$LD_LIBRARY_PATH"
  fi
  if [[ $LD_LIBRARY_PATH != *"/lib64"* && -d /usr/lib64 ]]; then
    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib64:/usr/lib64
  fi
  print_msg "LD_LIBRARY_PATH is set to: $LD_LIBRARY_PATH"
fi

if [ ${dependencies} == "yes" ]; then
    instal_dependencies
fi
if [ -f ~/.profile.python ]; then
  print_msg "Reading python profile ~/.profile.python"
  source ~/.profile.python
fi

CMAKE_BIN=cmake
command -v cmake3 > /dev/null
status=$?
if [[ ${status} == 0 ]]; then
    CMAKE_BIN=cmake3
fi

######################################
# Start installation

init_py_env
init_go_env

write_env_file

install_ydk_cpp

install_ydk_py

install_ydk_go

if [ ${install_venv} == "yes" ]; then
  deactivate
fi

cd ${curr_dir}
