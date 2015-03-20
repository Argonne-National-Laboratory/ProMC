// * This code is licensed under:
// * JHPlot License, Version 1.0
// * - for license details see http://hepforge.cedar.ac.uk/jhepwork/ 
// *
// * Copyright (c) 2005 by S.Chekanov (chekanov@mail.desy.de). 
// * All rights reserved.



#include "CBook/EPBook.h"


#define EPBOOK_VERSION 1;

#include <vector>
int nev; 
zipios::ZipOutputStream *outzip;
EPBook* EPBook::_singleton = 0;

EPBook::EPBook()
{ }

EPBook* EPBook::open(const char* filename)
{
   if (_singleton == 0)
   {

       GOOGLE_PROTOBUF_VERIFY_VERSION;
       nev=1;
       outzip = new zipios::ZipOutputStream(filename);
       outzip->putNextEntry( "info" ) ;
       std::string s;
       std::stringstream out;
       out << EPBOOK_VERSION;
       (*outzip) << out.str();



      _singleton = new EPBook;

   }      // end if
   return _singleton;
}    



// get size of H1 histogram
unsigned int  EPBook::size()
{
    return  nev;
}



// print all
void EPBook::clear()
{

}


// write X and Y with errors 
void EPBook::write(promc::HEvent eve ) { 

         std::string out;
         if (!eve.SerializeToString(&out)) {
            cerr << "Failed to write event" << endl;
         }

          std::stringstream sout;
          sout << nev;
          outzip->putNextEntry( sout.str() ) ;
          (*outzip) << out;
          nev++;
          eve.Clear();
}



// close
void EPBook::close()

{
    
    outzip->close();
    clear();
}




