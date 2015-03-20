// this :
#include <CBook/H1D.h>

#include <HCL/Histogram.h>


extern std::vector< H1D* > H1;






H1D::H1D(const char *name, int bin, double d1,double d2):
                       Histogram1D(name,bin,d1,d2) {

        H1.push_back(this);
       // cout << "Constructor: " << endl;
      };




H1D::H1D(const char *name, const std::vector<double> d):
         Histogram1D(name, d) {
         H1.push_back(this);
      };


H1D::~H1D() { 
}; 



