// main02.cc is a part of the PYTHIA event generator.
// Copyright (C) 2009 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL version 2, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.
// 
//  S.Chekanov (ANL). 
//  Add an example of how to write ProMC files   
#include <map>
#include "Pythia.h"
// ProMC file
#include "promc/ProMC.pb.h"

using namespace Pythia8; 

string getEnvVar( std::string const & key ) {
  char * val = getenv( key.c_str() );
  return val == NULL ? std::string("") : std::string(val);
}


void readPDG( ProMCHeader * header  ) {

  string temp_string;
  istringstream curstring;
  string  PdgTableFilename  = getEnvVar("PROMC")+"/data/particle.tbl";
  if (PdgTableFilename.size()<2) {
  cout <<"**        ERROR: PROMC variable not set. Did you run source.sh" <<
         "      **" << endl;
    exit(1);
  }

  ifstream fichier_a_lire(PdgTableFilename.c_str());
  if(!fichier_a_lire.good()) {
    cout <<"**        ERROR: PDG Table ("<< PdgTableFilename
         <<  ") not found! exit.                        **" << endl;
    exit(1);
    return;
  }

  // first three lines of the file are useless 
  getline(fichier_a_lire,temp_string);
  getline(fichier_a_lire,temp_string);
  getline(fichier_a_lire,temp_string);
  while (getline(fichier_a_lire,temp_string)) {
    curstring.clear(); // needed when using several times istringstream::str(string)
    curstring.str(temp_string);
    long int ID; std::string name; int charge; float mass; float width; float lifetime;
    // ID name   chg       mass    total width   lifetime
    //  1 d      -1      0.33000     0.00000   0.00000E+00
    //  in the table, the charge is in units of e+/3
    //  the total width is in GeV
    //  the lifetime is ctau in mm
    curstring >> ID >> name >> charge >> mass >> width >> lifetime;
    ProMCHeader_ParticleData* pp= header->add_particledata();
    pp->set_id(ID); 
    pp->set_mass(mass);
    pp->set_name(name);
    pp->set_width(width);
    pp->set_lifetime(lifetime);
    cout << ID << " " << name << " " << mass << endl;
  }

}


int main() {

 int Ntot=1000; // total events

  // Generator. Process selection. Tevatron initialization. Histogram.
 Pythia pythia;
 pythia.readString("WeakSingleBoson:ffbar2gmZ = on");
 pythia.readString("PhaseSpace:mHatMin = 80.");
 pythia.readString("PhaseSpace:mHatMax = 120.");
 pythia.readString("Random:setSeed = on");
 pythia.readString("Random:seed = 0");
 pythia.init( 2212, 2212, 14000.);

 pythia.settings.listChanged(); // Show changed settings
 pythia.particleData.listChanged(); // Show changed particle data

  // ****************  book ProMC file **********************
  ProMCBook*  epbook = new ProMCBook("Pythia8.promc","w");
  epbook->setDescription(Ntot,"PYTHIA8");
  ProMCHeader header;
  header.set_id1( pythia.info.idA() );
  header.set_id2( pythia.info.idB() );
  header.set_name(pythia.info.name()); 
  header.set_code(pythia.info.code());
  header.set_ecm(pythia.info.eCM());
  header.set_s(pythia.info.s());

 // Use the range 0.01 MeV to 20 TeV using varints (integers)  
 // if particle in GeV, we mutiple it by kEV, to get 0.01 MeV =1 unit
 const double kEV=1000*100;

  // With particle in mm, we multiply it by kL to get 0.01 mm = 1 unit. 
  const double kL = 100;

  header.set_momentumunit((int)kEV);
  header.set_lengthunit((int)kL);

   // store a map with PDG information (stored in the header) 
  readPDG( &header );
  epbook->setHeader(header); // write header 
 
  // Begin event loop. Generate event. Skip if error. List first one.
  for (int n = 0; n < Ntot; n++) {
    if (!pythia.next()) continue;
    if (n < 3) {pythia.info.list(); pythia.event.list();}
    // Loop over particles in event. Find last Z0 copy. Fill its pT. 

   // If failure because reached end of file then exit event loop.
      if (pythia.info.atEndOfFile()) {
        cout << " Aborted since reached end of Les Houches Event File\n";
        break;
      }

  //************  ProMC file ***************//
  ProMCEvent promc;

  // fill event information
  ProMCEvent_Event  *eve= promc.mutable_event();
  eve->set_number(n);
  eve->set_process_id(pythia.info.code());     // process ID 
  eve->set_scale(pythia.info.pTHat());       
  eve->set_alpha_qed(pythia.info.alphaEM()); 
  eve->set_alpha_qcd(pythia.info.alphaS()); 
  eve->set_scale_pdf(pythia.info.QFac());  
  eve->set_weight(pythia.info.weight());  
  eve->set_x1(pythia.info.x1pdf());  
  eve->set_x2(pythia.info.x2pdf()); 
  eve->set_id1(pythia.info.id1pdf()); 
  eve->set_id2(pythia.info.id2pdf()); 
 

  // fill truth particle information 
  ProMCEvent_Particles  *pa= promc.mutable_particles();

  for (int i =0; i<pythia.event.size(); i++) {
  pa->add_pdg_id( pythia.event[i].id() );
  pa->add_status(  pythia.event.statusHepMC(i) ); // particle status in HEPMC style 
  pa->add_px( (int)(pythia.event[i].px()*kEV) );
  pa->add_py(  (int)(pythia.event[i].py()*kEV) );
  pa->add_pz(   (int)(pythia.event[i].pz()*kEV)  );
  pa->add_mass( (int)(pythia.event[i].m()*kEV) ); 
  pa->add_mother1( pythia.event[i].mother1()  );
  pa->add_mother2( pythia.event[i].mother2()  );
  pa->add_daughter1( pythia.event[i].daughter1()  );
  pa->add_daughter2( pythia.event[i].daughter2()   );
  pa->add_barcode( i  ); // dummy 
  pa->add_id( i  );
  pa->add_x( int(pythia.event[i].xProd()*kL)  );
  pa->add_y( int(pythia.event[i].yProd()*kL)  );
  pa->add_z( int(pythia.event[i].zProd()*kL)  );
  pa->add_t( int(pythia.event[i].tProd()*kL)  );
  }

   epbook->write(promc); // write event



  } // endl loop over events 


   // To check which changes have actually taken effect 
  pythia.settings.listChanged();
  pythia.particleData.list(25);

  pythia.statistics();

  double sigmapb = pythia.info.sigmaGen() * 1.0E9;
  cout << "== Cross section for this run=" <<  sigmapb << " pb" << endl; 
  cout << "== Events for this run=" <<  Ntot << endl;
  double lumi=(Ntot/sigmapb)/1000;
  cout << "== Luminosity for this run=" <<  lumi  << " fb-1" << endl;
  cout << "\n\n";

  // save post-generation statistics for ProMC
  ProMCStat stat;
  stat.set_cross_section_accumulated( sigmapb ); // in pb 
  stat.set_cross_section_error_accumulated( pythia.info.sigmaErr() * 1e9 );
  stat.set_luminosity_accumulated(  Ntot/sigmapb );
  stat.set_ntried(pythia.info.nTried());
  stat.set_nselected(pythia.info.nSelected());
  stat.set_naccepted(pythia.info.nAccepted());
  epbook->setStatistics(stat); 

  // close the ProMC file
  epbook->close(); // close


  return 0;
}

