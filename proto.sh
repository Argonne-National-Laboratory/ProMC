#!/bin/bash
#
# Configuration script, to be run before compilation
#
# S.Chekanov

CURRENT_DIR=`pwd`

# specify compilers here (do not change this part!) 
source $CURRENT_DIR/cbook/config.sh

protocxx=$CURRENT_DIR/share/bin/protoc
chmod 755 $protocxx

echo "Generating proto files.."
export PROTOBUF=$CURRENT_DIR/share
export LD_LIBRARY_PATH=$PROTOBUF/lib:$LD_LIBRARY_PATH
export PATH=$PROTOBUF/bin:$PATH
export PKG_CONFIG_PATH=$PROTOBUF/lib/pkgconfig:$PKG_CONFIG_PATH


echo "######## ProMC #######"
PRFILE="ProMC"
echo "Process $PRFILE file for MC data"
rm -f $PRFILE.proto
ln -s proto/promc/$PRFILE.proto .
$protocxx --cpp_out=src          $PRFILE.proto # for backward compatibility 
$protocxx --cpp_out=src/promc    $PRFILE.proto # for release 1.5 
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --python_out=src       $PRFILE.proto
$protocxx --python_out=src/promc $PRFILE.proto # for release 1.5 
cp -f src/*.h inc/
rm -f $PRFILE.proto


echo "ProNLO: $PRFILE file for NLO data"
ln -s proto/pronlo/$PRFILE.proto .
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --cpp_out=src/pronlo   $PRFILE.proto # for release 1.5
$protocxx --python_out=src/pronlo $PRFILE.proto # for release 1.5
rm -f $PRFILE.proto

echo "ProNLO: $PRFILE file for EIC data"
ln -s proto/proeic/$PRFILE.proto .
$protocxx --java_out=java/src     $PRFILE.proto
$protocxx --cpp_out=src/proeic    $PRFILE.proto  # for release 1.6 
$protocxx --python_out=src/proeic $PRFILE.proto # for release 1.6 
rm -f $PRFILE.proto

echo "ProReco: $PRFILE file for Reconstructed data"
ln -s proto/proreco/$PRFILE.proto .
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --cpp_out=src/proreco  $PRFILE.proto # for release 1.5
$protocxx --python_out=src/proreco $PRFILE.proto # for release 1.5
rm -f $PRFILE.proto


echo "######## ProMCDescription #######"
PRFILE="ProMCDescription"
echo "Process $PRFILE proto file for MC data"
ln -s proto/promc/$PRFILE.proto .
$protocxx --cpp_out=src         $PRFILE.proto # for backward compatibility 
$protocxx --cpp_out=src/promc   $PRFILE.proto # for release 1.5
$protocxx --java_out=java/src   $PRFILE.proto
$protocxx --python_out=src      $PRFILE.proto
$protocxx --python_out=src/promc  $PRFILE.proto
cp -f src/*.h inc/
rm -f $PRFILE.proto

echo "ProNLO: $PRFILE for NLO data"
ln -s proto/pronlo/$PRFILE.proto .
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --cpp_out=src/pronlo   $PRFILE.proto # for release 1.5
$protocxx --python_out=src/pronlo $PRFILE.proto # for release 1.5
rm -f $PRFILE.proto

echo "ProEIC: $PRFILE for EIC data"
ln -s proto/proeic/$PRFILE.proto .
$protocxx --java_out=java/src     $PRFILE.proto
$protocxx --cpp_out=src/proeic    $PRFILE.proto  # for release 1.6
$protocxx --python_out=src/proeic $PRFILE.proto # for release 1.6
rm -f $PRFILE.proto


echo "ProReco: $PRFILE for Reco data"
ln -s proto/proreco/$PRFILE.proto .
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --cpp_out=src/proreco  $PRFILE.proto # for release 1.5
$protocxx --python_out=src/proreco $PRFILE.proto # for release 1.5
rm -f $PRFILE.proto



echo "######  ProMCHeader ######"
PRFILE="ProMCHeader"
echo "Process $PRFILE proto file for MC data"
ln -s proto/promc/$PRFILE.proto .
$protocxx --cpp_out=src          $PRFILE.proto # for backward compatibility 
$protocxx --cpp_out=src/promc    $PRFILE.proto # for release 1.5
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --python_out=src       $PRFILE.proto
$protocxx --python_out=src/promc  $PRFILE.proto # for release 1.5
cp -f src/*.h inc/
rm -f $PRFILE.proto

echo "ProNLO: $PRFILE for NLO data"
ln -s proto/pronlo/$PRFILE.proto .
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --cpp_out=src/pronlo   $PRFILE.proto # for release 1.5
$protocxx --python_out=src/pronlo $PRFILE.proto
rm -f $PRFILE.proto


echo "ProEIC: $PRFILE for EIC data"
ln -s proto/proeic/$PRFILE.proto .
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --cpp_out=src/proeic   $PRFILE.proto # for release 1.5
$protocxx --python_out=src/proeic $PRFILE.proto
rm -f $PRFILE.proto


echo "ProNLO: $PRFILE for Reco data"
ln -s proto/proreco/$PRFILE.proto .
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --cpp_out=src/proreco  $PRFILE.proto # for release 1.5
$protocxx --python_out=src/proreco $PRFILE.proto
rm -f $PRFILE.proto


echo "##### ProMCStat ######" 
PRFILE="ProMCStat"
echo "Process $PRFILE proto file"
ln -s proto/promc/$PRFILE.proto .
$protocxx --cpp_out=src          $PRFILE.proto # for backward compatibility 
$protocxx --cpp_out=src/promc    $PRFILE.proto # for release 1.5
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --python_out=src       $PRFILE.proto
$protocxx --python_out=src/promc $PRFILE.proto
cp -f src/*.h inc/
rm -f $PRFILE.proto

echo "ProNLO: $PRFILE for NLO data"
ln -s proto/pronlo/$PRFILE.proto .
$protocxx --cpp_out=src/pronlo   $PRFILE.proto # for release 1.5
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --python_out=src/pronlo $PRFILE.proto
rm -f $PRFILE.proto

echo "ProEIC: $PRFILE for EIC data"
ln -s proto/proeic/$PRFILE.proto .
$protocxx --cpp_out=src/proeic    $PRFILE.proto # for release 1.6 
$protocxx --java_out=java/src     $PRFILE.proto
$protocxx --python_out=src/pronlo $PRFILE.proto
rm -f $PRFILE.proto

echo "ProNLO: $PRFILE for Reco data"
ln -s proto/proreco/$PRFILE.proto .
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --cpp_out=src/proreco  $PRFILE.proto # for release 1.5
$protocxx --python_out=src/proreco $PRFILE.proto
rm -f $PRFILE.proto


echo "#### AUX functions ####"
PRFILE="HEvent"
echo "Process $PRFILE proto file"
ln -s proto/cbook/$PRFILE.proto .
$protocxx --cpp_out=src        $PRFILE.proto
$protocxx --java_out=java/src  $PRFILE.proto
$protocxx --python_out=src     $PRFILE.proto
cp -f src/*.h inc/
cp src/$PRFILE.pb.cc   cbook/src/HRecord/$PRFILE.pb.cc
cp src/$PRFILE.pb.h    cbook/inc/CBook/
rm -f $PRFILE.proto

PRFILE="HRecord"
echo "Process $PRFILE proto file"
ln -s proto/cbook/$PRFILE.proto .
$protocxx --cpp_out=src          $PRFILE.proto
$protocxx --java_out=java/src    $PRFILE.proto
$protocxx --python_out=src       $PRFILE.proto
cp -f src/*.h inc/
cp src/$PRFILE.pb.cc  cbook/src/HRecord/$PRFILE.pb.cc
cp src/$PRFILE.pb.h   cbook/inc/CBook/
rm -f $PRFILE.proto

echo "### proto/ProMCBook.* and compile libraries for each case"
cp -f proto/ProMCBook.* src/
cp -f proto/ProMCBook.* src/promc/
cp -f proto/ProMCBook.* src/proreco/
cp -f proto/ProMCBook.* src/pronlo/
cp -f preic/ProMCBook.* src/proeic/

PROMC_LIB="-L./share/lib -lprotoc -lprotobuf -lprotobuf-lite -lcbook -lz"
INCLUD1=-I./share/include
INCLUD2=-I./cbook/inc
INCLUD3=-I./cbook/src
INCLUD4=-I./cbook/share/lib/libzip/include

echo "--> Build ProMC library.."
AR_LIB=share/lib/libpromc.a
XDIR=./src/promc
for f in $XDIR/*.cc
do
  echo "Compiling $f.."
  obj=${f%.cc}.o
  $CXX $CPP_FLAGS -c $f -o  $obj $INCLUD1 $INCLUD2 $INCLUD3 $INCLUD4
  $AR rcs $AR_LIB $obj
done
if [ ! -e "$AR_LIB" ]; then
    echo "Error! File $AR_LIB does not exist!!!"
else
    chmod 755 $AR_LIB
    echo "$AR_LIB was build"
fi


echo "--> Build ProNLO library.."
AR_LIB=share/lib/libpronlo.a
XDIR=./src/pronlo
for f in $XDIR/*.cc
do
  echo "Compiling $f.."
  obj=${f%.cc}.o
  $CXX $CPP_FLAGS -c $f -o  $obj $INCLUD1 $INCLUD2 $INCLUD3 $INCLUD4
  $AR rcs $AR_LIB $obj
done
if [ ! -e "$AR_LIB" ]; then
    echo "Error! File $AR_LIB does not exist!!!"
else 
    chmod 755 $AR_LIB
    echo "$AR_LIB was build"
fi 

echo "--> Build ProEIC library.."
AR_LIB=share/lib/libproeic.a
XDIR=./src/proeic
for f in $XDIR/*.cc
do
  echo "Compiling $f.."
  obj=${f%.cc}.o
  $CXX $CPP_FLAGS -c $f -o  $obj $INCLUD1 $INCLUD2 $INCLUD3 $INCLUD4
  $AR rcs $AR_LIB $obj
done
if [ ! -e "$AR_LIB" ]; then
    echo "Error! File $AR_LIB does not exist!!!"
else
    chmod 755 $AR_LIB
    echo "$AR_LIB was build"
fi


echo "--> Build ProReco library.."
AR_LIB=share/lib/libproreco.a
XDIR=./src/proreco
for f in $XDIR/*.cc
do
  echo "Compiling $f.."
  obj=${f%.cc}.o
  $CXX $CPP_FLAGS -c $f -o  $obj $INCLUD1 $INCLUD2 $INCLUD3 $INCLUD4
  $AR rcs $AR_LIB $obj
done
if [ ! -e "$AR_LIB" ]; then
    echo "Error! File $AR_LIB does not exist!!!"
else
    chmod 755 $AR_LIB
    echo "$AR_LIB was build"
fi


 
echo "--> Done !"

