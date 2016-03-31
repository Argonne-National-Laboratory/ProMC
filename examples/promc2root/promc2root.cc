/***************************************************************************
 *  Copyright  2013 S.Chekanov (ANL)  
 *  chekanov@anl.gov
 *  A library for HEP events storage and processing based on Google's PB   
****************************************************************************/


#include<iostream>
#include<fstream>
#include<stdlib.h>

#include <TROOT.h>
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>
#include "TApplication.h"
#include "TObject.h"
#include "TMath.h"
#include"time.h"
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
			cout << argv[0] << ": a ProMC to ROOT converter" << endl;
			cout << "Usage: " << argv[0] << "[<input ProMC file> <output ROOT file>]" << endl;
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
		cerr << "Usage: " << argv[0] << "[<PROMC files input> <ROOT file out>]" << endl;
		exit(1);
	}

	std::vector<Int_t> m_barcode,m_id,m_status,m_pdg_id,m_m1, m_m2,m_d1,m_d2;
	std::vector<Double32_t> m_Px, m_Py, m_Pz, m_E, m_M, m_weight;
	std::vector<Double32_t> m_x, m_y, m_z, m_t;
        std::vector<Double32_t> m_charge;


	ProMCBook *epbook = new ProMCBook( infile.c_str(),"r");

        // if ProMC file is made using zip64, use
        //ProMCBook *epbook = new ProMCBook( infile.c_str(),"r",true);


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

        // do you need Particle data and names?
        // this is optional. You may need particle masses and names
        // stored directly inside the ProMC file
        std::map <int,float> charges;

        for (int i=0; i<header.particledata_size(); i++){
          ProMCHeader_ParticleData p= header.particledata(i);
          string name=p.name();
          int    id=p.id(); 
          int    charge=p.charge();
          double mass=p.mass();
          double width=p.width();
          double lifetime = p.lifetime();
          cout << "Reading PDG=" << i << " is= " << id << " name=" <<  name << " cha=" << charge/3.0 << endl;
          charges[id]=charge/3.0; 
        }

	cout << "\n -> Output ROOT file =" << outfile << endl;
	TFile * RootFile = new TFile(outfile.c_str(), "RECREATE", "ProMC record");
	//  RootFile->SetCompressionLevel(0);
	if (!RootFile){
		std::cout << "RootTuple:: Error: Cannot create ROOT file" << std::endl;
		exit(1);

	}

	TTree *  m_tree  = new TTree("ProMC","ProMC");
	m_tree->SetAutoFlush(100000);

	m_tree->Branch("id",       &m_id);
	m_tree->Branch("pdg_id",   &m_pdg_id);
	m_tree->Branch("status",   &m_status);
	m_tree->Branch("weight",   &m_weight);
	m_tree->Branch("barcode",  &m_barcode);
        m_tree->Branch("charge",  &m_charge);
	m_tree->Branch("Px",       &m_Px);
	m_tree->Branch("Py",       &m_Py);
	m_tree->Branch("Pz",       &m_Pz);
	m_tree->Branch("E",        &m_E);
	m_tree->Branch("M",        &m_M);
	m_tree->Branch("X",        &m_x);
	m_tree->Branch("Y",        &m_y);
	m_tree->Branch("Z",        &m_z);
	m_tree->Branch("T",        &m_t);
	m_tree->Branch("mother1",       &m_m1);
	m_tree->Branch("mother2",       &m_m2);
	m_tree->Branch("daughter1",     &m_d1);
	m_tree->Branch("daughter2",     &m_d2);



	// loop over all events
	for (int nn=0; nn<nev; nn++){
		if (epbook->next() !=0) continue;
		ProMCEvent eve = epbook->get();


		m_id.clear();
		m_barcode.clear();
                m_charge.clear();
		m_Px.clear();
		m_Py.clear();
		m_Pz.clear();
		m_E.clear();
		m_M.clear();
		m_x.clear();
		m_y.clear();
		m_z.clear();
		m_t.clear();
		m_id.clear();
		m_status.clear();
		m_pdg_id.clear();
		m_m1.clear();
		m_m2.clear();
		m_d1.clear();
		m_d2.clear();
		m_weight.clear();

		// get truth information
		ProMCEvent_Particles  *pa=eve.mutable_particles();
                if (nn%100 ==0) cout << "# event= " << nn << endl;

		// fill tree from ProMC record.
		for (int j=0; j<pa->pdg_id_size(); j++){
			m_id.push_back(pa->id(j));
			if (pa->barcode_size()==0) m_barcode.push_back(0);
			else m_barcode.push_back(pa->barcode(j));
			m_barcode.push_back(j);
                        m_charge.push_back( charges[pa->id(j)] );
			m_Px.push_back(pa->px(j)/kEV);
			m_Py.push_back(pa->py(j)/kEV);
			m_Pz.push_back(pa->pz(j)/kEV);
			m_E.push_back(pa->energy(j)/kEV);
			m_M.push_back(pa->mass(j)/kEV);
			m_status.push_back(pa->status(j));
			m_pdg_id.push_back(pa->pdg_id(j));
			m_m1.push_back(pa->mother1(j));
			m_m2.push_back(pa->mother2(j));
			m_d1.push_back(pa->daughter1(j));
			m_d2.push_back(pa->daughter2(j));
			if (pa->weight_size()==0) m_weight.push_back(1.0);
			else m_weight.push_back(pa->weight(j));
			m_x.push_back(pa->x(j)/kLe);
			m_y.push_back(pa->y(j)/kLe);
			m_z.push_back(pa->z(j)/kLe);
			m_t.push_back(pa->t(j)/kLe);
		}


		m_tree->Fill();


	} // end  event


	epbook->close(); // close



	RootFile->Write();
	//RootFile->Print();
	RootFile->Close();

	cout << "Output ROOT file: " << outfile << endl;

	return 0;
}
