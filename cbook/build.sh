#!/bin/bash
# S.Chekanov. ANL 
# May 2014
echo "Building CBook of ProMC.."

export CURRENT_DIR=`pwd`
chmod -R 755 $CURRENT_DIR 
export DIR_CBOOK=$CURRENT_DIR
export DIR_CBOOK_PARENT=`dirname $PWD`


ARCH0=`uname -s`
if [ "$ARCH0" == "Darwin" ];
then
   echo "Operating system=$ARCH0. Apply patches..";
   mv $DIR_CBOOK/src/zipios/directory.cpp $DIR_CBOOK/src/zipios/directory.cpp_def 
   cp -f $DIR_CBOOK/inc/CBook/ZIP.h  $DIR_CBOOK/src/CBook/ZIP.h
fi


echo " --> Start compiling LibZip"
LIBZIP=`ls -1 libzip-*gz`
echo "$LIBZIP was found" 
tar -zvxf $LIBZIP
LIBZIP_DIR=${LIBZIP%.tar.gz}
echo "$LIBZIP_DIR created.." 
mv $LIBZIP_DIR "libzip"
if [ -d "$DIR_CBOOK/libzip" ]; then
   cd $DIR_CBOOK/libzip
  ./configure --enable-shared=no --enable-static=yes --enable-fast-install=no --with-pic=yes --prefix=$CURRENT_DIR/share
   make
   make install
fi


echo "Compiling the rest of CBook.."
cd $DIR_CBOOK
make

echo "Create large archive "
cd $DIR_CBOOK/share/lib/
ar -x libcbook.a
ar -x libzip.a
ar -r -c  $DIR_CBOOK_PARENT/share/lib/libcbook.a *.o
rm -rf *.o

cd $DIR_CBOOK
ranlib $DIR_CBOOK/share/lib/libcbook.a 
echo "$DIR_CBOOK_PARENT/share/lib/libcbook.a created"
echo "All done.."
