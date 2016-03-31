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

using namespace std;
using namespace promc;


int main(int argc, char **argv)
{

	// Look for a help argument
	for (int i = 1; i < argc; ++i) {
		const string arg = argv[i];
		if (arg == "--help" || arg == "-h") {
			cout << argv[0] << ": a ProMC to HepEvn converter" << endl;
			cout << "Usage: " << argv[0] << "[<input ProMC file> <output HepEvn file>]" << endl;
			cout << "This program converts a ProMC file to ROOT" << endl;
			exit(0);
		}
	}

	// Choose input and output files from the command line
	string infile("-"), outfile("-");
	if (argc == 3) {
		infile = argv[1];
		outfile = argv[2];
	} else if (argc != 3) {
		cerr << "Usage: " << argv[0] << "[<PROMC files input> <HepEvn file out>]" << endl;
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


	cout << "\n -> Output HepEvn file =" << outfile << endl;
        ofstream out(outfile.c_str());
        bool longHepEvt = true;


	// loop over all events
	for (int nn=0; nn<nev; nn++){
		if (epbook->next() !=0) continue;
		ProMCEvent eve = epbook->get();

		// get truth information
		ProMCEvent_Particles  *pa=eve.mutable_particles();
                if (nn%100 ==0) cout << "# event= " << nn << endl;

                out << pa->pdg_id_size() << endl;            // NHEP, nr of particles in this event

		// read ProMC record.
		for (int j=0; j<pa->pdg_id_size(); j++){
                       // not used: pa->barcode_size()
                       // not used: pa->weight_size()
                       int  ISTHEP=pa->status(j);
                       int  IDHEP = pa->pdg_id(j);
                       out << setw(4) << ISTHEP << ' ';   // ISTHEP, status code
                       out << setw(8) << IDHEP << ' ';    // IDHEP, PDG code

                        if (longHepEvt) {
                                int JMOHEP1 = pa->mother1(j) > 0 ? pa->mother1(j)+1 : 0;
                                int JMOHEP2 = pa->mother2(j) > 0 ? pa->mother2(j)+1 : 0;
                                out << setw(4) << JMOHEP1 << ' ';                               // JMOHEP1, first mother id
                                out << setw(4) << (JMOHEP2 == 0 ? JMOHEP1 : JMOHEP2) << ' ';    // JMOHEP2, last mother id
                        }
 

                        int JDAHEP1 = pa->daughter1(j) > 0 ? pa->daughter1(j)+1 : 0;
                        int JDAHEP2 = pa->daughter2(j) > 0 ? pa->daughter2(j)+1 : 0; 
                        out << setw(4) << JDAHEP1 << ' ';                              // JDAHEP1, first daughter id
                        out << setw(4) << (JDAHEP2 == 0 ? JDAHEP1 : JDAHEP2) << ' ';   // JDAHEP2, last daughter id

                        // now start the info in double, so switch the output to that format
                        out.setf(ios_base::scientific);
                        out.precision(7);

                        out << setw(14) << pa->px(j)/kEV << ' ';         // PHEP1, px in GeV/c
                        out << setw(14) << pa->py(j)/kEV << ' ';         // PHEP2, py in GeV/c
                        out << setw(14) << pa->pz(j)/kEV << ' ';         // PHEP3, pz in GeV/c
                        if (longHepEvt)
                                out << setw(14) << pa->energy(j)/kEV << ' ';    // PHEP4, energy in GeV
                        out << setw(14) << pa->mass(j)/kEV << ' ';              // PHEP5, mass in GeV/cc

                        if (longHepEvt) {
                                out << setw(14) << pa->x(j)/kLe << ' ';      // VHEP1, x vertex pos in mm
                                out << setw(14) << pa->y(j)/kLe << ' ';      // VHEP2, y vertex pos in mm
                                out << setw(14) << pa->z(j)/kLe << ' ';      // VHEP3, z vertex pos in mm
                                out << setw(14) << pa->t(j)/kLe << ' ';      // VHEP4, production time in mm/c
                        }

                        out << endl;



		}




	} // end  event


        out.close();
	epbook->close(); // close


	cout << "Output HepEvt file: " << outfile << endl;

	return 0;
}
