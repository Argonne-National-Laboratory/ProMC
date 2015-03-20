//==========================================================================//
//                                 promc_split.cxx                          //
//    Kyle Strand - kstrand12@winona.edu                                    //   
//    8 July 2013                                                           //
//                                                                          //
//    This file will read a ProMC file and split it into multiple files     //
//                                                                          //
//==========================================================================//

#include<iostream>
#include<fstream>
#include<stdlib.h>
#include"ProMCHeader.pb.h"
#include"ProMC.pb.h"
#include"ProMCBook.h"


/////////////////////////////////
//     Function definitions    //
/////////////////////////////////

// Input error function
void error() {
   cerr << endl << "Command structure: \"promc_split <input ProMC file> <Number of divisions>\"" << endl << endl;
   cerr << "Use \"promc_split --help\"" << endl << endl << "or" << endl << endl;
   cerr << "\"promc_split -h\" for more information" << endl << endl;
   exit(1);
}

// Help function
void help() {
   cout << endl <<  "promc_split: Reads a ProMC file and splits it into multiple ProMC files" << endl << endl;
   cout << "Command structure: \"promc_split <input ProMC file> <number of divisions>\"" << endl << endl;
   cout << "<Number of divisions> will be even divisons. If uneven divisions, extra";
   cout << " events will be placed in the last file." << endl << endl;
   exit(0);
}

// Statistics Function
void stats( ProMCBook *old, ProMCBook *new1[], int num) {
   ProMCStat stat = old->getStatistics();
   for (int i = 0; i<num; i++) new1[i]->setStatistics(stat);
}

// Close ProMCBook Function
void closer( ProMCBook *old, ProMCBook *new1[], int num) {
   for (int i = 0; i<num; i++)  new1[i]->close();
   old->close();
}

// Main function
int main(int argc, char **argv) {

   // Command line arguments
   for (int i = 1; i < argc; i++) {
      const string arg = argv[i];
      if (arg == "--help" || arg == "-h") {
         help();
      }
   }

   // Select input file and divisions from command line
   string infile("-");
   int div;
   if (argc == 3) {
      infile = argv[1];
      div = atoi(argv[2]);
   } else if (argc !=3) {
      error();
   }

   // Reading ProMC file	
   ProMCBook *epbook = new ProMCBook(infile.c_str(),"r");
   // get ProMC version
   int h = epbook->getVersion();
   cout << "ProMC Version = " << h << endl;
   // get description of file
   string d = epbook->getDescription();
   cout << "Description: " << d << endl;
   int nev = epbook->getEvents();
   cout << "Number of events: " << nev << endl;
   // get the header information
   ProMCHeader header = epbook->getHeader();


   int divevent = nev/div;                // number of events per division
   int divremain = nev % div;             // remainder of events that don't fit into clean divisions
   int divtotal = divremain + divevent;   // for adding the remainder into the final file
   string split[div];                     // string array for new files
   string name;                           // the final name for the new files
   ProMCBook *epbook1[div];               // object constructor for new promc files

   //create new files and set header and description
   for (int i = 0; i<div; i++) {
      split[i] = "out/split";
      std::stringstream str;
      str << i;
      split[i].append("0");
      split[i].append(str.str());
      split[i].append(".promc");
      name = split[i];
      epbook1[i] = new ProMCBook(name.c_str(),"w");     // writing new file
      if (i< ( div - 1 ) ) {
         epbook1[i]->setDescription(divtotal, d);       // set description for final file
      }
      epbook1[i]->setDescription(divevent, d);          // set description for new files
      epbook1[i]->setHeader(header);                    // set header for new files
   }

   // gather info from each event
   for (int i = 0; i<div; i++) {                        // looping for correct file

      if (i < ( div - 1 ) ) {                           // all files except final
         for (int j = 0; j < divevent; j++) {           // looping each event
            if (epbook->next() != 0) continue;
         
            ProMCEvent eve = epbook->get();             // getting event info
            epbook1[i]->write(eve);                     // writing event to correct file
         }
      }  else if (i == (div - 1) ){                     // determining final file
         for (int j = 0; j < divtotal; j++) {           // looping each event
            if (epbook->next() != 0) continue;

            ProMCEvent eve = epbook->get();             // getting event info
            epbook1[i]->write(eve);                     // writing event info to final file
         }
      } 
   }
 
   stats(epbook, epbook1, div);                        
   closer(epbook, epbook1, div);
   return 0;

}
