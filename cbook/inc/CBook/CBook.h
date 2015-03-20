/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
**  A library for HEP events storage and processing based on Google's ProtocolBuffers 
**
** Author: S.Chekanov (ANL). chekanov@anl.gov
** Copyright  2012 
** -------------------------------------------------------------------------*/


#ifndef CBook_H 
#define CBook_H
#include <HCL/Histogram.h>

class CBook{
public:
      
     static CBook* open(const char* filename);
     unsigned int  sizeH1D();
     unsigned int  sizeH2D();
     unsigned int  size(); // all size
     void  write();
     void  print();
     void  clear();
     void  close();
     void printH1D(unsigned int i);
     void printH2D(unsigned int i);
     void print(
                   HCL::Base1D& aHistogram);
     void print(
                   HCL::Base2D& aHistogram);

protected:
    CBook();                   // constructor
private:
    static CBook* _singleton;
};

#endif
