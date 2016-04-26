#!/bin/bash
#
# Scritp for installing precompiled fxs for ydktest
#

function test {
    "$@"
    local status=$?
    if [ $status -ne 0 ]; then
        exit $status
    fi
    return $status
}

ROOT=/root
CONFD=/root/confd
CONFD_TARGET_DIR=$CONFD/etc/confd
YDKTEST_FXS=$CONFD/src/confd/yang/ydktest/fxs/ydktest/
BGP_DEVIATION_FXS=$CONFD/src/confd/yang/ydktest/fxs/bgp_deviation/
YDKTEST_DEVIATION_FXS=$CONFD/src/confd/yang/ydktest/fxs/ydktest_deviation/
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

# init confd for ydktest
cp $YDKTEST_FXS/* $CONFD_TARGET_DIR
source $CONFD/confdrc
cd $CONFD_TARGET_DIR
confd -c confd.conf

# clone repo
cd $ROOT
printf "Cloning from: %s, branch: %s\n" "$REPO" "$BRANCH"
git clone -b $BRANCH $REPO 
cd ydk-gen
YDK_ROOT=`pwd`

# pygen test
cd $YDK_ROOT
source install.sh
export PYTHONPATH=.:$PYTHONPATH
python test/pygen_tests.py

# generate ydktest package based on proile
python generate.py --profile profiles/test/ydktest.json --python --no-doc --verbose
deactivate

# sanity tests
virtualenv myenv
source myenv/bin/activate
pip install gen-api/python/dist/ydk-0.4.0.tar.gz
source gen-api/python/env.sh
cd gen-api/python
test python tests/test_sanity_codec.py
test python tests/test_sanity_types.py
test python tests/test_sanity_filters.py
test python tests/test_sanity_levels.py
test python tests/test_sanity_filter_read.py
test python tests/test_sanity_netconf.py
test python tests/test_sanity_rpc.py
cd ydk/tests
test python import_tests.py

# deviation tests
# modify confd instance
cp $YDKTEST_FXS/* $CONFD_TARGET_DIR
source $CONFD/confdrc
confd --stop

cp $BGP_DEVIATION_FXS/* $CONFD_TARGET_DIR
cp $YDKTEST_DEVIATION_FXS/* $CONFD_TARGET_DIR
cd $CONFD_TARGET_DIR
confd -c confd.conf

# sanity deviation
cd $YDK_ROOT
source gen-api/python/env.sh
cd gen-api/python
export PYTHONPATH=.:$PYTHONPATH
test python tests/test_sanity_deviation.py

# bgp deviation
deactivate
cd $YDK_ROOT
source mypython/bin/activate
python generate.py -p --no-doc --profile profiles/test/deviation/deviation.json
deactivate
source myenv/bin/activate
pip install gen-api/python/dist/ydk-0.4.0.tar.gz
source gen-api/python/env.sh
cd gen-api/python
test python tests/test_sanity_deviation_bgp.py

exit