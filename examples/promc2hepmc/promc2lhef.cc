// Convert ProMC to LHEF files 
// It reads ProMC and tries to write  Les Houches Event Files according to the proposal by Torbjörn Sjöstrand discussed at the MC4LHC workshop at CERN 2006. 
// Author: Last Feremenga (ANL, UTA) 

#include <cstdlib>
#include <string>
#include <cassert>
#include <iostream>
#include "ProMC.pb.h"
#include "ProMCBook.h"
#include "ProMCHeader.pb.h"
#include "promc2lhef.h"

using namespace std;
using namespace ProMC;

// main program to read ProMC and convert to HEPMC
int main(int argc, char** argv) {

  // Look for a help argument
  for (int i = 1; i < argc; ++i) {
    const string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      cout << argv[0] << ": a ProMC to LHEF converter" << endl;
      cout << "Usage: " << argv[0] << "[<input ProMC file> <output LHEF file>]" << endl;
      cout << "This program converts a ProMC file to LHEF" << endl;
      exit(0);
    }
  }

  // Choose input and output files from the command line
  string infile("-"), outfile("-");
  if (argc == 3) {
    infile = argv[1];
    outfile = argv[2];
  } else if (argc != 3) {
    cerr << "Usage: " << argv[0] << "[<PROMC files input> <LHEF file out>]" << endl;
    exit(1);
  }
	
	LHAupFromProMC mylha;
  mylha.openLHEF(outfile);
	mylha.initLHEF(infile);
  mylha.eventLHEF(infile); 
	mylha.closeLHEF();

  return EXIT_SUCCESS;
}


bool LHAup::openLHEF(string fileNameIn) {

  // Open file for writing. Reset it to be empty.
  string fileName = fileNameIn;
  const char* cstring = fileName.c_str();
  osLHEF.open(cstring, ios::out | ios::trunc);  
  if (!osLHEF) {
		cout<<"Error: Could not open "<<fileNameIn<<endl;
    return false;
  }

  // Write header.
  osLHEF << "<LesHouchesEvents version=\"1.0\">\n" 
         << "<!--\n"
         << " File written by ProMC::LHAup-->" << endl;

  // Done.
  return true;

}

bool LHAup::initLHEF(string infile) {
	ProMCBook *epbook = new ProMCBook( infile.c_str(),"r");
	ProMCHeader header = epbook->getHeader();
  double xcross=header.cross_section();
  unsigned int  nev=epbook->getEvents();

  xcross=6.01E-04;

  // Write information on beams. 
  osLHEF << "<init>\n" << scientific << setprecision(8)
         << "  " << "2212"  << "  " << "2212" 
         << "  " << "8000"
				 << " "  << setw(13) << "Excl. H->all"  
           << " " << setw(13) << xcross 
           << " " << setw(6) << 0 << "\n";

  // Done.
  osLHEF << "</init>" << endl;
  epbook->close();
  return true;

}


bool LHAup::eventLHEF(string infile) {
	ProMCBook *epbook = new ProMCBook( infile.c_str(),"r");
	int  h=epbook->getVersion();
	unsigned int  nev=epbook->getEvents();

  ProMCHeader header = epbook->getHeader();
  float kEV=(float)(header.momentumunit());
  float kLe=(float)(header.lengthunit());

for (unsigned int j=0; j<nev; j++){ // loop over events
	if (epbook->next() !=0) continue;
	ProMCEvent eve = epbook->get();
	ProMCEvent_Event  *e=eve.mutable_event();
  // get truth particle information 
  ProMCEvent_Particles  *pa=eve.mutable_particles();
	int event_number=e->number();
	int signal_process_id=e->process_id();
	float event_scale=e->scale();
	float alphaQED =e->alpha_qed();
	float alphaQCD =e->alpha_qcd();
	int  id1pdf=e->id1();
	int  id2pdf=e->id2();
	float  x1pdf=e->x1();
	float  x2pdf=e->x2();
	float  pdf1=e->pdf1();
	float  pdf2=e->pdf2();
	float  Qfac=e->scale_pdf();


    // Write information on process as such. 
    osLHEF<< "<event>\n" << scientific << setprecision(6)
        	<< " " << setw(5) << pa->pdg_id_size() - 1 
          << " " << setw(5) << signal_process_id 
          << " " << setw(13) << 0 //zero is a dummy 
          << " " << setw(13) << event_scale 
          << " " << setw(13) << alphaQED 
          << " " << setw(13) << alphaQCD  << "\n";

  for (unsigned int n=0; n<pa->pdg_id_size(); n++){ // loop over particles

         int status=pa->status(n);
         LHAParticle ptNow;
          ptNow.idPart=pa->id(n); // position in the record (putting as dummy)
          ptNow.idPart=pa->pdg_id(n); // PDG id 
          ptNow.statusPart=pa->status(n);
          ptNow.pxPart=(pa->px(n)/kEV); 
          ptNow.pyPart=(pa->py(n)/kEV);
          ptNow.pzPart=(pa->pz(n)/kEV);
          ptNow.mPart=(pa->mass(n)/kEV); 
          ptNow.daughter1Part=pa->daughter1(n);
          ptNow.daughter2Part=pa->daughter2(n);
          ptNow.mother1Part=pa->mother1(n);
          ptNow.mother2Part=pa->mother2(n);
          ptNow.ePart=(pa->energy(n)/kEV);



      osLHEF << " " << setw(8) <<  ptNow.idPart
             << " " << setw(5) <<  ptNow.statusPart
             << " " << setw(5) <<  ptNow.mother1Part
             << " " << setw(5) <<  ptNow.mother2Part
             << " " << setw(5) <<  ptNow.daughter1Part
             << " " << setw(5) <<  ptNow.daughter2Part << setprecision(10)
             << " " << setw(17) << ptNow.pxPart 
             << " " << setw(17) << ptNow.pyPart 
             << " " << setw(17) << ptNow.pzPart  
             << " " << setw(17) << ptNow.ePart 
             << " " << setw(17) << ptNow.mPart  << setprecision(6);
      osLHEF << "\n";

    } // particle loop
  // Done.
  osLHEF << "</event>" << endl;
	}//event loop

  epbook->close();

  return true;
}
//--------------------------------------------------------------------------

// Write end of a Les Houches Event File and close it.
bool LHAup::closeLHEF() {

  // Write an end to the file.
  osLHEF << "</LesHouchesEvents>" << endl;
  osLHEF.close();

  // Done.
  return true;

}

