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
# Prepare virtual environment for ydk-gen,
# Requirement: device with pip and virtualenv installed
#
# ------------------------------------------------------------------


command -v pip >/dev/null 2>&1 || {
    echo "pip not found.. please install python pip before continuing !!" >&2;
    return
}

command -v virtualenv >/dev/null 2>&1 || {
    echo ""
    echo "Installing virtualenv"
    pip install virtualenv 
    echo ""
}

export YDKGEN_HOME=`pwd`
MY_ENV=mypython

# Install virtual environment
echo ""
echo "Creating/activating virtual environment"
echo ""
if [ -f "$MY_ENV/bin/activate" ]; then
    source $MY_ENV/bin/activate
else
    virtualenv -p python3 $MY_ENV
    source $MY_ENV/bin/activate
fi

# Install pip dependency packages
echo ""
echo "Installing python dependencies.."
echo ""
pip install -r requirements.txt

echo "Setup completed.. "
echo ""
echo "To exit virtualenv, run the below command from your shell/terminal:"
echo "deactivate"
echo ""
echo "To generate YDK model APIs for your profile, run the below command from your shell/terminal:"
echo "python generate.py --profile <profile-file>"
echo ""

