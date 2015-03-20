// Convert ProMC to HEPMC files 
// This code is experimental. It reads ProMC and tries to write HEPMC file.
// Author: Sergei Chekanov (ANL) 
// Based on the Pythia8 code.

#include "HepMC/GenEvent.h"
#include "HepMC/IO_GenEvent.h"
#include <cstdlib>
#include <string>
#include <cassert>
#include <iostream>
#include "promc/ProMC.pb.h"
#include "promc/ProMCBook.h"
#include "promc/ProMCHeader.pb.h"

using namespace std;
using namespace HepMC;


float kEV=0;
float kLe=0;
bool  m_print_inconsistency_errors=true; // false;



// main program to read ProMC and convert to HEPMC
int main(int argc, char** argv) {

  // Look for a help argument
  for (int i = 1; i < argc; ++i) {
    const string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      cout << argv[0] << ": a ProMC to HepMC converter" << endl;
      cout << "Usage: " << argv[0] << "[<input ProMC file> <output HEPMC file>]" << endl;
      cout << "This program converts a ProMC file to HepMC" << endl;
      exit(0);
    }
  }

  // Choose input and output files from the command line
  string infile("-"), outfile("-");
  if (argc == 3) {
    infile = argv[1];
    outfile = argv[2];
  } else if (argc != 3) {
    cerr << "Usage: " << argv[0] << "[<PROMC files input> <HEPMC file out>]" << endl;
    exit(1);
  }

 // -------------- ProMC file ----------------------
  ProMCBook *epbook = new ProMCBook( infile.c_str(),"r");
  // get the version number
  int  h=epbook->getVersion();
  // cout << "ProMC version = " << h << endl;
  // get the description of this file
  //string d=epbook->getDescription();
  //cout << "Description = " << d << endl;
  unsigned int  nev=epbook->getEvents();
  cout << "Events = " << nev  << endl;

  // get the header file with units, cross sections etc.
  ProMCHeader header = epbook->getHeader();
  //cout << "Cross section=" << header.cross_section() << endl;
  // here is the units for energy
  kEV=(float)(header.momentumunit());
  kLe=(float)(header.lengthunit()); 
  cout << "Energy unit=" << kEV << endl;
  cout << "Length unit=" << kLe << endl;
 

 
  IO_GenEvent* writer = (outfile == "-") ? new IO_GenEvent(cout) : new IO_GenEvent(outfile, ios::out);
  writer->write_comment(epbook->getDescription());
 
 // HepMC::GenCrossSection xsec;
 // xsec.set_cross_section( header.cross_section());
 // evt->set_cross_section(xsec);


  for (unsigned int j=0; j<nev; j++){ // loop over events
  if (epbook->next() !=0) continue;
  ProMCEvent eve = epbook->get();


  // event information
  ProMCEvent_Event  *e=eve.mutable_event();
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

  // Construct new empty HepMC event.
  GenEvent* hepmcevt = new GenEvent( signal_process_id, event_number );
  // define the units
  hepmcevt->use_units(HepMC::Units::GEV, HepMC::Units::MM);
  hepmcevt->set_event_number(event_number);
  hepmcevt->set_signal_process_id(signal_process_id);
  hepmcevt->set_event_scale(event_scale);
  hepmcevt->set_alphaQED( alphaQED );
  hepmcevt->set_alphaQCD( alphaQCD );
  hepmcevt->set_pdf_info( PdfInfo( id1pdf, id2pdf, x1pdf, x2pdf, Qfac, pdf1, pdf2) );


  // Conversion factors a units in GeV and mm to HepMC ones.
   double momFac = HepMC::Units::conversion_factor(HepMC::Units::GEV,
    hepmcevt->momentum_unit());
  double lenFac = HepMC::Units::conversion_factor(HepMC::Units::MM,
    hepmcevt->length_unit());


  // get truth particle information 
  ProMCEvent_Particles  *pa=eve.mutable_particles();


   std::vector<GenParticle*> hepevt_particles( pa->pdg_id_size()+1 );
   hepevt_particles[0] = 0;
   for ( int n = 0; n <  pa->pdg_id_size(); n++ ) {
          int id=pa->id(n); // position in the record 
          int pid=pa->pdg_id(n); // PDG id 
          //if (n==0 || n==1) cout << n << " Beam: " << pid << endl;
          int status=pa->status(n);
          float px=(pa->px(n)/kEV);
          float py=(pa->py(n)/kEV);
          float pz=(pa->pz(n)/kEV);
          float mass=(pa->mass(n)/kEV);
          int daughter1=pa->daughter1(n);
          int daughter2=pa->daughter2(n);
          int mother1=pa->mother1(n);
          int mother2=pa->mother2(n);
          //double e=sqrt(px*px+py*py+pz*pz+mass*mass);
          double e=(pa->energy(n)/kEV);
    hepevt_particles[n+1] = new GenParticle(HepMC::FourVector(px,py,pz,e),pid,status);
    hepevt_particles[n+1]->setGeneratedMass( mass );
    hepevt_particles[n+1]->suggest_barcode( n );
    }

  // Here we assume that the first two particles in the list
  // are the incoming beam particles.
   hepmcevt->set_beam_particles( hepevt_particles[1], hepevt_particles[2] );


   for (int  i = 0; i < pa->pdg_id_size(); ++i ) {

     // 3. Build the production_vertex (if necessary).
   GenParticle *p = hepevt_particles[i+1];
 
   vector<int> mothers;
  // Read out the two official mother indices and status code.
  int mother1   = pa->mother1(i);
  int mother2   = pa->mother2(i);
  int statusAbs =  pa->status(i);



 // MC dependent! 
  if (mother2 == 0 || mother2 == mother1) mothers.push_back(mother1);
  else if (abs(mother2-mother1)==1) {
   mothers.push_back( min(mother1, mother2) );
   mothers.push_back( max(mother1, mother2) );
   }



//      cout << mothers.size() << endl;
      unsigned int imother = 0;
      int mother = -1; // note that in Pythia8 there is a particle number 0!
      if ( !mothers.empty() ) mother = mothers[imother];
      GenVertex* prod_vtx = p->production_vertex();
      while ( !prod_vtx && mother > 0 ) {
        prod_vtx = hepevt_particles[mother]->end_vertex();
        if ( prod_vtx ) prod_vtx->add_particle_out( p );
        imother++;
        if ( imother < mothers.size() ) mother = mothers[imother];
        else mother = -1;
      }

     // 3b. If no suitable production vertex exists - and the particle has
      // at least one mother or position information to store - make one.

    float x=(pa->x(i)/kLe);
    float y=(pa->y(i)/kLe);
    float z=(pa->z(i)/kLe);
    float t=(pa->t(i)/kLe);

    HepMC::FourVector prod_pos(x,y,z,t);

      unsigned int nparents = mothers.size();
      if ( !prod_vtx && ( nparents > 0 || prod_pos != FourVector() ) ) {
        prod_vtx = new GenVertex();
        prod_vtx->add_particle_out( p );
        hepmcevt->add_vertex( prod_vtx );
      }

    // 3c. If prod_vtx doesn't already have position specified, fill it.
      if ( prod_vtx && prod_vtx->position() == FourVector() )
        prod_vtx->set_position( prod_pos );

      // 3d. loop over mothers to make sure their end_vertices are consistent.
      imother = 0;
      mother = -1;
      if ( !mothers.empty() ) mother = mothers[imother];
      while ( prod_vtx && mother > 0 ) {

        // If end vertex of the mother isn't specified, do it now.
        if ( !hepevt_particles[mother]->end_vertex() ) {
          prod_vtx->add_particle_in( hepevt_particles[mother] );

        // Problem scenario: the mother already has a decay vertex which 
        // differs from the daughter's production vertex. This means there is
        // internal inconsistency in the HEPEVT event record. Print an error.
        // Note: we could provide a fix by joining the two vertices with a 
        // dummy particle if the problem arises often.
        } else if (hepevt_particles[mother]->end_vertex() != prod_vtx ) {
         if ( m_print_inconsistency_errors ) std::cerr
            << "HepMC::inconsistent mother/daugher "
            << "information in the event " << std::endl
            << "i= " << i << " mother = " << mother
            << "\n This warning can be turned off with the "
            << "print_inconsistency_errors switch." << std::endl;
        }

        // Variant with motherList.
        imother++;
        if ( imother < mothers.size() ) mother = mothers[imother];
        else mother = -1;
      }

     // Variant with motherList.
        imother++;
        if ( imother < mothers.size() ) mother = mothers[imother];
        else mother = -1;

      }



  if (j%100==0)
          cout << j <<  " Nr particles=" << pa->pdg_id_size()  << endl;


  writer->write_event(hepmcevt); // write HEPMC event 


  } // end loop over events


  ProMCStat stat = epbook->getStatistics();
  double xcross=stat.cross_section_accumulated();
  double err=stat.cross_section_error_accumulated();

  HepMC::GenCrossSection xsec;
  xsec.set_cross_section( xcross, err);
 // hepmcevt->set_cross_section(xsec);
//  hepmcevt->weights().push_back( 1.0d0 );



 
  epbook->close(); // close
  delete writer;

  return EXIT_SUCCESS;
}
