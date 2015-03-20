/***************************************************************************
 *  Copyright  2013 S.Chekanov (ANL)  
 *  chekanov@anl.gov
 *  A library for HEP events storage and processing based on Google's PB   
****************************************************************************/


#include<iostream>
#include<fstream>
#include<stdlib.h>
#include"time.h"
#include"pronlo/ProMCBook.h"

using namespace std;
using namespace promc;

// shows how to read NLO file

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

        double tweight=0; // global weight 

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

       // this is global weight, which is Nr of iterations
       // divide event weight by this number
       int nIterations=(int)header.weight();
       cout << "Number of iterations=" << nIterations << endl;

	// loop over all events
	for (int nn=0; nn<nev; nn++){
		if (epbook->next() !=0) continue;
                ProMCEvent eve = epbook->get();


                 // event information
                 ProMCEvent_Event  *e=eve.mutable_event();
                 ProMCEvent_Particles  *pa=eve.mutable_particles();
           	
                 if (e->fdata_size()<1) {
                      cout << "Nr of weight is 0 =" << e->fdata_size() << endl;
                      continue;
                  }



                if  (nn%1000 == 0) cout << "# event= " << nn << endl; 

                float wi=e->fdata(0); // this is a global weight  
                tweight = tweight + wi;
                 

                wi=e->fdata(0)/(double)nIterations; // this is a global weight/ N iterations


                int Mpdf=e->idata_size(); // Nr of PDFs

                // PDF weights
                 double uL=1000.;
                for(int i=0; i<Mpdf; i++) {
                        double pdf_err = 1.0+(e->idata(i))/uL;
                  }

		// particle NLO record.
		for (int j=0; j<pa->pdg_id_size(); j++){
		// int id=pa->id(j);
		double px=pa->px(j)/kEV;
                double py=pa->py(j)/kEV;
		double pz=pa->pz(j)/kEV;
		double energy=pa->energy(j)/kEV;
		int pid=pa->pdg_id(j);
		}


	} // end  event

        ProMCStat stat = epbook->getStatistics();
        double cross=stat.cross_section_accumulated();
        double error=stat.cross_section_error_accumulated();
        cout << "Cross section=" << cross << " +- " << error << " pb" << endl;
 

	epbook->close(); // close

	return 0;
}
