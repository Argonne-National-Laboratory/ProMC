/***************************************************************************
 *  Copyright  2013 S.Chekanov (ANL)  
 *  chekanov@anl.gov
 *  A library for HEP events storage and processing based on Google's PB   
****************************************************************************/


#include<iostream>
#include<fstream>
#include<stdlib.h>
#include"time.h"
#include "promc/ProMCBook.h"

using namespace std;
using namespace promc;


int main(int argc, char **argv)
{

	// Look for a help argument
	for (int i = 1; i < argc; ++i) {
		const string arg = argv[i];
		if (arg == "--help" || arg == "-h") {
			cout << argv[0] << ": a ProMC to ROOT converter" << endl;
			cout << "Usage: " << argv[0] << "[<input ProMC file>]" << endl;
			cout << "This program converts a ProMC file to ROOT" << endl;
			exit(0);
		}
	}

	// Choose input and output files from the command line
	string infile("-");
	if (argc == 2) {
		infile = argv[1];
	} else if (argc != 2) {
		cerr << "Usage: " << argv[0] << "[<PROMC files input>]" << endl;
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

	// loop over all events
	for (int nn=0; nn<nev; nn++){
		if (epbook->next() !=0) continue;
		ProMCEvent eve = epbook->get();
		// get truth information
		ProMCEvent_Particles  *pa=eve.mutable_particles();
                if  (nn%100 == 0) cout << "# event= " << nn << endl; 

		// shows how to access particles 
		for (int j=0; j<pa->pdg_id_size(); j++){
		int id=pa->id(j);
	        int barcode=0;
                if (pa->barcode_size()>0) pa->barcode(j);
		double px=pa->px(j)/kEV;
                double py=pa->py(j)/kEV;
		double pz=pa->pz(j)/kEV;
		double energy=pa->energy(j)/kEV;
		double mass=0;
                if (pa->mass_size()>0) mass=pa->mass(j)/kEV;
	        int status=pa->status(j);
		int pid=pa->pdg_id(j);
		int mother1=pa->mother1(j);
	        int mother2=pa->mother2(j);
	        int daughter1=pa->daughter1(j);
	        int daughter2=pa->daughter2(j);
                double weight=1.0; 
                if (pa->weight_size()>0) weight=pa->weight(j);
                int x=pa->x(j)/kLe;
                int y=pa->x(j)/kLe;
                int z=pa->z(j)/kLe; 
	        int time=pa->t(j)/kLe;
		}


	} // end  event

        ProMCStat stat = epbook->getStatistics();
        double cross=stat.cross_section_accumulated();
        double error=stat.cross_section_error_accumulated();
        cout << "Cross section=" << cross << " +- " << error << " pb" << endl;
 

	epbook->close(); // close

	return 0;
}
