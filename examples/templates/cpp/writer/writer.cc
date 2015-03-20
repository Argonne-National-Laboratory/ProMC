/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
**  A library for HEP events storage and processing based on Google's ProtocolBuffers 
**
** Author: S.Chekanov (ANL). chekanov@anl.gov
** Copyright  2013 
** -------------------------------------------------------------------------*/


#include<iostream>
#include<fstream>
#include<stdlib.h>
#include"ProMCBook.h"
#include"ProMCHeader.pb.h"
#include"ProMC.pb.h"
#include"ProMCStat.pb.h"


using namespace std;
using namespace promc; 


int main(int argc, char **argv)
{


  for (int i = 1; i < argc; ++i) {
    const string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      cout << argv[0] << ": writer" << endl;
      cout << "Usage: " << argv[0] << "[<outfile>]" << endl;
      exit(0);
    }
  }

  // Choose input and output files from the command line
  string infile("-");
  if (argc == 2) {
    infile = argv[1];
  } else if (argc == 1 || argc > 3) {
    cerr << "Usage: " << argv[0] << " [<outfile>]" << endl;
    exit(1);
  }


  ProMCBook*  epbook = new ProMCBook(infile.c_str(),"w");


  // current test
  int Ntot=1000;               // 1000 events 

  // use 0.01*MeV units
  const double kEV=1000*100;
 // this is length unit. multiply CM by 1000
 const double uL=1000.;

  epbook->setDescription(Ntot, "ProMC file to keep MC data");
  ProMCHeader header;
  // set cross section
  header.set_cross_section(10000);

  // set PDF for beams 
  header.set_id1(200);
  header.set_id2(-200); 

  // set units in 0.01*MeV for pX,pY,pZ 
  // in reader, we will read this back
  header.set_momentumunit((int)kEV);

  // set length unit. In Pythia, we multiply CM by 1000 
  header.set_lengthunit((int)uL);

  // let's store a map with most common masses:
  ProMCHeader_ParticleData* pi= header.add_particledata();
  pi->set_id(211); // pi+ 
  pi->set_mass(0.13957018);
  pi->set_name("pi+");

  // write the header.
  epbook->setHeader(header); // write header 
 
  // output TXT file for comparions
  ofstream myfile;
  myfile.open ("out/output.txt");

cout << "Start writing data .." << endl;
for (int i=0; i<Ntot; i++) { 


               ProMCEvent promc;


              // fill event information
              ProMCEvent_Event  *eve= promc.mutable_event();
              eve->set_number(i);
              eve->set_process_id(10); // 10 is just a dummy number 


               // fill truth information 
               ProMCEvent_Particles  *pa= promc.mutable_particles();


               if (i%100 == 0) cout << "Event=" << i << endl;


               epbook->write(promc); // write event

}




  // at the end of the record, write accumulated statistics
  cout << "Write accumulated statistics to the ProMC file " << endl;
  ProMCStat stat;
  stat.set_cross_section_accumulated(10000);
  stat.set_cross_section_error_accumulated(10);
  stat.set_luminosity_accumulated(1000);
  epbook->setStatistics(stat); // write header 

   // close the file
   myfile.close();  
   cout << "\nFile " << infile << " is done for all events" << endl;
   epbook->close(); // close


    return 0;
}
