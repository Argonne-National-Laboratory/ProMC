#!/bin/bash
# Configuration script. Clean the project
# S.Chekanov. ANL 

CURRENT_DIR=`pwd`
echo "Clear the installation.."

export PROMC=$CURRENT_DIR
export LD_LIBRARY_PATH=$PROMC/share/lib:$LD_LIBRARY_PATH
export PATH=$PROMC/bin:$PATH

rm -f *~
rm -f */*~
rm -f */*/*~
rm -f  setup.sh
rm -rf share/lib
rm -rf share/bin

cd $CURRENT_DIR/libzip
echo "Clean $CURRENT_DIR/libzip"
make clean
make distclean
rm -f *.log
rm -f */*.log


cd $CURRENT_DIR/protobuf
echo "Clean $CURRENT_DIR/protobuf"
make clean
make distclean
rm -f *.log
rm -f */*.log

cd $CURRENT_DIR/protobuf/gtest
make clean
make distclean

cd $CURRENT_DIR/protobuf/python
setup.py clean


echo "Clean $CURRENT_DIR/cbook"
cd $CURRENT_DIR/cbook
make clean
rm -f */*.o
rm -f */*/*.o
rm -f version.h
rm -f qftitle.inc
if [ -d "$CURRENT_DIR/cbook/libzip" ]; then
 rm -rf $CURRENT_DIR/cbook/libzip
fi
rm -rf $CURRENT_DIR/cbook/share
# rm -f config.mk


cd $CURRENT_DIR/examples/reader_mc
make clean

cd $CURRENT_DIR/examples/reader_nlo
make clean

cd $CURRENT_DIR/examples/promc2root
make clean

cd $CURRENT_DIR/examples/root
make clean

cd $CURRENT_DIR/examples/hepmc2promc
make clean

cd $CURRENT_DIR/examples/random
make clean
rm -rf python src java proto

cd $CURRENT_DIR/examples/stdhep2promc
make clean

cd $CURRENT_DIR

# remove duplicate
rm -rf java/build
rm -rf java/dist
rm -rf java/doc/*
rm -f java/lib/promc-protobuf-tmp.jar

# some cleanups
rm -f */*~
rm -f */*/*~
rm -f src/ProMC*
rm -f src/promc/*
rm -f src/pronlo/*
rm -f src/proreco/*
rm -f inc/ProMC*
rm -f examples/cpp/out/* 
rm -f examples/browser/lib/browser_promc_tmp.jar
rm -f examples/root/out/*
rm -f *~ 
