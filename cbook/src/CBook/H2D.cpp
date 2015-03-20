// this :
#include <CBook/H2D.h>
#include <HCL/Histogram.h>

extern std::vector< H2D* > H2;



// constractor
H2D::H2D(const char *name, int binx, double d1X,double d2X,
                           int biny, double d1Y,double d2Y):
                       Histogram2D(name,binx,d1X,d2X,biny,d1Y,d2Y) {

        H2.push_back(this);
      };


// constractor with vectors
H2D::H2D(     const char *name,
              std::vector<double> v1,
              std::vector<double> v2): 
              Histogram2D(name,v1,v2) { 

        
        H2.push_back(this);
      };




H2D::~H2D() { 
}; 


