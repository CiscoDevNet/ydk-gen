#!/bin/sh

# Install script for YDK for device with pip and virtualenv installed

show_help() {
    echo "Options for install.sh:"
    printf "\n"
    printf "  -h Show this help\n"
    printf "  -p | --profile Specify profile file\n"
    printf "  -o | --output directory, default to ${YDKGEN_HOME}/gen-api/python\n"
    printf "  -e | --env-name Specify virtual environment name, default to mypython\n"
    printf "  --no-env Disable creating virtual environment under ${PWD}\n"
    printf "  --no-doc Disable generating documentation\n"
    printf "  --no-dep Disable install python dependency specified in ${PWD}/requirements.txt\n"
}

YDKGEN_HOME=`pwd`
NO_ENV=false
NO_DOC=false
NO_DEP=false
OUT_DIR=$YDKGEN_HOME/gen-api/python

# Parse options
while [ $# -gt 0 ]; do
    arg="$1"
    shift
    case "$arg" in
        -h | --help)
            show_help
            exit;;
        -p | --profile)
            PROFILE_FILE="$1"
            shift;;
        -o | --out-dir)
            OUT_DIR="$1"
            shift;;
        -e | --env-name)
            ENV_NAME="$1"
            shift;;
        --no-env)
            NO_ENV=true
            shift;;
        --no-doc)
            NO_DOC=true
            shift;;
        --no-dep)
            NO_DEP=true
            shift;;
        *)
            echo "${key}: Unknown option"
            exit;;
    esac
done

# Profile file is explicitly required
if [ -z "$PROFILE_FILE" ]; then
    echo "A profile file is required!"
    exit 1
fi

# Setting up virtual environment
if [ -z "$ENV_NAME" ]; then
    MY_PYTHON=mypython
else
    MY_PYTHON=$ENV_NAME
fi

if [ "$NO_ENV" = false ] || [ -z "$ENV_NAME" ]; then
    echo "Installing virtual environment under ${PWD}"
    virtualenv $MY_PYTHON
    source $MY_PYTHON/bin/activate
fi

# Install pip dependency packages
if [ "$NO_DEP" = false ] ; then
    echo "Installing python dependency"
    pip install -r requirements.txt
fi

# Generating API
if [ "$NO_DOC" = false ] ; then
    python generate.py -p -v --profile $PROFILE_FILE --output-directory $OUT_DIR
else
    python generate.py -p -v --no-doc --profile $PROFILE_FILE --output-directory $OUT_DIR
fi

# Get version number
VERSION=$(grep 'version' $PROFILE_FILE | sed 's/.*version\"://g' | sed 's/[" ,]//g')

# Install YDK-py
pip install $YDKGEN_HOME/gen-api/python/dist/ydk-$VERSION.tar.gz

# If new virtual environment is created, print instruction
if [ "$NO_ENV" = false ]; then
    printf "=================================================\n"
    printf "Activate virtualenv for YDK:\n"
    printf "  source ${MY_PYTHON}/bin/activate\n"
fi

exit 0
