# ProMC
# Next generation input-output file format  for particle experiments

ProMC is a library to store Monte Carlo event records in a 
high-optimized event format using varints as implemented in Google Protocol buffers.

S.Chekanov (ANL)
chekanov@anl.gov



1) Compile:

  ./build.sh

2) Install libraries and include files to a 
   given directory (must have permissions!):

   ./install.sh [promc_dir] [hepmc_dir]


 (the location of HEPMC libraries are optional)


3) Setup the ProMC as:

   source  [promc_dir]/setup.sh



To unistall ProMC:
=================
./uninstall.sh



Make only proto:
./proto.sh


Clear build directory:
./clear.sh


Examples:
===============
Look at the examples in $PROMC/examples directory



S.Chekanov (ANL)
October 2014
