# ProMC
Next generation input-output file format  for particle experiments


ProMC is a library for file input and output of Monte Carlo event records or any HEP structural data. The main features are:

- Content-dependent compression. Event records are encoded using Google protocol buffers with variable number of bytes ("varints"). 
  This leads to 30% (and more) smaller file sizes compared to any known compression based on a fixed-length representation of numeric information.
- Multiplatform. Data records can be read and can be written natively in C++, Java and Python.
- Self-describing data format. Analysis source codes can be generated from ProMC files with unknown data layouts.
- Forwards-compatible and backwards-compatible binary wire format.
- Random access. Events can be read starting at any index.
- Fast. No CPU overhead on decompression of events
- Simplicity. No external dependence. The library is small and self-contained. The library has been deployed on BlueGene/Q.


<h2>Description</h2>
The description of the file format is given in these two articles:

- S.Chekanov, E.May, K.Strand, P. Van Gemmeren, 
  ProMC: Input-output data format for HEP applications using varint encoding, 
  ANL-HEP-PR-13-41, arXiv:1311.1229, Computer Physics Communications 185 (2014), pp. 2629-2635

- S.Chekanov, “Next generation input-output data format for HEP using Google protocol buffers”, 
  ANL-HEP-CP-13-32, Snowmass 2013 Proceedings. arxiv.org:1306.6675


Read the manual: https://atlaswww.hep.anl.gov/asc/wikidoc/doku.php?id=asc:promc


<h2>How to install</h2>

1) Compile:
  ./build.sh

2) Install libraries and include files to a 
   given directory (must have permissions!):
 
  ./install.sh [promc_dir] [hepmc_dir]

 (the location of HEPMC libraries are optional)


3) Setup the ProMC as:

   source  [promc_dir]/setup.sh


<h2>Uninstall</h2>

./uninstall.sh

Clear the build directory:

./clear.sh


Examples:
========
Look at the examples in $PROMC/examples directory


S.Chekanov (ANL), E.May (ANL), P. Van Gemmeren (ANL) 

October 2014
