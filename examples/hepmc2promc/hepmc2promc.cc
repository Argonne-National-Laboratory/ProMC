// Convert HEPMC files to ProMC format.  
// Author: Sergei Chekanov (ANL) 
// some helpful pieces als come from:
// Severine Ovyn                   Xavier Rouby      
// severine.ovyn@uclouvain.be      xavier.rouby@cern 


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


string getEnvVar( std::string const & key ) { 
  char * val = getenv( key.c_str() );
  return val == NULL ? std::string("") : std::string(val);
}

 

// find index to HepMC::GenParticle* p in map m
int find_in_map( const std::map<HepMC::GenParticle*,int>& m, HepMC::GenParticle *p) { 
  std::map<HepMC::GenParticle*,int>::const_iterator iter = m.find(p);
  return (iter == m.end()) ? 0 : iter->second;
}



//------------------------------------------------------------------------------
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
    pp->set_id(ID); // pi+ 
    pp->set_mass(mass);
    pp->set_name(name);
    pp->set_width(width);
    pp->set_lifetime(lifetime);
    pp->set_charge(charge);
    //cout << ID << " " << name << " " << mass << endl;
  }

} 




int main(int argc, char** argv) {

  // Look for a help argument
  for (int i = 1; i < argc; ++i) {
    const string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      cout << argv[0] << ": a HepMC to ProMC converter" << endl;
      cout << "Usage: " << argv[0] << "[<infile> <outfile>]" << endl;
      cout << "This program converts a HEPMC file to a ProMC file" << endl;
      exit(0);
    }
  }

  // Choose input and output files from the command line
  string infile("-"), outfile("-"), desc("none");
  if (argc == 4) {
    infile = argv[1];
    outfile = argv[2];
    desc=argv[3];
  } else if (argc != 4) {
    cerr << "Usage: " << argv[0] << "[<HEPMC files input> <ProMC file out> \"description\"]" << endl;
    exit(1);
  }


  // find out what is inside:
  int icount=0;
  HepMC::IO_GenEvent ascii_in(infile,std::ios::in);
  cout << "Pre-preprocessing the HEPMC file.. " << endl;
  HepMC::GenEvent* eevt = ascii_in.read_next_event();
  while ( eevt ) {
        icount++;
        delete eevt;
        ascii_in >> eevt;
  }


  cout << "Start this fill for " << icount << " input events.." << endl;

 // -------------- ProMC file ----------------------
 // use 0.01*MeV units
 const double kEV=1000*100.;
 // this is length unit. multiply CM by 1000
 const double uL=1000.;

  ProMCBook *epbook = new ProMCBook( outfile.c_str(),"w");
  epbook->setDescription(icount, desc);
  ProMCHeader header;
  // set units in 0.01*MeV for pX,pY,pZ 
  // in reader, we will read this back
  header.set_momentumunit((int)kEV);
  // set length unit. In Pythia, we multiply CM by 1000 
  header.set_lengthunit((int)uL);
  // other info
  header.set_id1(0);
  header.set_id2(0);
  header.set_pdf1( 0 );
  header.set_pdf2( 0  );
  header.set_x1(  0  );
  header.set_x2(  0   );
  header.set_scalepdf( 0  );
  header.set_weight( 0 );
  header.set_name("Converted HEPMC events");
  header.set_code(0);
  header.set_ecm(0);
  header.set_s(0);
  header.set_cross_section( 0 );
  header.set_cross_section_error( 0 );



  // let's store a map with most common masses:
  readPDG( &header );
  // write the header.
  epbook->setHeader(header); // write header 


  // Init readers and writers
  IO_GenEvent* reader = (infile == "-") ? new IO_GenEvent(cin) : new IO_GenEvent(infile, ios::in);

  std::vector<HepMC::GenParticle*> index_to_particle;
  std::map<HepMC::GenParticle*,int> particle_to_index;


  // Event loop
  int event_num = 0;
  GenEvent* evt = reader->read_next_event();
  while (reader->rdstate() == 0) {
     ++event_num; 

    if (event_num%100 ==0) cout << "#" << event_num << endl; 
    //evt->print();






  ProMCEvent promc; // create ProMC events


    // event info
  ProMCEvent_Event  *eve= promc.mutable_event();
  eve->set_number(evt->event_number());
  eve->set_process_id( evt->signal_process_id());
  eve->set_scale( evt->event_scale());
  eve->set_alpha_qed( evt->alphaQED());
  eve->set_alpha_qcd( evt->alphaQCD());


  // particle info    
  ProMCEvent_Particles  *par= promc.mutable_particles();
//  int mo1, mo2, da1, da2, status, pid;
 unsigned int particle_counter=0;
 index_to_particle.clear();
 particle_to_index.clear();
 index_to_particle.reserve(evt->particles_size());
 index_to_particle[0] = 0;
 HepMC::GenEvent::vertex_const_iterator v;
 for (v = evt->vertices_begin(); v != evt->vertices_end(); ++v )
   {
     // making a list of incoming particles of the vertices
     // so that the mother indices in HEPEVT can be filled properly
     HepMC::GenVertex::particles_out_const_iterator p1;
     for (p1 = (*v)->particles_in_const_begin();p1 != (*v)->particles_in_const_end(); ++p1 )
       {

         ++particle_counter;
         //particle_counter can be very large for heavy ions
         if(particle_counter >= index_to_particle.size() )
           {
             //make it large enough to hold up to this index
             index_to_particle.resize(particle_counter+1);
           }
         index_to_particle[particle_counter] = *p1;
         particle_to_index[*p1] = particle_counter;
       }
     // daughters are entered only if they aren't a mother of
     // another vertex
   HepMC::GenVertex::particles_out_const_iterator p2;
     for (p2 = (*v)->particles_out_const_begin();p2 != (*v)->particles_out_const_end(); ++p2)
       {
         if (!(*p2)->end_vertex())
           {
             ++particle_counter;
             //particle_counter can be very large for heavy ions
             if(particle_counter  >= index_to_particle.size() )
               {
                 //make it large enough to hold up to this index
                 index_to_particle.resize(particle_counter+1);
               }
             index_to_particle[particle_counter] = *p2;
             particle_to_index[*p2] = particle_counter;
           }
       }

   }




     for(int j=1; j<=evt->particles_size(); j++) {
      int mo1=0;
      int mo2=0;
      int da1=0;
      int da2=0;
      int status =  index_to_particle[j]->status();
      int pid = index_to_particle[j]->pdg_id();
      
      // get mothers
      if ( index_to_particle[j]->production_vertex() ) {

          int num_mothers = index_to_particle[j]->production_vertex()->particles_in_size();
          if (num_mothers ==0) {
            mo1 = 0;
            mo2 = 0;
          }
          else {
            int first_mother = find_in_map( particle_to_index,*(index_to_particle[j]->production_vertex()->particles_in_const_begin()));
            int last_mother = first_mother + num_mothers - 1;
            if ( first_mother == 0 ) last_mother = 0;
            mo1=first_mother;
            mo2=last_mother;
          } // if num_mothers !=0
        } else // no data on production_vertex
        {
          mo1 =0;
          mo2 =0;
        }
 
        
       if (index_to_particle[j]->end_vertex()) { 
          // make sure first and last daughter are indeed the first and last
          int first_daughter = find_in_map( particle_to_index,*(index_to_particle[j]->end_vertex()->particles_begin(HepMC::children)));
          int last_daughter = 0;

          HepMC::GenVertex::particle_iterator ic;
          for (ic = index_to_particle[j]->end_vertex()->particles_begin(HepMC::children);ic != index_to_particle[j]->end_vertex()->particles_end(HepMC::children); ++ic) {
            int current_daughter = find_in_map( particle_to_index,*ic) ;
            if (current_daughter < first_daughter)
              first_daughter = current_daughter;
            if (current_daughter > last_daughter)
              last_daughter = current_daughter;
          }
          if (first_daughter== 0) last_daughter = 0;
          da1=first_daughter;
          da2=last_daughter;

        }
      else
       {
          da1=0;
          da2=0;
        }
      
      
      
      par->add_id( j  );
      par->add_pdg_id(  pid );
      par->add_status( status );
      par->add_px( int(kEV*(index_to_particle[j]->momentum().px() )));
      par->add_py( int(kEV*(index_to_particle[j]->momentum().py() )));
      par->add_pz( int(kEV*(index_to_particle[j]->momentum().pz() )));
      par->add_energy( int(kEV*(index_to_particle[j]->momentum().e() )));
      par->add_mass( int(kEV*(index_to_particle[j]->generated_mass() )));
      par->add_barcode(index_to_particle[j]->barcode());
      par->add_daughter1( da1-1  );
      par->add_daughter2( da2-1  );
      par->add_mother1( mo1-1  );
      par->add_mother2( mo2-1  );
 
      
     GenVertex* vstart =  index_to_particle[j]->production_vertex();
     //GenVertex* vend =    index_to_particle[j]->end_vertex();
     GenVertex* vrtI = index_to_particle[j]->production_vertex();
     if(vrtI) {
       par->add_x( int(uL*(vstart->position().x() ) ));
       par->add_y( int(uL*(vstart->position().y() ) ));
       par->add_z( int(uL*(vstart->position().z() ) ));
       par->add_t( int(uL*(vstart->position().t() ) ));
     } else {
      par->add_x(0);
      par->add_y(0);
      par->add_z(0);
      par->add_t(0);
     }
 
      
      
      
      
      
      
   //   getStatsFromTuple(evt, mo1,mo2,da1,da2,status,pid,n);
    }


    delete evt;

    epbook->write(promc); // write ProMC event
    evt = reader->read_next_event();
  }

  delete reader;


  // save post-generation statistics for ProMC
   ProMCStat stat;
  // stat.set_cross_section_accumulated( sigmapb ); // in pb 
  // stat.set_cross_section_error_accumulated( pythia.info.sigmaErr() * 1e9 );
  // stat.set_luminosity_accumulated(  Ntot/sigmapb );
  // stat.set_ntried(pythia.info.nTried());
  // stat.set_nselected(pythia.info.nSelected());
  // stat.set_naccepted(pythia.info.nAccepted());
   epbook->setStatistics(stat);
 
  epbook->close(); // close

  return EXIT_SUCCESS;
}
