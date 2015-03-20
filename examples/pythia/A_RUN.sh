#!/bin/bash
# set up root
source /share/grid/app/asc_app/asc_rel/1.0/setup-script/set_asc.sh


XPWD=`pwd`
echo $XPWD

export HEPMC=/share/sl5/HepMC
export LD_LIBRARY_PATH=$HEPMC/lib:$LD_LIBRARY_PATH

make -f Makefile

./writer_pythia
