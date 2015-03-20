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
#include"ProMCHeader.pb.h"
#include"ProMC.pb.h"
#include"ProMCBook.h"

using namespace std;
using namespace promc; 

int main(int argc, char **argv)
{


  ProMCBook*  epbook = new ProMCBook("out/output.promc","r");


  cout << "\n\n Start to read this file.." << endl;

  // get the version number
  int  h=epbook->getVersion();
  cout << "Version = " << h << endl;
  // get the description of this file
  string d=epbook->getDescription();
  cout << "Description = " << d << endl;
  unsigned int  nev=epbook->getEvents();
  cout << "Events = " << nev  << endl;



  // get the header file with units, cross sections etc.
  ProMCHeader header = epbook->getHeader();
  cout << "Cross section=" << header.cross_section() << endl;

  // here is the units for energy
  double kEV=(double)(header.momentumunit());
  cout << "Momentum units 0.01*MeV=" << kEV << endl;
  //cout << "Nr of entries = " << epbook->size() << endl;


  // ProMCEvent eve = epbook->event(21);
  // get truth information 
  // ProMCEvent_Particles  *pa=eve.mutable_particles();
  // cout << " ####  Nr particles=" << pa->pdg_id_size()  << endl;

  for (unsigned int j=0; j<nev; j++){
  if (epbook->next() !=0) continue;
  ProMCEvent eve = epbook->get();

  // get truth information 
  ProMCEvent_Particles  *pa=eve.mutable_particles();
 

  if (j%100==0)
          cout << j <<  " Nr particles=" << pa->pdg_id_size()  << endl;
  }




/*
  // loop over all events
  int nn=0;
  while (epbook->next()==0) {
  ProMCEvent eve = epbook->get();
  if (&eve == NULL) continue;
  if (nn%10==0)
          cout << nn <<  " Nr particles=" << eve.pdg_id_size()  << endl;
  // for (unsigned int j=0; j<eve.pdg_id_size(); j++){
  //      cout << "  " << j << " " << eve.pdg_id(j) << " " << eve.px(j)/kEV << " " << eve.py(j)/kEV << endl; 
  // }

  nn++;
  } 
*/

  epbook->close(); // close
 


  return 0;
}
