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

function usage {
    printf "\n    Usage: Use this script to create YDK repositories (ydk-py, ydk-go etc) hosted on github, generated using ydk-gen. Specify the language to create the SDK and the path to where the github SDK is cloned \n\n      ./%s -l <[one of python|cpp|go]> -s <PATH-TO-SDK>\n\n" "$( basename "${BASH_SOURCE[0]}" )"
}

function check_gen_api_directories {
GEN_API_CONTENTS=$(ls ${1} &> /dev/null)
if [[ ${GEN_API_CONTENTS} != *"cisco_ios_xe-bundle"* ]] || [[ ${GEN_API_CONTENTS} != *"cisco_ios_xr-bundle"* ]] \
            || [[ ${GEN_API_CONTENTS} != *"openconfig-bundle"* ]] || [[ ${GEN_API_CONTENTS} != *"ietf-bundle"* ]] \
             || [[ ${GEN_API_CONTENTS} != *"cisco_nx_os-bundle"* ]] || [[ ${GEN_API_CONTENTS} != *"ydk"* ]]; then
    printf "\n    All packages have not been generated.\n\
        Please run './generate.py --${LANGUAGE} --bundle profile/bundles/<profile>.json' for all desired bundles.\n\
        Run './generate.py --${LANGUAGE} --core' for core\n\n"
    exit 1
fi
}

function copy_bundle_packages {
    echo "Copying ietf from ${GEN_API_PATH}/ietf-bundle/ to ${SDK_PATH}"
    cp -r ${GEN_API_PATH}/ietf-bundle/* ${SDK_PATH}/ietf

    echo "Copying openconfig from ${GEN_API_PATH}/openconfig-bundle/ to ${SDK_PATH}"
    cp -r ${GEN_API_PATH}/openconfig-bundle/* ${SDK_PATH}/openconfig

    echo "Copying cisco-ios-xr from ${GEN_API_PATH}/cisco_ios_xr-bundle/ to ${SDK_PATH}"
    cp -r ${GEN_API_PATH}/cisco_ios_xr-bundle/* ${SDK_PATH}/cisco-ios-xr

    echo "Copying cisco-ios-xe from ${GEN_API_PATH}/cisco_ios_xe-bundle/ to ${SDK_PATH}"
    cp -r ${GEN_API_PATH}/cisco_ios_xe-bundle/* ${SDK_PATH}/cisco-ios-xe

    echo "Copying cisco-ios-nx from ${GEN_API_PATH}/cisco_ios_nx-bundle/ to ${SDK_PATH}"
    cp -r ${GEN_API_PATH}/cisco_ios_nx-bundle/* ${SDK_PATH}/cisco-ios-nx
}

function copy_readme {
    echo "Copying README from ${SDK_STUB_PATH} to ${SDK_PATH}"
    cp -r ${SDK_STUB_PATH}/README.${1} ${SDK_PATH}
}

function copy_gen_api_to_go_sdk {
    GO_GEN_API_DIR=${1}
    echo "Copying ${GO_GEN_API_DIR} from ${GEN_API_PATH}/${GO_GEN_API_DIR}/ to ${SDK_PATH}"
    cp -r ${GEN_API_PATH}/${GO_GEN_API_DIR}/ydk/* ${SDK_PATH}/ydk
}

function clear_sdk_directories {
    echo "Clearing ${SDK_PATH} of existing files"
    rm -rf ${SDK_PATH}/ietf/* ${SDK_PATH}/openconfig/* ${SDK_PATH}/cisco-ios-xr/* ${SDK_PATH}/cisco-ios-xe/* ${SDK_PATH}/cisco-ios-nx/*
}

########################## EXECUTION STARTS HERE #############################
######################################
# Parse args
######################################
LANGUAGE=0
SDK_PATH=0

# extract options and their arguments into variables.
while getopts ":l:s:h" opt; do
  case ${opt} in
    l )
      LANGUAGE=$OPTARG
      ;;
    s )
      SDK_PATH=$OPTARG
      ;;
    h )
      usage
      exit 1
      ;;
    \? )
      echo "Invalid option: $OPTARG" 1>&2
      usage
      exit 1
      ;;
    : )
      echo "Invalid option: $OPTARG requires an argument" 1>&2
      usage
      exit 1
      ;;
  esac
done

if [[ ${LANGUAGE} == 0 ]] || [[ ${SDK_PATH} == 0 ]] ; then
    usage
    exit 1
fi

if [[ ${LANGUAGE} != "cpp" ]] && [[ ${LANGUAGE} != "python" ]] && [[ ${LANGUAGE} != "go" ]]; then
    echo "    Invalid language"
    usage
    exit 1
fi

GEN_API_PATH=$(pwd)/gen-api/${LANGUAGE}
SDK_STUB_PATH=$(pwd)/sdk/${LANGUAGE}

if [[ ! -d ${SDK_PATH} ]]; then
    echo "SDK path '${SDK_PATH}' is invalid! Please provide a valid path to a cloned of the YDK github SDK repository\n"
    exit 1
fi

check_gen_api_directories ${GEN_API_PATH}

while true; do
    printf "\n"
    read -p "    About to delete docs directories. Do you wish to continue? " yn
    case $yn in
        [Yy]* ) break;;
        [Nn]* ) printf "\nExiting...\n\n";exit;;
        * ) echo "Please answer yes or no.";;
    esac
done

printf "\nDeleting docs directories..\n\n"
rm -rf ${GEN_API_PATH}/*/docsgen ${GEN_API_PATH}/*/docs_expanded

echo "${LANGUAGE} being copied to ${SDK_PATH}"

if [[ ${LANGUAGE} == "cpp" ]]; then
    clear_sdk_directories
    rm -rf ${SDK_PATH}/core/ydk/*

    echo "Copying core from ${GEN_API_PATH}/ydk/ to ${SDK_PATH}"
    cp -r ${GEN_API_PATH}/ydk/* ${SDK_PATH}/core/ydk

    copy_bundle_packages

    copy_readme md

elif [[ ${LANGUAGE} == "python" ]]; then
    clear_sdk_directories
    rm -rf ${SDK_PATH}/core/ydk/*

    echo "Deleting ${GEN_API_PATH}/ydk/tests"
    rm -rf ${GEN_API_PATH}/ydk/tests

    echo "Copying core from ${GEN_API_PATH}/ydk/ to ${SDK_PATH}"
    cp -r ${GEN_API_PATH}/ydk/* ${SDK_PATH}/core/

    copy_bundle_packages

    copy_readme rst

elif [[ ${LANGUAGE} == "go" ]]; then
    echo "Clearing ${SDK_PATH} of existing files"
    rm -rf ${SDK_PATH}/ydk/*
    rm -rf ${SDK_PATH}/samples/*

    echo "Deleting ${GEN_API_PATH}/ydk/tests"
    rm -rf ${GEN_API_PATH}/ydk/tests

    copy_gen_api_to_go_sdk ydk
    echo "Copying samples from ${GEN_API_PATH}/ydk/samples/* to ${SDK_PATH}/samples/"
    cp -r ${GEN_API_PATH}/ydk/samples/* ${SDK_PATH}/samples/
    copy_gen_api_to_go_sdk ietf-bundle
    copy_gen_api_to_go_sdk openconfig-bundle
    copy_gen_api_to_go_sdk cisco_ios_xr-bundle
    copy_gen_api_to_go_sdk cisco_ios_xe-bundle
    copy_gen_api_to_go_sdk cisco_ios_nx-bundle
    copy_readme md

fi

printf "\n\n    Please check ${SDK_PATH} and perform the git commit and push\n\n"
