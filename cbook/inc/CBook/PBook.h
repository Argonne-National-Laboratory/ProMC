// * This code is licensed under:
// * JHPlot License, Version 1.0
// * - for license details see http://hepforge.cedar.ac.uk/jhepwork/ 
// *
// * Copyright (c) 2005 by S.Chekanov (chekanov@mail.desy.de). 
// * All rights reserved.



#ifndef PBook_H 
#define PBook_H
#include <HCL/Histogram.h>

class PBook{
public:
     static PBook* open(const char* filename);
     unsigned int  sizeH1D();
     unsigned int  sizeH2D();
     unsigned int  size(); // all size
     void  close();
     void  write(); // write histogram if any
     void  write(const char* name, std::vector< std::vector<int> >);
     void  write(const char* name, std::vector< std::vector<double> >);
     void  write(const char* name, std::vector<int>);
     void  write(const char* name, std::vector<double>);
     void  write(const char* name, std::vector<double>,std::vector<double>);
     void  write(const char* name, std::vector<double>,std::vector<double>,std::vector<double>);
     void  print();
     void  clear();
     void printH1D(unsigned int i);
     void printH2D(unsigned int i);
     void print(
                   HCL::Base1D& aHistogram);
     void print(
                   HCL::Base2D& aHistogram);

protected:
    PBook();                   // constructor
private:
    static PBook* _singleton;
};

#endif
