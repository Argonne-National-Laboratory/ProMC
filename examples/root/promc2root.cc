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
#include <TH1D.h>
#include "TApplication.h"
#include "TObject.h"
#include "TMath.h"
#include "TRandom.h"
#include<TClonesArray.h>
#include"time.h"
#include"LParticle.h"
#include "promc/ProMC.pb.h"
#include "promc/ProMCBook.h"


using namespace std;
using namespace promc;


int main(int argc, char **argv)
{



  ProMCBook*  epbook = new ProMCBook("../random/out/output.promc","r");
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
  cout << "Momentum units 0.01*MeV=" << kEV << endl;
  double kLe=(double)(header.lengthunit());
  cout << "Length unit=" << kLe << endl;


  string outputfile="out/output.root";
  cout << "\n -> Output file is =" << outputfile << endl;
  TFile * RootFile = new TFile(outputfile.c_str(), "RECREATE", "Histogram file");
//  RootFile->SetCompressionLevel(0);

   TTree *  m_ntuple  = new TTree("Ntuple","Ntuple");
   m_ntuple->SetAutoFlush(100000);
   LParticle* partVec = 0;
   m_ntuple->Branch("particles", "LParticle", &partVec, 256000, 99);


  // loop over all events
  for (unsigned int nn=0; nn<nev; nn++){
  if (epbook->next() !=0) continue;
  ProMCEvent eve = epbook->get();

  // get truth information 
  ProMCEvent_Particles  *pa=eve.mutable_particles();


  if (nn%10==0)
          cout << nn <<  " Nr particles=" << pa->pdg_id_size()  << endl;


   // fill tree from ProMC record. 
   for (int j=0; j<pa->pdg_id_size(); j++){
    LParticle  p(pa->px(j)/kEV,pa->py(j)/kEV,pa->pz(j)/kEV,pa->mass(j)/kEV);
    p.SetStatus(pa->status(j));
    p.SetType(pa->pdg_id(j));
    p.SetVertex(pa->x(j),pa->y(j),pa->z(j),pa->t(j));
    p.SetDaughters(pa->daughter1(j),pa->daughter2(j));
    p.SetMothers(pa->mother1(j),pa->mother2(j));
    p.SetCharge(pa->barcode(j));

   partVec = &p;
   m_ntuple->Fill();
   }


  }


  epbook->close(); // close


   m_ntuple->Fill();

   RootFile->Write();
   //RootFile->Print();
   RootFile->Close();

   cout << "Writing ProMC record to ROOT file out/output.root" << endl;

    return 0;
}
