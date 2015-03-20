#!/bin/bash
# ProMC setup script. Do not modify, created by install.sh
# Read more in: http://atlaswww.hep.anl.gov/asc/promc/

CFILE="./lib/libcbook.a"
export PROMC=`pwd`
if [ ! -f $CFILE ];
then
    export PROMC="$( cd "$( echo "${BASH_SOURCE[0]%/*}" )" && pwd )"
fi
echo  "PROMC was set to $PROMC"
export LD_LIBRARY_PATH=$PROMC/lib:$LD_LIBRARY_PATH
export PATH=$PROMC/bin:$PATH
export PKG_CONFIG_PATH=${PROMC}/lib/pkgconfig:${PKG_CONFIG_PATH}
export PYTHONPATH=${PROMC}/python/lib/python2.6/site-packages:$PYTHONPATH
