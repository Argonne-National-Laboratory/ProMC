#!/bin/bash
#
# Configuration script to build ProMC
# S.Chekanov. ANL 
# May 2013
echo "Building ProMC.."
CURRENT_DIR=`pwd`

# specify compilers here (do not change this part!) 
CXX="g++"
CC="gcc"
AR="ar"
XMAKE="make"

ARCH0=`uname -s`
if [ "$ARCH0" != "Linux" ] && [ "$ARCH0" != "Darwin" ] && [ "$ARCH0" != "FreeBSD" ]; 
then
  echo " Error: operating system=$ARCH0, this is not Linux/FreeBSD/Darwin! Stop";
  exit 1;
fi

if [ "$ARCH0" == "Darwin"  ]; 
then
  echo " Warning: Support for $ARCH0 operating system is experimental..";
fi

echo " * ok, platform is $ARCH0. Start compilation.."

chmod -R 755 $CURRENT_DIR

fcheck=`type -p $CC`
if test -x "$fcheck";
then
    echo " * ok, $fcheck -> $CC found" 
else
    echo "Error: $CC does not exist"
    exit 0;
fi


fcheck=`type -p $CXX`
if test -x "$fcheck";
then
    echo " * ok, $fcheck -> $CXX found"
else
    echo "Error: $CXX does not exist"
    exit 0;
fi


fcheck=`type -p protoc`
if test -x "$fcheck";
then
    PRBUF=`protoc --version`
    echo "Warning: Protocol Buffers $PRBUF installed in $fcheck"
    echo "         ProMC uses an internal Protocol Buffers. Continue."
fi


ZLIB="/usr/include/zlib.h"
if [ -f $FILE ]
then
    echo " * ok, $ZLIB -> is found"
else
   echo "Error: zlib-dev does not exist"
   echo "Try: yum install zlib-devel (or) sudo apt-get install zlib1g-dev (ubuntu)" 
   exit 0;
fi


# check make
fcheck=`type -p $XMAKE`
if test -x "$fcheck";
then
    echo " * ok, $fcheck -> GNU $XMAKE found"
else
    echo "Error: $XMAKE does not exist"
    exit 0;
fi


fcheck=`type -p $AR`
if test -x "$fcheck";
then
    echo " * ok, $fcheck -> $AR  found"
else
    echo "Error: $AR does not exist"
    exit 0;
fi


fcheck=`type -p ranlib`
if test -x "$fcheck";
then
    echo " * ok, $fcheck -> ranlib found"
else
    echo "Error: ranlib does not exist"
    exit 0;
fi


fcheck=`type -p python`
PYTHON_OK=`python -c 'import sys; print (sys.version_info >= (2, 5) and "1" or "0")'`
if [ "$PYTHON_OK" = '0' ]; then
    echo "  Note: Python version is too old. Should be >2.4. PYTHON is not used"
    #exit 0;
else
    echo " * ok, $fcheck -> PYTHON is found"
fi

ARCH=`arch`
echo "-->Success. Library for $ARCH"

#
# User-changeable part, experts  -----------------------------
CPP_FLAGS=" -ansi -fPIC -Wall -c"
CFLAGS="${CFLAGS_OPT}"
CXXFLAGS="${CXXFLAGS_OPT}"


echo "Creating $CURRENT_DIR/cbook/config.mk"
cd $CURRENT_DIR/cbook
rm -f config.mk
echo SHELL=/bin/sh > config.mk
echo DIR_SHARE=$CURRENT_DIR/share >> config.mk
echo DIR_CBOOK=$CURRENT_DIR/cbook >> config.mk
echo DIR_CURRENT=$CURRENT_DIR >> config.mk
echo AR=${AR} >> config.mk
echo ARCH=${ARCH} >> config.mk
echo CXX=${CXX} >> config.mk
echo CPP_COMP=${CXX} >> config.mk
echo CPP_FLAGS=${CPP_FLAGS} >> config.mk
echo CC=${CC} >> config.mk


echo "Creating $CURRENT_DIR/cbook/config.sh"
cd $CURRENT_DIR/cbook
rm -f config.sh
echo "#!/bin/bash" > config.sh
echo "DIR_SHARE=$CURRENT_DIR/share" >> config.sh
echo "DIR_CBOOK=$CURRENT_DIR/cbook" >> config.sh
echo "DIR_CURRENT=$CURRENT_DIR" >> config.sh
echo "AR=${AR}" >> config.sh
echo "ARCH=${ARCH}" >> config.sh
echo "CXX=${CXX}" >> config.sh
echo "CPP_COMP=${CXX}" >> config.sh
echo "CPP_FLAGS=\"${CPP_FLAGS}\"" >> config.sh
echo "CC=${CC}" >> config.sh




echo ""
echo " --> Start compiling ProtoBuf"
cd $CURRENT_DIR/protobuf/
./configure --enable-shared=no --enable-static=yes --prefix=$CURRENT_DIR/share
make 
make install


cd $CURRENT_DIR
echo ""
echo " * ok, setting up env variables for proto library"
export PROTOBUF=$CURRENT_DIR/share
export LD_LIBRARY_PATH=$PROTOBUF/lib:$LD_LIBRARY_PATH
export PATH=$PROTOBUF/bin:$PATH
export PKG_CONFIG_PATH=$PROTOBUF/lib/pkgconfig:$PKG_CONFIG_PATH

PRBUF="-1"
fcheck=`type -p protoc`
if test -x "$fcheck";
then
    PRBUF=`protoc --version`
else
    echo "Error: protocol buffers does not exist"
    echo "Install it from http://code.google.com/p/protobuf/"
    exit 0;
fi

pkgcheck="`pkg-config --cflags --libs protobuf`"
if [ -n "$pkgcheck" ]; then
    echo " * ok,  protobuf is found. Version=$PRBUF" 
else
    echo "pkg-config --cflags --libs protobuf is not found"
    echo "Consider: export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig"
    exit 0;
fi



echo " --> Start installing ProtoBuff Python modules"
cd $CURRENT_DIR/protobuf/python
# export PROTOCOL_BUFFERS_PYTHON_IMPLEMENTATION=cpp
python ./setup.py build
# python ./setup.py install --prefix=$CURRENT_DIR/protobuf/python

echo ""
echo " --> Building CBook library.."
cd $CURRENT_DIR/cbook/
./build.sh
cd $CURRENT_DIR


echo " --> Generating proto files.."
cd $CURRENT_DIR
./proto.sh
cd $CURRENT_DIR


fcheck=`type -p ant`
if test -x "$fcheck";
then
    echo " * ok, $fcheck -> ant is found"
    cd ./java
    ant
    ant javadoc
    cd $CURRENT_DIR
else
    echo "ant is not found. Skip this step. Use default"
fi



echo "All done.."
