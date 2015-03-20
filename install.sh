#!/bin/bash
#
# Configuration script to install ProMC 
# S.Chekanov. ANL 

CURRENT_DIR=`pwd`

args=$#           # Number of args passed.
if [ $args == 0  ]
then
   echo "->Error: You did not specify the installation directory!"
   echo "         Run it as install.sh <installation directory>"
   echo "         Most common installation: install.sh /usr/local"
   echo "         Now we exit!"
   exit 1;
fi
PROMC_DIR=$1


## try HEPMC
HEPMC=""
HEPMC_ENV=""
if [ $args == 2  ]
then
   export HEPMC=$2
   HEPMC_ENV="export HEPMC=${HEPMC}; LD_LIBRARY_PATH=\$HEPMC/lib:\$LD_LIBRARY_PATH"; 
   if [ ! -d "$HEPMC/include" ]; then
   echo "->HEPMC: You have given the HEPMC location, but HEPMC cannot be  found!"
   exit 1;
   fi
   echo "->HEPMC: HEPMC intallation directory is $HEPMC"
fi


libcbook=$CURRENT_DIR"/share/lib/libcbook.a" 
if [ ! -f "$libcbook" ]
then
  echo " Error: Cannot find ProMC library $libcbook";
  echo "        Did you compile it? Exit.";
  exit 1;
fi

libcbook=$CURRENT_DIR"/share/lib/libpromc.a"
if [ ! -f "$libcbook" ]
then
  echo " Error: Cannot find $libcbook";
  echo "        Did you compile it? Exit.";
  exit 1;
fi

libcbook=$CURRENT_DIR"/share/lib/libpronlo.a"
if [ ! -f "$libcbook" ]
then
  echo " Error: Cannot find $libcbook";
  echo "        Did you compile it? Exit.";
  exit 1;
fi

libcbook=$CURRENT_DIR"/share/lib/libproreco.a"
if [ ! -f "$libcbook" ]
then
  echo " Error: Cannot find $libcbook";
  echo "        Did you compile it? Exit.";
  exit 1;
fi


RSYNC="rsync"
fcheck=`type -p $RSYNC`
if test -x "$fcheck";
then
    echo " " 
else
    echo "Error: $RSYNC does not exists. Please install it first"
    exit 0;
fi

echo "Ok, the installation was successful"

# check
while true; do
  echo -ne "Install ProMC in $PROMC_DIR? (y/n)"
  read yn
  case $yn in
    y* | Y* ) command ; break ;;
    [nN]* )   echo "Not installed!" ; exit; break ;;
    q* ) exit ;;
    * ) echo "Enter yes or no" ;;
  esac
done

export PROJ=$PROMC_DIR"/promc"
# check if exits
if [ -d "$PROJ" ]
then

 # check
while true; do
  echo -ne "The directory \"$PROJ\" exists"
  echo ""
  echo -ne  "Do you want a fresh install? (y/n)"
  read yn
  case $yn in
    y* | Y* ) command ; break ;;
    [nN]* )   echo "Not installed!" ; exit; break ;;
    q* ) exit ;;
    * ) echo "Enter yes or no" ;;
  esac
done

 rm -rf $PROJ 
 mkdir -p  $PROJ/include
 mkdir -p  $PROJ/lib
 mkdir -p  $PROJ/etc
 mkdir -p  $PROJ/proto
 mkdir -p  $PROJ/examples
 mkdir -p  $PROJ/python/lib
 mkdir -p  $PROJ/src
else 
 mkdir -p $PROJ/include
 mkdir -p $PROJ/lib
 mkdir -p $PROJ/etc
 mkdir -p $PROJ/proto
 mkdir -p $PROJ/examples
 mkdir -p $PROJ/python/lib
 mkdir -p $PROJ/src
fi


python_dis=`python -c "import sys,os; ss=os.sep.join([sys.prefix, 'lib', 'python' + sys.version[:3], 'site-packages']); ss=ss.split('/'); print ss[len(ss)-2]+'/'+ss[len(ss)-1]"`
# echo "Python install directory=$python_dis"
mkdir -p  ${PROJ}/python/lib/$python_dis


cd ${CURRENT_DIR}
rsync -ra --exclude ".svn/" share/*     ${PROJ}/
rsync -ra --exclude ".svn/" cbook/inc/* ${PROJ}/include/
rsync -ra --exclude ".svn/" cbook/share/include/zip.h           ${PROJ}/include/
rsync -ra --exclude ".svn/" cbook/share/lib/libzip/include/*    ${PROJ}/include/
rsync -ra --exclude ".svn/" examples/*  ${PROJ}/examples/
rsync -ra --exclude ".svn/" java/*      ${PROJ}/java/
rsync -ra --exclude ".svn/" cbook/config.mk ${PROJ}/etc/
rsync -ra --exclude ".svn/" proto/*         ${PROJ}/proto/
rsync -ra --exclude ".svn/" bin/*           ${PROJ}/bin/
rsync -ra --exclude ".svn/" --exclude="*.o"  src/*      ${PROJ}/src/
rsync -ra --exclude ".svn/" proto/ProMCBook.*    ${PROJ}/src/
chmod -R 755 $PROJ/bin
chmod -R 755 $PROJ/src
chmod -R 755 $PROJ/lib

export PROMC=${PROJ}
echo $PROMC > $CURRENT_DIR/.installdir
echo "-> Installation directory PROMC=${PROMC}"
cat ./scripts/setup.sh > $PROMC/setup.sh
echo ${HEPMC_ENV} >> $PROMC/setup.sh
chmod 755 $PROMC/setup.sh

source $PROMC/setup.sh
if [ $args == 2  ]
then
   echo "Compile HEPMC to ProMC converter"
   cd $CURRENT_DIR/examples/hepmc2promc
   make 
   cp $CURRENT_DIR/examples/hepmc2promc/hepmc2promc $PROJ/bin/
   echo "-> HEPMC to PROMC is installed!"
   echo "Compile ProMC to HEPMC converter"
   cd $CURRENT_DIR/examples/promc2hepmc
   make
   cp $CURRENT_DIR/examples/promc2hepmc/promc2hepmc $PROJ/bin/
   echo "-> ProMC to HepMC is installed!"
fi

fcheck=`type -p python`
PYTHON_OK=`python -c 'import sys; print (sys.version_info >= (2, 5) and "1" or "0")'`
if [ "$PYTHON_OK" = '0' ]; then
    echo "-> PYTHON version is too old. Should be >2.5. PYTHON is not used"
    #exit 0;
else
   echo "-> Installing Python library to ${PROMC}/python/lib/$python_dis"
   cd ${CURRENT_DIR}/protobuf/python
   python setup.py install --prefix=${PROJ}/python > /tmp/promc_python.log
fi


echo "-> Setup your enviroment as: source ${PROMC}/setup.sh"
echo "-> ProMC is installed!"
