// Convert TXT file to ProMC format.  
// Author: Sergei Chekanov (ANL) 

#include <cstdlib>
#include <string>
#include <cassert>
#include <iostream>
#include <stdio.h>
#include "promc/ProMC.pb.h"
#include "promc/ProMCBook.h"

using namespace std;


string getEnvVar( std::string const & key ) { 
  char * val = getenv( key.c_str() );
  return val == NULL ? std::string("") : std::string(val);
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


/*
 * A convertion of TXT to ProMC. During the convertion, you can set the descrription,
 * number of events, cross section and its error. 
 *
 * */
int main(int argc, char** argv) {

  // Look for a help argument
  for (int i = 1; i < argc; ++i) {
    const string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      cout << argv[0] << ": a TXT to ProMC converter" << endl;
      cout << "Usage: " << argv[0] << "[<TXT infile> <ProMC outfile>]" << endl;
      cout << "   or: " << argv[0] << "[<TXT input> <ProMC out> \"description\" \"NrEvents\" \"Cross Section (pb)\" \"Erroron Cross section (pb)\" ]" << endl;
      cout << "\nThis program converts LHEF file to ProMC file. \nYou can pass the description, Nr of events, cross section and it's error as arguments. \nIn this case, LHEF internal values will not be used. - S.Chekanov - \n" << endl;
      exit(0);
    }
  }

  // Choose input and output files from the command line
  string infile("-"), outfile("-"), desc("none");
  string events("-1"),cross_section("-1"),cross_section_error("-1"); 
  if (argc == 4) {
    infile = argv[1];
    outfile = argv[2];
    desc=argv[3];
  } else if (argc == 7) {
    infile = argv[1];
    outfile = argv[2];
    desc=argv[3];
    events=argv[4];
    cross_section=argv[5];
    cross_section_error=argv[6];
  } else if (argc != 4 and argc != 7) {
    cerr << "Usage: " << argv[0] << "[<TXT input> <ProMC out> \"description\"] OR" << endl;
    cerr << "   or  " << argv[0] << "[<TXT input> <ProMC out> \"description\" \"NrEvents\" \"cross section (pb)\" \"Error on the cross section (pb)\" ]" << endl;
    exit(1);
  }

   int valueNrEvents = atoi(events.c_str());  
   double valueCrossX=atof(cross_section.c_str()); // in pb    
   double valueCrossXerror=atof(cross_section_error.c_str()); // in pb

   if (valueNrEvents>-1){
     cout << "txt2promc:  Nr of input events  is  set to: " << valueNrEvents << endl; 
   }
   if (valueCrossX>-1){
     cout << "txt2promc: Cross section (in pb) is  set to:  " << valueCrossX << endl;
     cout << "txt2promc: Error on cross section is set to:  " << valueCrossXerror << endl;
   }



 cout << "Read file=" << infile.c_str() << endl;

 // -------------- ProMC file ----------------------
 // use 0.01*MeV units for 13 TeV 
 const double kEV=1000*100.;
 // this is length unit. multiply CM by 1000
 const double uL=1000.;

  ProMCBook *epbook = new ProMCBook( outfile.c_str(),"w");
  epbook->setDescription(valueNrEvents, desc);
  ProMCHeader header;
  // set units in 0.01*MeV for pX,pY,pZ 
  // in reader, we will read this back
  header.set_momentumunit((int)kEV);
  // set length unit. In Pythia, we multiply CM by 1000 
  header.set_lengthunit((int)uL);
  // other info


  header.set_id1( -11 ); // PID of incoming particles 
  header.set_id2( 11 );
  header.set_x1(  0  );
  header.set_x2(  0   );
  header.set_scalepdf( 0  );

  header.set_pdf1( 0); // put PDF of incoming particles  
  header.set_pdf2( 0);
  header.set_weight( 1 );
  header.set_name("TXT converted file");

  double ECM=250;
  header.set_code(1);
  header.set_ecm(ECM);
  header.set_s(ECM);
  if (valueCrossX>-1 && valueCrossXerror>-1) {
        header.set_cross_section(valueCrossX);
        header.set_cross_section_error(valueCrossXerror);
  } 


  // let's store a map with most common masses:
  readPDG( &header );
  // write the header.
  epbook->setHeader(header); // write header 


  std::ifstream file(infile.c_str());
  std::string line; 
  int nev=0; int particle=1;
  bool  new_event=true;
  ProMCEvent promc; // create ProMC event

  while (std::getline(file, line))
    {
      istringstream linestream(line);  
      if (line == "#END") {new_event = true; epbook->write(promc); } // write ProMC event 
      if (line.substr(0,1) == "#") continue;
      // Process str
      std::string name; 
      int ii, isthep,idhep,m1,m2,d1,d2;
      double px,py,pz,e,mass,x,y,z,t;
      linestream >> ii>> name >> idhep >> isthep >>  m1 >> m2 >> d1 >> d2 >> px >> py >> pz>> e>> mass>> x >> y >> z>> t; 

      // start a new event and fill some info
      if (new_event) { 
          // event info
          ProMCEvent_Event  *eve= promc.mutable_event();
          eve->set_number(nev+1);
          eve->set_scale(1.0);  // define scale of the process (optional) 
          eve->set_weight(1.0); // event weight (optional)  
          eve->set_alpha_qed(1.0);
          eve->set_alpha_qcd(1.0);
          eve->set_process_id(0); // process ID (optional)  
          particle=1;
      cout << "Read event =" << nev << endl;
      nev++;
      } else { // fill the current event with particles 

      ProMCEvent_Particles  *par= promc.mutable_particles();
      par->add_id( particle  );
      par->add_pdg_id(  idhep ); // IDHEP PDG ID 
      par->add_status( isthep ); // ISTHEP (1=stable) 
      par->add_px( int(kEV*px));
      par->add_py( int(kEV*py));
      par->add_pz( int(kEV*pz));
      par->add_energy( int(kEV*e));
      par->add_mass( int(kEV*mass));
      par->add_barcode(0);       // optional 
      par->add_daughter1( d1  );
      par->add_daughter2( d2  );
      par->add_mother1( m1  );
      par->add_mother2( m2  );
      // vertex
      par->add_x( int(uL*x ));
      par->add_y( int(uL*y ));
      par->add_z( int(uL*z ));
      par->add_t( int(uL*t ));
      particle++; 
     } 
      // continue to fill particles 
      new_event =false;
    }


   std::cout << nev << " events processed" << std::endl;


  // save post-generation statistics for ProMC
   ProMCStat stat;
   // stat.set_cross_section_accumulated( sigmapb ); // in pb 
   stat.set_cross_section_accumulated( valueCrossX);
   stat.set_cross_section_error_accumulated(valueCrossXerror);
   stat.set_luminosity_accumulated(  nev/valueCrossX );
   cout << "Cross section=" << valueCrossX << "+-" << valueCrossXerror << " pb-1" << endl;
   cout << "Luminosity   =" << nev/valueCrossX << " pb-1" << endl;
   stat.set_ntried(nev);
   stat.set_nselected(nev);
   stat.set_naccepted(nev);
   epbook->setStatistics(stat);
   epbook->close(); // close

  return EXIT_SUCCESS;
}
