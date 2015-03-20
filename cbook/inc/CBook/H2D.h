#ifndef H2D_h
#define H2D_h
#include <iostream>
#include <vector>
#include <HCL/Histogram.h>
using namespace std;

class H2D: public HCL::Histogram2D {


 public:


    // the standard constructor 
    H2D(const char *name, int binx, double d1X,double d2X,
                          int biny, double d1Y,double d2Y);


    // constract using vectors
    H2D(      const char *name,
              std::vector<double> v1,
              std::vector<double> v2);



    virtual ~H2D();

};


#endif




