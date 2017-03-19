// Convert LHE files to ProMC format.  
// Author: Sergei Chekanov (ANL) 


#include <cstdlib>
#include <string>
#include <cassert>
#include <iostream>
#include <stdio.h>

// LHE version 1
#include "LHEF.h"
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
 * A convertion of STDHEP to ProMC. During the convertion, you can set the descrription,
 * number of events, cross section and its error. 
 *
 * */
int main(int argc, char** argv) {

    int ierr, i, lbl;
    int istr = 0;
    int nevt = 0;

  // Look for a help argument
  for (int i = 1; i < argc; ++i) {
    const string arg = argv[i];
    if (arg == "--help" || arg == "-h") {
      cout << argv[0] << ": LHE to ProMC converter" << endl;
      cout << "Usage: " << argv[0] << "[<LHEF infile> <ProMC outfile>]" << endl;
      cout << "   or: " << argv[0] << "[<LHEF input> <ProMC out> \"description\" \"NrEvents\" \"Cross Section (pb)\" \"Erroron Cross section (pb)\" ]" << endl;
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
    cerr << "Usage: " << argv[0] << "[<LHEF input> <ProMC out> \"description\"] OR" << endl;
    cerr << "   or  " << argv[0] << "[<LHEF input> <ProMC out> \"description\" \"NrEvents\" \"cross section (pb)\" \"Error on the cross section (pb)\" ]" << endl;
    exit(1);
  }

   int valueNrEvents = atoi(events.c_str());  
   double valueCrossX=atof(cross_section.c_str()); // in pb    
   double valueCrossXerror=atof(cross_section_error.c_str()); // in pb

   if (valueNrEvents>-1){
     cout << "lhe2promc:  Nr of input events  is  set to: " << valueNrEvents << endl; 
   }
   if (valueCrossX>-1){
     cout << "lhe2promc: Cross section (in pb) is  set to:  " << valueCrossX << endl;
     cout << "lhe2promc: Error on cross section is set to:  " << valueCrossXerror << endl;
   }



    cout << "Read file=" << infile.c_str() << endl;
    LHEF::Reader reader(infile.c_str());
    cout << reader.initComments << endl;


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


  header.set_id1( reader.heprup.IDBMUP.first );
  header.set_id2( reader.heprup.IDBMUP.second );
  header.set_x1(  0  );
  header.set_x2(  0   );
  header.set_scalepdf( 0  );

  header.set_pdf1( reader.heprup.PDFGUP.first);
  header.set_pdf2( reader.heprup.PDFGUP.second);
  header.set_weight( reader.heprup.IDWTUP );
  header.set_name("LHE converted file");

  double ECM=reader.heprup.EBMUP.first+reader.heprup.EBMUP.second;
  header.set_code(1);
  header.set_ecm(ECM);
  header.set_s(ECM);
  if (valueCrossX>-1 && valueCrossXerror>-1) {
        header.set_cross_section( valueCrossX);
        header.set_cross_section_error(valueCrossXerror);
  } else { 
        header.set_cross_section(  reader.heprup.XSECUP[0] );
        header.set_cross_section_error( reader.heprup.XERRUP[0] );
  }

  // let's store a map with most common masses:
  readPDG( &header );
  // write the header.
  epbook->setHeader(header); // write header 

   i = 0;
   while ( reader.readEvent() ) {
          ProMCEvent promc; // create ProMC event
          // event info
          ProMCEvent_Event  *eve= promc.mutable_event();

          eve->set_number(reader.hepeup.NUP+2);
          eve->set_scale(reader.hepeup.SCALUP);
          eve->set_weight(reader.hepeup.weight());
          eve->set_alpha_qed(reader.hepeup.AQEDUP);
          eve->set_alpha_qcd(reader.hepeup.AQCDUP);
          eve->set_process_id(0); // not set 
         
      double px=0;
      double py=0; 
      double pz=reader.heprup.EBMUP.first;
      double e=reader.heprup.EBMUP.first; 
      double mass=0;
      double x=0;
      double y=0;
      double z=0;
      double t=0;

      // beam particles
      ProMCEvent_Particles  *par= promc.mutable_particles();
      par->add_id( 0  );
      par->add_pdg_id(  reader.heprup.IDBMUP.first );
      par->add_status( 11 ); // as in pythia 
      par->add_px( int(kEV*px));
      par->add_py( int(kEV*py));
      par->add_pz( int(kEV*pz));
      par->add_energy( int(kEV*e));
      par->add_mass( int(kEV*mass));
      par->add_barcode(0);
      par->add_daughter1( 0  );
      par->add_daughter2( 0  );
      par->add_mother1(0  );
      par->add_mother2(0  );
      par->add_x( int(uL*x ));
      par->add_y( int(uL*y ));
      par->add_z( int(uL*z ));
      par->add_t( int(uL*t ));

      pz=-1*reader.heprup.EBMUP.second;
      e=reader.heprup.EBMUP.second;  
      par->add_id( 1  );
      par->add_pdg_id(  reader.heprup.IDBMUP.second );
      par->add_status( 11 );
      par->add_px( int(kEV*px));
      par->add_py( int(kEV*py));
      par->add_pz( int(kEV*pz));
      par->add_energy( int(kEV*e));
      par->add_mass( int(kEV*mass));
      par->add_barcode(0);
      par->add_daughter1( 0  );
      par->add_daughter2( 0  );
      par->add_mother1(0  );
      par->add_mother2(0  );
      par->add_x( int(uL*x ));
      par->add_y( int(uL*y ));
      par->add_z( int(uL*z ));
      par->add_t( int(uL*t ));
 
       for (int k = 0; k < reader.hepeup.NUP; k++) {
            int idhep=reader.hepeup.IDUP[k]; 
            int isthep=reader.hepeup.ISTUP[k]; 
            int jmo1=reader.hepeup.MOTHUP[k].first;
            int jmo2=reader.hepeup.MOTHUP[k].second;
            int jda1=0;
            int jda2=0;
            double px=reader.hepeup.PUP[k][0];
            double py=reader.hepeup.PUP[k][1];
            double pz=reader.hepeup.PUP[k][2];
            double e=reader.hepeup.PUP[k][3];
            double mass=reader.hepeup.PUP[k][4];
            double x=0;
            double y=0;
            double z=0;
            double t=reader.hepeup.VTIMUP[k];

      par->add_id( k+2  );
      par->add_pdg_id(  idhep );
      par->add_status( isthep );
      par->add_px( int(kEV*px));
      par->add_py( int(kEV*py));
      par->add_pz( int(kEV*pz));
      par->add_energy( int(kEV*e));
      par->add_mass( int(kEV*mass));
      par->add_barcode(0);
      par->add_daughter1( jda1  );
      par->add_daughter2( jda2  );
      par->add_mother1( jmo1  );
      par->add_mother2( jmo2  );

      // vertex
      par->add_x( int(uL*x ));
      par->add_y( int(uL*y ));
      par->add_z( int(uL*z ));
      par->add_t( int(uL*t ));

       } // end loop over particles 


      epbook->write(promc); // write ProMC event
      if (i%100 ==0) cout << "# event=" << i << endl;


      i++;
    }
    std::cout << i << " events read" << std::endl;


  // save post-generation statistics for ProMC
   ProMCStat stat;
  // stat.set_cross_section_accumulated( sigmapb ); // in pb 
  if (valueCrossX>-1 && valueCrossXerror>-1) {
    stat.set_cross_section_accumulated( valueCrossX);
    stat.set_cross_section_error_accumulated(valueCrossXerror);
  }
   stat.set_luminosity_accumulated(  i/valueCrossX );
   stat.set_ntried(i);
   stat.set_nselected(i);
   stat.set_naccepted(i);
   epbook->setStatistics(stat);
 
   epbook->close(); // close

  return EXIT_SUCCESS;
}
