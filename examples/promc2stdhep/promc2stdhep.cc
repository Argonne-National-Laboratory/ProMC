/***************************************************************************
 *  Copyright  2013 S.Chekanov (ANL)  
 *  chekanov@anl.gov
 *  A library for HEP events storage and processing based on Google's PB   
****************************************************************************/


#include<iostream>
#include<fstream>
#include<stdlib.h>
#include "promc/ProMC.pb.h"
#include "promc/ProMCBook.h"
#include <cstdlib>
#include <string>
#include <cassert>
#include <stdio.h>

// STDHEP
#include "stdhep.h"
#include "stdlun.h"
#include "stdcnt.h"
#include "stdhep_mcfio.h"
#include "hepeup.h"
#include "heprup.h"
#include "stdver.h"

using namespace std;
using namespace promc;


int main(int argc, char **argv)
{

	// Look for a help argument
	for (int i = 1; i < argc; ++i) {
		const string arg = argv[i];
		if (arg == "--help" || arg == "-h") {
			cout << argv[0] << ": a ProMC to StdHep converter" << endl;
			cout << "Usage: " << argv[0] << "[<input ProMC file> <output StdHep file>]" << endl;
			cout << "This program converts a ProMC file to StdHep" << endl;
			exit(0);
		}
	}

	// Choose input and output files from the command line
	string infile("-"), outfile("-");
	if (argc == 3) {
		infile = argv[1];
		outfile = argv[2];
	} else if (argc != 3) {
		cerr << "Usage: " << argv[0] << "[<PROMC files input> <StdHep file out>]" << endl;
		exit(1);
	}


	ProMCBook *epbook = new ProMCBook( infile.c_str(),"r");
	cout << "\n\n Start to read this file.." << endl;
	// get the version number
	int  h=epbook->getVersion();
	cout << "Version = " << h << endl;
	// get the description of this file
	string d=epbook->getDescription();
	cout << "Description = " << d << endl;
	int  nev=epbook->getEvents();
	cout << "Events = " << nev  << endl;
	// get the header file with units, cross sections etc.
	ProMCHeader header = epbook->getHeader();
	cout << "Cross section=" << header.cross_section() << endl;
	// Ok, here is the units
	double kEV=(double)(header.momentumunit());
	cout << "Momentum units =" << kEV << endl;
	double kLe=(double)(header.lengthunit());
	cout << "Length unit=" << kLe << endl;


    cout << "\n -> Output StdHep file =" << outfile << endl;

    char  sinfile[230] = " ";
    strcpy(sinfile, outfile.c_str());
 
    char  desc[500] = "Converted ProMC file";
    //strcpy(desc, d.c_str());

     int istr=0;
    //initialize MCFio input
    int ierr = StdHepXdrWriteInit(sinfile, desc, nev, istr);
    if (ierr != 0) {
        std::cout << "ERROR: problem opening input file: "
                  << outfile.c_str() << std::endl;
        return 1;
    }

     // ilbl = 100   - start run record 
     ierr = StdHepXdrWrite(100,istr);
     if (ierr != 0) {
           printf(" unexpected initialization of file after");
           exit(0);
      }

    
	// loop over all events
	for (int nn=0; nn<nev; nn++){
		if (epbook->next() !=0) continue;
		ProMCEvent eve = epbook->get();

		// get truth information
		ProMCEvent_Particles  *pa=eve.mutable_particles();
                if (nn%100 ==0) cout << "# event= " << nn << endl;


                hepevt_.nevhep = nn;
                hepevt_.nhep = pa->pdg_id_size();

		// read ProMC record.
		for (int j=0; j<pa->pdg_id_size(); j++){
                       // not used: pa->barcode_size()
                       // not used: pa->weight_size()

                       // statusHepMC from Pythia8
                       int  ISTHEP=pa->status(j);

                       
/*
ISTHEP(IHEP):
    status code for entry IHEP, with the following meanings:
    = 0 :
        null entry. 
    = 1 :
        an existing entry, which has not decayed or fragmented. This is the main class of entries, which represents the `final state' given by the generator. 
    = 2 :
        an entry which has decayed or fragmented and is therefore not appearing in the final state, but is retained for event history information. 
    = 3 :
        a documentation line, defined separately from the event history. This could include the two incoming reacting particles, etc. 
    = 4 - 10 :
        undefined, but reserved for future standards. 
    = 11 - 200 :
        at the disposal of each model builder for constructs specific to his program, but equivalent to a null line in the context of any other program. 
    = 201 - :
        at the disposal of users, in particular for event tracking in the detector. 
*/



                       int  IDHEP = pa->pdg_id(j);
                       //hepevt_.ihep =j;
                       hepevt_.isthep[j] = ISTHEP;
                       hepevt_.idhep[j]=IDHEP;


                       int JMOHEP1 = pa->mother1(j) > 0 ? pa->mother1(j)+1 : 0;
                       int JMOHEP2 = pa->mother2(j) > 0 ? pa->mother2(j)+1 : 0;
                       hepevt_.jmohep[j][0]=JMOHEP1;
                       hepevt_.jmohep[j][1]=JMOHEP2;
 
                        int JDAHEP1 = pa->daughter1(j) > 0 ? pa->daughter1(j)+1 : 0;
                        int JDAHEP2 = pa->daughter2(j) > 0 ? pa->daughter2(j)+1 : 0; 
                        hepevt_.jdahep[j][0] = JDAHEP1;
                        hepevt_.jdahep[j][1] = JDAHEP2;

                        // now start the info in double, so switch the output to that format
                        hepevt_.phep[j][0] = pa->px(j)/kEV;
                        hepevt_.phep[j][1] = pa->py(j)/kEV;
                        hepevt_.phep[j][2] = pa->pz(j)/kEV;
                        hepevt_.phep[j][3] = pa->energy(j)/kEV;
                        hepevt_.phep[j][4] = pa->mass(j)/kEV;
                        // next event
                        hepevt_.vhep[j][0] =  pa->x(j)/kLe;
                        hepevt_.vhep[j][1] =  pa->y(j)/kLe;
                        hepevt_.vhep[j][2] =  pa->z(j)/kLe;
                        hepevt_.vhep[j][3] =  pa->t(j)/kLe;


		}


                 // ilbl = 1   - standard HEPEVT common block
                 int ierr = StdHepXdrWrite(1,istr);
                 if (ierr != 0) {
                    printf(" unexpected end of file after %d events\n",nn);
                    exit(0);
                 }





	} // end  event



     // ilbl = 200   - end run record 
     ierr = StdHepXdrWrite(200,istr);
     if (ierr != 0) {
           printf(" unexpected end of file");
           exit(0);
      }


       StdHepXdrEnd(istr);
       epbook->close(); // close


	cout << "Output StdHep file: " << outfile << endl;

	return 0;
}
