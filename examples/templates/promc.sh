#!/bin/bash
#
# Configuration script, to be run before compilation
#
# S.Chekanov

CURRENT_DIR=`pwd`


source setup.sh


# destination
DIR=$CURRENT_DIR"/cpp/writer/"

cd $CURRENT_DIR/proto
for filename in *.proto
do
  INPUT=${filename}
  echo "Processing $INPUT proto file"
  protoc --cpp_out=$DIR/src/  $INPUT
  protoc --java_out=$CURRENT_DIR/java/src $INPUT
done

cd $DIR
rm -f proto
ln -s ../../proto .

echo "--> Done !"

