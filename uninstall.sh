#!/bin/bash
#
# script to uninstall ProMC 
# S.Chekanov. ANL 

CURRENT_DIR=`pwd`

PROMC_DIR=$1

installdir=$CURRENT_DIR"/.installdir" 
if [ -f "$installdir" ]
then
  echo ""
else
  echo " Error: Cannot find installation";
  echo "        Did you compile it? Exit.";
  exit 1;
fi

DLINE=""
file=$installdir
while read line
do
       DLINE="$line"
done < "$file"



LEN=$(echo ${#DLINE})
if [ -z "$DLINE" ]
then
    echo "ProMC installation not found"
    exit 1
fi


echo "ProMC is installed in $DLINE" 
# check
while true; do
  echo -ne '\E[37;44m'"\033[1m Uninstall ProMC from $DLINE? (y/n) :\033[0m"
  read yn
  case $yn in
    y* | Y* ) command ; break ;;
    [nN]* )   echo "Not uninstalled!" ; exit; break ;;
    q* ) exit ;;
    * ) echo "Enter yes or no" ;;
  esac
done

rm -rf $DLINE
rm -f  $installdir 
echo "-> ProMC was uninstalled!"
