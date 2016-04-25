#!/bin/sh

# Prepare virtual environment for ydk-gen,
# Requirement: device with pip and virtualenv installed
export YDKGEN_HOME=`pwd`
MY_ENV=mypython
OUT_DIR=$YDKGEN_HOME/gen-api/python
PROFILE_FILE=$YDKGEN_HOME/profiles/ydk/ydk_0_4_0.json

# Install virtual environment
echo "Installing virtual environment under ${PWD}"
virtualenv $MY_ENV
source $MY_ENV/bin/activate

# Install pip dependency packages
echo "Installing python dependency"
pip install -r requirements.txt

echo "To exit current environment:"
echo "  $ deactivate"
