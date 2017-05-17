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
#include"SimpleRNG.h" // random numbers
#include"ProMCHeader.pb.h"
#include"ProMC.pb.h"
#include"ProMCBook.h"

using namespace std;
using namespace promc; 

// Generate a random number between 0 and 1
// return a uniform number in [0,1].
double unifRand() { 
    return rand() / double(RAND_MAX);
}



int main(int argc, char **argv) { 

  int Ntot=7000;               // 100 events 
  double AvParticles=50;     // everange Nr of particles per event 

 // srand (time(NULL));

 cout << "-> Pseudo events to be generated=" << Ntot << endl;
 cout << "-> Average Nr of pseudo particles per event =" << AvParticles << endl;
 cout << "-> Exponential random  parametrization\n" << endl;

  // use 0.01*MeV units
  const double kEV=1000*100;

 // this is length unit. multiply CM by 1000
 const double uL=1000.;

  bool isMemory=false;
  ProMCBook *epbook = new ProMCBook("out/output.promc","w",isMemory);
  epbook->setDescription(Ntot, "ProMC file to keep MC data");

  ProMCHeader header;
  // set cross section
  header.set_cross_section(10000);

  // set ID for beams 
  header.set_id1(2112);
  header.set_id2(-2112); 

  // set units in 0.01*MeV for pX,pY,pZ 
  // in reader, we will read this back
  header.set_momentumunit((int)kEV);

  // set length unit. In Pythia, we multiply CM by 1000 
  header.set_lengthunit((int)uL);


  // let's store a map with most common masses:
  ProMCHeader_ParticleData* pi= header.add_particledata();
  pi->set_id(211); // pi+ 
  pi->set_mass(0.13957018);
  pi->set_name("pi+");



  // write the header.
  epbook->setHeader(header); // write header 
 
  // output TXT file for comparions
  ofstream myfile;
  myfile.open ("out/output.txt");


  // random numbers for test
  SimpleRNG* ran = new SimpleRNG(); 
  ran->SetState(100,100);

cout << "Start writing data .." << endl;
for (int i=0; i<Ntot; i++) { 

               ProMCEvent promc;

              // fill event information
              ProMCEvent_Event  *eve= promc.mutable_event();
              eve->set_number(i);
              eve->set_process_id(10); // 10 is just a dummy number 

             // -- do you also need to fill some arrays for this event with weights etc? You can do this too -- 
             // add double values 
             eve->add_fdata((double)0.091);
             eve->add_fdata((double)0.092);
             // .. add as many as you like

             // add integer values 
              eve->add_idata(0);
              eve->add_idata(2);
             //.. add as many as you like


               // fill truth information 
               ProMCEvent_Particles  *pa= promc.mutable_particles();

                // start real loop over particles
                int Nparticles=ran->GetPoisson(AvParticles);

                   /*                 
                   if (i==0) {  // this just a check for one event
                   // here is a test for ranges 
                   promc.add_pdg_id(10);
                   promc.add_status(-10);
                   // store kEV type particle
                   int x=(int)(0.001*0.01*kEV); 
                   promc.add_px( x );
                   promc.add_py( x );
                   promc.add_pz( x );
                   promc.add_mass( x );
                   myfile << "0.01 MeV particle " << " " << 10 << " " << x << " " << x << " " << x << " " << x << endl;

                   // store MeV-type particle 
                   x=(int)(0.001*kEV); 
                   promc.add_pdg_id(20);
                   promc.add_status(-20);
                   promc.add_px( x );
                   promc.add_py( x );
                   promc.add_pz( x );
                   promc.add_mass( x );
                   myfile << " 1 MeV particle = " << " " << 20 << " " << x << " " << x << " " << x << " " << x << endl;

                   // store GeV-type particle
                   x=(int)(1*kEV); 
                   promc.add_pdg_id(30);
                   promc.add_status(-30);
                   promc.add_px( x );
                   promc.add_py( x );
                   promc.add_pz( x );
                   promc.add_mass( x );
                   myfile << " 1 GeV particle " << " "  << 30 << " " << x << " " << x << " " << x << " " << x << endl;

                   // stote TeV-type particle
                   x=(int)(1000*kEV);
                   promc.add_pdg_id(40);
                   promc.add_status(-40);
                   promc.add_px( x );
                   promc.add_py( x );
                   promc.add_pz( x );
                   promc.add_mass( x );
                   myfile << " 1 TeV particle " << " " << 40 << " " << x << " " << x << " " << x << " " << x << endl;


                   // store 20 TeV particle          
                   x=(int)(20*1000*kEV); 
                   promc.add_pdg_id(50);
                   promc.add_status(-50);
                   promc.add_px( x );
                   promc.add_py( x );
                   promc.add_pz( x );
                   promc.add_mass( x );
                   myfile << "20 TeV particle=" << " " << 50 << " " << x << " " << x << " " << x << " " << x << endl;
 

                  } // end of one event check
                  */


                myfile << " " << endl; 
                for (int j=0; j<Nparticles; j++) { 

                   int  id = (int)ran->GetNormal(0.0,500.0);
                   int  status = (int)ran->GetNormal(0.0,500.0);
                   int  px=(int)(ran->GetExponential(0.5)*kEV);
                   int  py=-1*(int)(ran->GetExponential(0.5)*kEV);
                   int  pz=(int)(ran->GetExponential(2)*kEV);
                   int  m=(int)(ran->GetExponential(0.2)*kEV); // cannot be negative 

                   // store particles
                   pa->add_pdg_id(id); 
                   pa->add_status(status);
                   pa->add_px( px ); 
                   pa->add_py( py ); 
                   pa->add_pz( pz ); 
                   pa->add_mass(m); 
                   pa->add_barcode(id);
                   pa->add_mother1( id*10  );
                   pa->add_mother2( id*10  );
                   pa->add_daughter1( id  );
                   pa->add_daughter2(  id   );


                   myfile << id << " " << status << " " << px << " " << py << " " << pz << " " << m << " " << id*10 << " " << id*100 << " " << id << " " << id <<  " " << id << endl; 
                 }

                 // store vertex info
                 for (int j=0; j<Nparticles; j++) {

                   int  id=(int)(ran->GetNormal(0.0,10.0));
                   int  x=(int)(ran->GetNormal(0.0,10.0) );
                   int  y=-1*(int)(ran->GetNormal(0.0,10.0) );
                   int  z=(int)(ran->GetNormal(0.0,10.0));
                   int  t=(int)( ran->GetNormal(0.0,10.0));

                   pa->add_id(id); 
                   pa->add_x((int)(x*uL));
                   pa->add_y((int)(y*uL));
                   pa->add_z((int)(z*uL));
                   pa->add_t((int)(t*uL));


                   myfile << id << " " << x << " " << y << " " << z << " " << t << endl;
                 }



                if (i==0) { // just one event for checking 
                   fstream output("out/event.proto", ios::out | ios::binary | ios::trunc);
                   promc.SerializeToOstream(&output);
                   output.close();
                }


                if (i % 100==0) cout << " event=" << i << "  -> Nparticles="<< Nparticles << endl;


                bool isGood=epbook->write(promc); // write event

}




  // at the end of the record, write accumulated statistics
  cout << "Write accumulated statistics to the ProMC file " << endl;
  ProMCStat stat;
  stat.set_cross_section_accumulated(10000);
  stat.set_cross_section_error_accumulated(10);
  stat.set_luminosity_accumulated(1000);
  epbook->setStatistics(stat); // write header 


   // close the file
   myfile.close();  



   cout << "\nFile out/output.promc is done for all events" << endl;
   cout << "File out/event.proto   is done for 1 event" << endl;
   cout << "File out/output.txt   is done (text)" << endl;


    epbook->close(); // close


    return 0;
}
