#!/bin/bash
# Run a java analysis code
# S.Chekanov (ANL)

if [ -z "$PROMC" ]; then
  echo "PROMC variable is not set. Need to run: source setup.sh"; exit 1 
fi 

args=$#           # Number of args passed.
if [ $args == 0  ]
then
   echo "did not specify input ProMC file!"; exit 1; 
fi
MFILE=$1

# link this library
BROWSER=${PROMC}/examples/browser/browser_promc.jar

echo "Compile.."
javac -cp  .:${BROWSER} src/promc/io/*.java  ReadProMC.java
echo "Build proto.jar .."
jar -cf proto.jar ReadProMC.class src
rm -f ReadProMC.class
echo "Run $MFILE"

# we assume that generated classes in src/ are first, so the loaded first.
# simple solution but not very robust 
java -cp  .:proto.jar:${BROWSER} ReadProMC $MFILE

