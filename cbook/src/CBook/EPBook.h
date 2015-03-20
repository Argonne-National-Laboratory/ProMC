// * This code is licensed under:
// * JHPlot License, Version 1.0
// * - for license details see http://hepforge.cedar.ac.uk/jhepwork/ 
// *
// * Copyright (c) 2005 by S.Chekanov (chekanov@mail.desy.de). 
// * All rights reserved.



#ifndef EPBook_H 
#define EPBook_H

using namespace std;

#include <iostream>
#include <fstream>
#include <iomanip>
using std::ofstream;
using std::ifstream;
using std::setw;

#include "zipios++/zipios-config.h"
#include "zipios++/meta-iostreams.h"
#include "zipios++/zipoutputstream.h"
#include "zipios++/zipfile.h"
#include "zipios++/zipinputstream.h"

#include "CBook/HEvent.pb.h"


class EPBook{
public:
     static EPBook* open(const char* filename);
     void  close();
     void  write(promc::HEvent e ); // write histogram if any
     void  clear();
    unsigned int  size(); // all size
protected:
     EPBook();                   // constructor
private:
    static EPBook* _singleton;
};

#endif
