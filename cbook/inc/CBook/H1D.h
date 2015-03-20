#ifndef H1D_h
#define H1D_h
#include <iostream>
#include <vector>
#include <HCL/Histogram.h>


using namespace std;

class H1D: public HCL::Histogram1D {


 public:


     H1D(const char *name, int bin, double d1,double d2);
     H1D(const char *name, const std::vector<double> d);

     virtual ~H1D();


};


#endif




