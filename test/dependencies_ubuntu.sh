#!/bin/bash
#  ----------------------------------------------------------------
# Copyright 2016-2019 Cisco Systems
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
# This file has been modified by Yan Gorelik, YDK Solutions.
# All modifications in original under CiscoDevNet domain
# introduced since October 2019 are copyrighted.
# All rights reserved under Apache License, Version 2.0.
# ------------------------------------------------------------------
#
# Script for running ydk CI on docker via travis-ci.org
# and installing YDK from install_ydk.sh script
#
# dependencies_ubuntu (Ubuntu 16.04, 18.04, 20.04)
# ------------------------------------------------------------------

function print_msg {
    echo -e "${MSG_COLOR}*** $(date) *** dependencies_ubuntu.sh | $* ${NOCOLOR}"
}

function run_cmd {
    local cmd=$@
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

function install_dependencies {
    print_msg "Installing dependencies"

    apt update -y
    apt install sudo -y
    $sudo_cmd apt-get install -y lsb-release
    codename=$(lsb_release -c | awk '{ print $2 }')
    ubuntu_release=$(lsb_release -r | awk '{ print $2 }' | cut -d '.' -f 1)
    if [[ $codename == "focal" || $codename == "jammy" || $codename == "noble" ]] && [[ ! -h /etc/localtime ]]; then
      # Fixing timezone setting issue in focal
      export DEBIAN_FRONTEND=noninteractive
      $sudo_cmd apt-get install -y tzdata
      ln -fs /usr/share/zoneinfo/US/Pacific /etc/localtime
      dpkg-reconfigure --frontend noninteractive tzdata
      unset DEBIAN_FRONTEND
    fi
    run_cmd $sudo_cmd apt-get install -y --no-install-recommends apt-utils
    run_cmd $sudo_cmd apt-get update -y
    run_cmd $sudo_cmd apt-get install -y build-essential
    run_cmd $sudo_cmd apt-get install libtool-bin -y > /dev/null
    local status=$?
    if [[ ${status} != 0 ]]; then
        run_cmd $sudo_cmd apt-get install libtool -y > /dev/null
    fi
    if [[ $codename == "focal" || $codename == "jammy" ]]; then
        $sudo_cmd apt-get install -y mlocate git > /dev/null
        if [[ ! -h /usr/local/lib/libnettle.so.6 ]]; then
          cd /usr/local/lib/
          $sudo_cmd ln -s $curr_dir/3d_party/linux/ubuntu/lib/libnettle.so.6.4
          $sudo_cmd ln -s libnettle.so.6.4 libnettle.so.6
          cd - > /dev/null
        fi
    else
        run_cmd $sudo_cmd apt-get install -y locate git > /dev/null
        run_cmd $sudo_cmd apt-get install -y curl libcurl4-openssl-dev > /dev/null
    fi
    run_cmd $sudo_cmd apt-get install -y bison doxygen flex unzip wget cmake gdebi-core lcov vim > /dev/null
    run_cmd $sudo_cmd apt-get install -y libcmocka0 libpcre3-dev libpcre++ > /dev/null
    run_cmd $sudo_cmd apt-get install -y libssh-dev libxml2-dev libxslt1-dev > /dev/null
    run_cmd $sudo_cmd apt-get install -y python3-dev python3-lxml python3-pip python3-venv > /dev/null
    run_cmd $sudo_cmd apt-get install -y pkg-config software-properties-common zlib1g-dev openjdk-8-jre > /dev/null
    run_cmd $sudo_cmd apt-get install -y valgrind > /dev/null
    if [[ $codename == "focal" || $codename == "jammy" || $codename == "noble" ]]; then
        run_cmd $sudo_cmd apt-get install -y python3-pybind11 --fix-missing > /dev/null
    fi
}

function check_install_python {
    python_version=$(python3 -V | awk '{ print $2 }' | cut -d '.' -f 2)
    if [[ $python_version -gt 11 ]]
    then
      print_msg "YDK does not support system installed Python 3.$python_version"
      python3x_location=$(find /usr/local/bin/ -name 'python3.1[0-1]' | tail -n 1)
      if [ -z $python3x_location ]
      then
        print_msg "Installing Python-3.11.10 from source to alternative location"
        wget https://www.python.org/ftp/python/3.11.10/Python-3.11.10.tgz
        tar zxf Python-3.11.10.tgz
        cd Python-3.11.10 > /dev/null
        ./configure --enable-shared --with-openssl=/usr/local/ssl --with-openssl-rpath=auto --enable-optimizations --with-ssl-default-suites=openssl CFLAGS="-I/usr/local/ssl/include" LDFLAGS="-L/usr/local/ssl"
        make
        make altinstall
        cd - > /dev/null
        print_msg "NOTE! The python3.11 must be used explicitly; buiding venv is required!"
      else
        print_msg "Found local Python3 installation $python3x_location"
      fi
    fi
}

function check_install_gcc_link {
  cd /usr/bin
  if [ -x $1 ]; then
    $sudo_cmd ln -fs $1 $2
  else
    print_msg "Compiler /usr/bin/$1 is not installed!"
    return 1
  fi
  cd - > /dev/null
  return 0
}

function check_install_gcc {
  command -v gcc > /dev/null
  local status_gcc=$?
  command -v g++ > /dev/null
  local status_gxx=$?
  if [[ $status_gcc == 0  && $status_gxx == 0 ]]
  then
    gcc_version=$(gcc --version | head -n 1 | awk '{ print $3 }' | cut -d '-' -f 1)
    print_msg "Current gcc/g++ version is $gcc_version"
    local major=$(echo $gcc_version | cut -d '.' -f 1)
    if [[ $codename == "focal" && $major -gt 7 ]]; then
      print_msg "Installing gcc/g++ version 7"
#       $sudo_cmd add-apt-repository ppa:ubuntu-toolchain-r/test -y
#       $sudo_cmd apt-get update -y > /dev/null
      $sudo_cmd apt-get install gcc-7 g++-7 -y > /dev/null
      gcc_version=$(gcc --version | head -n 1 | awk '{ print $3 }' | cut -d '-' -f 1)
      print_msg "Installed gcc/g++ version is $gcc_version"
      major=$(echo $gcc_version | cut -d '.' -f 1)
    fi
    print_msg "Checking and installing symbolic links for the gcc/g++ compilers"
    run_cmd check_install_gcc_link gcc-$major cc
    run_cmd check_install_gcc_link g++-$major c++
  else
    print_msg "The gcc/g++ not installed"
    exit 1
  fi
}

function check_install_curl {
  if [[ ! -x /usr/local/bin/curl ]]; then
    print_msg "Installing curl from source"
    git clone https://github.com/curl/curl.git -b curl-7_61_1
    cd curl
    ./buildconf
    ./configure --enable-versioned-symbols --with-openssl > /dev/null
    make > /dev/null
    $sudo_cmd make install
    cd - > /dev/null
    $sudo_cmd rm -f /usr/lib/x86_64-linux-gnu/libcurl.so
    $sudo_cmd rm -f /usr/lib/x86_64-linux-gnu/libcurl.so.4
    $sudo_cmd ln -sf /usr/local/lib/libcurl.so /usr/lib/x86_64-linux-gnu/libcurl.so
    $sudo_cmd ln -sf /usr/local/lib/libcurl.so.4 /usr/lib/x86_64-linux-gnu/libcurl.so.4
    rm -rf curl
    print_msg "Installed curl version: $(curl --version | sed 1q | awk '{print$2}')"
  fi
}

function check_install_libssh {
  if [[ $codename == "focal" || $codename == "jammy" || $codename == "noble" ]] && [[ ! -h /usr/local/lib/libssh_threads.so ]]; then
    print_msg "Copying libssh and libssh_threads to /usr/local/lib"
#    sudo cp $curr_dir/3d_party/linux/ubuntu/lib/libssh.so.4.5.0 /usr/local/lib/
    sudo cp $curr_dir/3d_party/linux/ubuntu/lib/libssh_threads.so.4.5.0 /usr/local/lib/
    cd /usr/local/lib/
#    sudo ln -s libssh.so.4.5.0 libssh.so.4
#    sudo ln -s libssh.so.4 libssh.so
    sudo ln -s libssh_threads.so.4.5.0 libssh_threads.so.4
    sudo ln -s libssh_threads.so.4 libssh_threads.so
    cd - > /dev/null
  fi
}

function check_install_openssl {
  if [[ $codename == "jammy" || $codename == "noble" ]] && [[ ! -h /usr/local/ssl/lib/libssl.so ]]
  then
    print_msg "Installing openssl-1.1.1 in /usr/local/ssl"
    cd /usr/local/src/
    sudo git clone https://github.com/openssl/openssl.git -b OpenSSL_1_1_1-stable openssl-1.1.1m
    cd openssl-1.1.1m
    sudo ./config --prefix=/usr/local/ssl --openssldir=/usr/local/ssl shared zlib
    sudo make
    sudo make test
    sudo make install_sw
    cd - > /dev/null
  fi
}

function check_install_go {
  go_exec=$(command -v go)
  if [[ -z ${go_exec} && -d /usr/local/go ]]; then
    go_exec=/usr/local/go/bin/go
  fi
  if [[ -x ${go_exec} ]]
  then
    go_version=$(${go_exec} version | awk '{ print $3 }' | cut -d 'o' -f 2)
    print_msg "Current Go version is $go_version"
    minor=$(echo $go_version | cut -d '.' -f 2)
  else
    print_msg "The Go is not installed"
    minor=0
  fi
  if [ $minor -lt 9 ]; then
    print_msg "Installing Golang version 1.13.1 in /usr/local/go"
    run_cmd wget https://storage.googleapis.com/golang/go1.13.1.linux-amd64.tar.gz
    $sudo_cmd tar -zxf  go1.13.1.linux-amd64.tar.gz -C /usr/local/
    rm -f go1.13.1.linux-amd64.tar.gz
    cd /usr/local/bin
    $sudo_cmd ln -sf /usr/local/go/bin/go
    cd - > /dev/null
  fi
}

function check_install_confd {
  if [[ ! -s $HOME/confd/bin/confd ]]; then
    if [[ $ubuntu_release -lt 20 ]]; then
      print_msg "Installing confd-basic-6.2"
      run_cmd wget https://github.com/CiscoDevNet/ydk-gen/files/562538/confd-basic-6.2.linux.x86_64.zip &> /dev/null
      unzip confd-basic-6.2.linux.x86_64.zip
      run_cmd ./confd-basic-6.2.linux.x86_64.installer.bin $HOME/confd
    else
      print_msg "Installing confd-basic-7.3"
      unzip $curr_dir/3d_party/linux/confd-basic-7.3.linux.x86_64.zip
      cd confd-basic-7.3.linux.x86_64
      run_cmd ./confd-basic-7.3.linux.x86_64.installer.bin $HOME/confd
      cd - > /dev/null
      ln -s $curr_dir/3d_party/linux/ubuntu/lib/libcrypto.so.1.0.0 $HOME/confd/lib/libcrypto.so.1.0.0
      ln -s $curr_dir/3d_party/linux/ubuntu/lib/libncurses.so.5.9 $HOME/confd/lib/libncurses.so.5
      ln -s $curr_dir/3d_party/linux/ubuntu/lib/libtinfo.so.5.9 $HOME/confd/lib/libtinfo.so.5
    fi
    rm -rf confd-basic-* ConfD*
  fi
}

########################## EXECUTION STARTS HERE #############################

# Terminal colors
NOCOLOR="\033[0m"
YELLOW='\033[1;33m'
MSG_COLOR=$YELLOW

print_msg "STARTED .."
sudo_cmd=
if [ $(id -u -n) != "root" ]; then
  sudo_cmd="sudo"
fi

curr_dir=$(pwd)

install_dependencies
check_install_gcc
check_install_openssl
check_install_python

check_install_go

if [[ $ubuntu_release -gt 19 ]]; then
  check_install_curl
fi
check_install_confd

$sudo_cmd updatedb
check_install_libssh
print_msg "FINISHED!"