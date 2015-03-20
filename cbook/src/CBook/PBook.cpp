// * This code is licensed under:
// * JHPlot License, Version 1.0
// * - for license details see http://hepforge.cedar.ac.uk/jhepwork/ 
// *
// * Copyright (c) 2005 by S.Chekanov (chekanov@mail.desy.de). 
// * All rights reserved.



using namespace std;

#include <iostream>
#include "CBook/PBook.h"
#include "CBook/H1D.h"
#include "CBook/H2D.h"
#include "HCL/Axis.h"
#include "HCL/Histogram.h"
#include <fstream>
#include <iomanip>
using std::ofstream;
using std::ifstream;
using std::setw;

#define IOS_IN std::ios::in
#define IOS_OUT std::ios::out
#define STREAM_OPEN(s) (s.is_open())

#include "zipios++/zipios-config.h"
#include "zipios++/meta-iostreams.h"
#include "zipios++/zipoutputstream.h"
#include "zipios++/zipfile.h"
#include "zipios++/zipinputstream.h"
#include "HRecord.pb.h"

#define VERSION_PFILE 1;

#include <vector>
std::vector< H1D* > H1;
std::vector< H2D* > H2;
promc::Record record;
int nev; 
zipios::ZipOutputStream *outzip;

PBook* PBook::_singleton = 0;

PBook::PBook()
{ }

PBook* PBook::open(const char* filename)
{
   if (_singleton == 0)
   {

       GOOGLE_PROTOBUF_VERIFY_VERSION;
       nev=1;
       outzip = new zipios::ZipOutputStream(filename);
       outzip->putNextEntry( "info" ) ;
       std::string s;
       std::stringstream out;
       out << VERSION_PFILE;
       (*outzip) << out.str();



      _singleton = new PBook;

   }      // end if
   return _singleton;
}    // end Instance()


// get size of H2 histogram
unsigned int  PBook::sizeH1D()
{
    return  H1.size();
}

// get size of H1 histogram
unsigned int  PBook::sizeH2D()
{
    return  H2.size();
}

// get size of H1 histogram
unsigned int  PBook::size()
{
    return  H1.size()+H2.size();
}


// get size of H1 histogram
void PBook::printH1D(unsigned int h)
{
    if (h<0 || h> H1.size()) {
     std::cout << "WRONG HISTOGRAM ID " <<std::endl;
     return;
    } 
    H1D* h1 = H1[h];
    print(*h1);
}


// get size of H1 histogram
void  PBook::printH2D(unsigned int h)
{
    if (h<0 || h> H2.size()) {
     std::cout << "WRONG HISTOGRAM ID " <<std::endl;
     return;
    }
    HCL::Histogram2D* h2 = H2[h];
    print(*h2);
}


// print all
void PBook::print()
{

        if (H1.size()>0) {
        std::cout << "\nPrint H1D histograms"<<std::endl;
        for(unsigned int i = 0; i < H1.size(); i++ ) {
         H1D* h1 = H1[i];
         print(*h1);
         }
        }


        if (H2.size()>0) {
        std::cout << "\nPrint H2D histograms"<<std::endl;
        for(unsigned int i = 0; i < H2.size(); i++ ) {
         // HCL::Histogram2D* h2 = H2[i];
         H2D* h2 = H2[i];
         print(*h2);
         }
        }

        if (H1.size()==0 && H2.size()==0) std::cout << "Histograms were not booked"<<std::endl;
}


// print all
void PBook::clear()
{


         if (H1.size()==0 && H2.size()==0) {
                  std::cout << "PBook: Nothing to clear"<<std::endl;
                  return;
         }

 
        if (H1.size()>0) {
        for(unsigned int i = 0; i < H1.size(); i++ ) {
         H1D* h1 = H1[i];
         delete h1; 
         }
        }


        if (H2.size()>0) {
        for(unsigned int i = 0; i < H2.size(); i++ ) {
         H2D* h2 = H2[i];
         delete h2; 
         }
        }

       H1.clear();
       H2.clear();
       //std::cout << "PBook: histograms were removed"<<std::endl; 
}



// write X and Y with errors 
void PBook::write(const char* name, std::vector<double> x, std::vector<double> y, 
            std::vector<double> err)
{

        promc::Record::P1D* p =  record.mutable_p1d();
        p->set_name( name );
        if (x.size() != y.size()) {
            cerr << "Sizes of X and Y vectors are different" << endl;
         }

        for(unsigned int ii=0; ii < x.size(); ii++){
                 p->add_x((double)x[ii]);
                 p->add_y((double)y[ii]);
                 p->add_yupper((double)err[ii]); 
                 p->add_ylower((double)err[ii]); 
         }
         
         std::string out;
         if (!record.SerializeToString(&out)) {
            cerr << "Failed to write P1D object" << endl;
         }
 
          std::stringstream sout;
          sout << nev; 
          outzip->putNextEntry( sout.str() ) ;
          (*outzip) << out;
          nev++;
          record.Clear();

}




// write X-Y data
void PBook::write(const char* name, std::vector<double> x, std::vector<double> y)
{

        promc::Record::PXY* p =  record.mutable_pxy();
        p->set_name( name );
        if (x.size() != y.size()) {
            cerr << "Sizes of X and Y vectors are different" << endl;
         }

        for(unsigned int ii=0; ii < x.size(); ii++){
                 p->add_x((int)x[ii]);
                 p->add_y((int)y[ii]);
         }
         
         std::string out;
         if (!record.SerializeToString(&out)) {
            cerr << "Failed to write PXY object" << endl;
         }
 
          std::stringstream sout;
          sout << nev; 
          outzip->putNextEntry( sout.str() ) ;
          (*outzip) << out;
          nev++;
          record.Clear();

}


// write vectors 
void PBook::write(const char* name, std::vector<int> vec)
{

        promc::Record::P0I* p =  record.mutable_p0i();
        p->set_name( name );
        for(unsigned int ii=0; ii < vec.size(); ii++)
                 p->add_value((int)vec[ii]);
         
         std::string out;
         if (!record.SerializeToString(&out)) {
            cerr << "Failed to write P0I object" << endl;
         }
 
          std::stringstream sout;
          sout << nev; 
          outzip->putNextEntry( sout.str() ) ;
          (*outzip) << out;
          nev++;
          record.Clear();

}




// write vectors multidimensional values
void PBook::write(const char* name, std::vector< std::vector<int> > vec)
{

        promc::Record::PNI* p =  record.mutable_pni();
        p->set_name( name );
        for(unsigned int i=0; i < vec.size(); i++) {
                 std::vector<int> vv=vec[i];
                 if (i==0) p->set_dimension( vv.size());
                 for(unsigned int j=0; j < vv.size(); j++) {
                   p->add_value((int)vv[j]);
                  }
         }
   
         
         std::string out;
         if (!record.SerializeToString(&out)) {
            cerr << "Failed to write PNI object" << endl;
         }
 
          std::stringstream sout;
          sout << nev; 
          outzip->putNextEntry( sout.str() ) ;
          (*outzip) << out;
          nev++;
          record.Clear();

}


// write vectors multidimensional values
void PBook::write(const char* name, std::vector< std::vector<double> > vec)
{

        promc::Record::PND* p =  record.mutable_pnd();
        p->set_name( name );
        for(unsigned int i=0; i < vec.size(); i++) {
                 std::vector<double> vv=vec[i];
                 if (i==0) p->set_dimension( (int)(vv.size()) );
                 for(unsigned int j=0; j < vv.size(); j++) {
                   p->add_value((double)vv[j]);
                  }
         }
   
         
         std::string out;
         if (!record.SerializeToString(&out)) {
            cerr << "Failed to write PND object" << endl;
         }
 
          std::stringstream sout;
          sout << nev; 
          outzip->putNextEntry( sout.str() ) ;
          (*outzip) << out;
          nev++;
          record.Clear();

}











void PBook::write(const char* name, std::vector<double> vec)
{
        promc::Record::P0D* p =  record.mutable_p0d();
        p->set_name( name );
        for(unsigned int ii=0; ii < vec.size(); ii++)
         p->add_value((double)vec[ii]);
         
       
         std::string out;
         if (!record.SerializeToString(&out)) {
            cerr << "Failed to write P0D object" << endl;
         }
 
          std::stringstream sout;
          sout << nev; 
          outzip->putNextEntry( sout.str() ) ;
          (*outzip) << out;
          nev++;
          record.Clear();
}




// write to ProtoBuf  file
void PBook::write()
{

        for(unsigned int i = 0; i < H1.size(); i++ ) {
         H1D* h1 = H1[i];

        promc::Record::H1D* h =  record.mutable_h1d();
        h->set_name( h1->title() );

        h->set_allentries(h1->allEntries());
        h->set_isfixedbins(false); // treat all as with non-fixed bins 
        h->set_bins(h1->axis().bins() ); 
        h->set_min( h1->axis().lowerEdge() ); 
        h->set_max( h1->axis().upperEdge() ); 
        double under=h1->binHeight(HCL::Axis::UNDERFLOW_BIN);
        double over=h1->binHeight(HCL::Axis::OVERFLOW_BIN);
        h->set_underflow(under);
        h->set_overflow(over);
        h->set_nentries(h1->allEntries()-under-over);
        h->set_rms( h1->rms() );
        h->set_mean( h1->mean() );

        for(int j = 0; j<h1->axis().bins() ; j++ ) {
              double x1=h1->axis().binLowerEdge(j); 
              //double x2=h1->axis().binUpperEdge(j);  
              h->add_edges(x1);
              h->add_entries(h1->binEntries(j) );
              h->add_errors(h1->binError(j) );
              h->add_heights(h1->binHeight(j));
              h->add_rmses(h1->binStdDev(j));
              h->add_means(h1->binMean(j)); 
        }

              int m=h1->axis().bins()-1;
              h->add_edges(h1->axis().binUpperEdge(m));
              h->add_entries(over);
              h->add_errors(0);
              h->add_heights(over);
              h->add_rmses(0);
              h->add_means(0); 


        std::string out1;
        if (!record.SerializeToString(&out1)) {
           cerr << "Failed to write H1D object" << endl;
        }
 
          std::stringstream sout1;
          sout1 << nev; 
          outzip->putNextEntry( sout1.str() ) ;
          (*outzip) << out1;
          nev++;

          record.Clear();

        }



        for(unsigned int i = 0; i < H2.size(); i++ ) {
         H2D* h2 = H2[i];

      //  cout << "DEBUG H2D=" << h2->title() << endl;

        promc::Record::H2D* h =  record.mutable_h2d();
        h->set_name( h2->title() );
        h->set_allentries(h2->allEntries());
        h->set_isfixedbins(false);
        unsigned  int xbins = h2->xAxis().bins();
        unsigned  int ybins = h2->yAxis().bins();
      
        h->add_summary(xbins);
        h->add_summary(h2->xAxis().lowerEdge());
        h->add_summary(h2->xAxis().upperEdge());
        h->add_summary(ybins);
        h->add_summary(h2->yAxis().lowerEdge());
        h->add_summary(h2->yAxis().upperEdge());

        h->add_summary(h2->meanX());
        h->add_summary(h2->rmsX());

        h->add_summary(h2->meanY());
        h->add_summary(h2->rmsY());



        
  // out of range
  //   6 | 7 | 8
  //  -----------
  //   3 | 4 | 5
  //  -----------
  //   0 | 1 | 2

  double height[9];

  height[0] = h2->binHeight(HCL::Axis::UNDERFLOW_BIN,
                                        HCL::Axis::UNDERFLOW_BIN);
  height[2] = h2->binHeight(HCL::Axis::OVERFLOW_BIN,
                                         HCL::Axis::UNDERFLOW_BIN);
  height[6] = h2->binHeight(HCL::Axis::UNDERFLOW_BIN,
                                         HCL::Axis::OVERFLOW_BIN);
  height[8] = h2->binHeight(HCL::Axis::OVERFLOW_BIN,
                                         HCL::Axis::OVERFLOW_BIN);

  h->set_nentries(h2->allEntries()-height[0]-height[2]-height[6]-height[8] );
/*
  unsigned int i,j;

  height[1] = 0;
  height[7] = 0;
  for(i=0;i<xbins;i++){
    height[1] += h1->binHeight(i,HCL::Axis::UNDERFLOW_BIN);
    height[7] += h1->binHeight(i,HCL::Axis::OVERFLOW_BIN);
  }

  height[3] = 0;
  height[5] = 0;
  for(j=0;j<ybins;j++){
    height[3] += h1->binHeight(HCL::Axis::UNDERFLOW_BIN,j);
    height[5] += h1->binHeight(HCL::Axis::OVERFLOW_BIN,j);
  }

  height[4] = 0;
  for(i=0;i<xbins;i++){
    for(j=0;j<ybins;j++){
      height[4] += h1->binHeight(i,j);
    }
  }
*/

        h->add_outofrange(  height[0] );
        h->add_outofrange(  height[0]);
        h->add_outofrange(  height[6] );
        h->add_outofrange(  height[2]);
        h->add_outofrange(  height[0] );
        h->add_outofrange(  height[8]);
        

         int ii=0;
         for(unsigned int j1 = 0; j1<xbins+1; j1++ ) {
          for(unsigned int j2 = 0; j2<ybins+1; j2++ ) { 
          
            if (j1<xbins && j2<ybins) {
              h->add_heights(h2->binHeight(j1,j2) );
              h->add_errors(h2->binError(j1,j2) );
              h->add_entries(h2->binEntries(j1,j2) );
              h->add_meansx(h2->binMeanX(j1,j2));
              h->add_rmsesx(h2->binStdDevX(j1,j2));
              h->add_meansy(h2->binMeanY(j1, j2));
              h->add_rmsesy(h2->binStdDevY(j1, j2));
             } else {
              h->add_heights(0 );
              h->add_errors(0);
              h->add_entries(0);
              h->add_meansx(0);
              h->add_rmsesx(0);
              h->add_meansy(0);
              h->add_rmsesy(0);
             }

              ii++;
       }  }


        std::string out2;
        if (!record.SerializeToString(&out2)) {
           cerr << "Failed to write H2D object" << endl;
        }


        std::stringstream sout2;
        sout2 << nev;
        outzip->putNextEntry( sout2.str() ) ;
        (*outzip) << out2;
        record.Clear();
        nev++;
        }



}




// close
void PBook::close()

{

    
    outzip->close();
    clear();
}





//////////////////////////////////////////////////////////////////////////////
void PBook::print(
     HCL::Base1D& aHistogram
)
//////////////////////////////////////////////////////////////////////////////
// A la HPRINT.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::cout << aHistogram.dimension() << aHistogram.title() << std::endl;
  std::cout
    << " * ENTRIES = " << aHistogram.allEntries()
    << " * ALL CHANNELS = " << aHistogram.sumBinHeights()
    << " * UNDERFLOW = " << aHistogram.binHeight(HCL::Axis::UNDERFLOW_BIN)
    << " * OVERFLOW = " << aHistogram.binHeight(HCL::Axis::OVERFLOW_BIN)
    << std::endl;
  std::cout
    << " * BIN WID = " << aHistogram.axis().binWidth(0)
    << " * MEAN VALUE = " << aHistogram.mean()
    << " * R . M . S = " << aHistogram.rms()
    << std::endl;

  // Some bins :
  int bins = aHistogram.axis().bins();
  std::cout
    << " * ENTRIES[0]   = "
    << aHistogram.binEntries(0)
    << " * HEIGHT[0] = "
    << aHistogram.binHeight(0)
    << " * ERROR[0] = "
    << aHistogram.binError(0)
    << std::endl;
  std::cout
    << " * ENTRIES[N/2] = "
    << aHistogram.binEntries(bins/2)
    << " * HEIGHT[N/2] = "
    << aHistogram.binHeight(bins/2)
    << " * ERROR[N/2] = "
    << aHistogram.binError(bins/2)
    << std::endl;
  std::cout
    << " * ENTRIES[N-1] = "
    << aHistogram.binEntries(bins-1)
    << " * HEIGHT[N-1] = "
    << aHistogram.binHeight(bins-1)
    << " * ERROR[N-1] = "
    << aHistogram.binError(bins-1)
    << std::endl;
}


/////////////////////////////////////////////////////////////////////////////
void PBook::print(
   HCL::Base2D& aHistogram
)

//////////////////////////////////////////////////////////////////////////////
// A la HPRINT.
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
{
  std::cout << aHistogram.dimension() << aHistogram.title() << std::endl;
  std::cout
    << " * ENTRIES = " << aHistogram.allEntries() << std::endl;

  //   6 | 7 | 8
  //  -----------
  //   3 | 4 | 5
  //  -----------
  //   0 | 1 | 2

  double height_0 = aHistogram.binHeight(HCL::Axis::UNDERFLOW_BIN,
                                         HCL::Axis::UNDERFLOW_BIN);
  double height_2 = aHistogram.binHeight(HCL::Axis::OVERFLOW_BIN,
                                         HCL::Axis::UNDERFLOW_BIN);
  double height_6 = aHistogram.binHeight(HCL::Axis::UNDERFLOW_BIN,
                                         HCL::Axis::OVERFLOW_BIN);
  double height_8 = aHistogram.binHeight(HCL::Axis::OVERFLOW_BIN,
                                         HCL::Axis::OVERFLOW_BIN);

  int i,j;

  double height_1 = 0;
  double height_7 = 0;
  for(i=0;i<aHistogram.xAxis().bins();i++){
    height_1 += aHistogram.binHeight(i,HCL::Axis::UNDERFLOW_BIN);
    height_7 += aHistogram.binHeight(i,HCL::Axis::OVERFLOW_BIN);
  }

  double height_3 = 0;
  double height_5 = 0;
  for(j=0;j<aHistogram.yAxis().bins();j++){
    height_3 += aHistogram.binHeight(HCL::Axis::UNDERFLOW_BIN,j);
    height_5 += aHistogram.binHeight(HCL::Axis::OVERFLOW_BIN,j);
  }

  double height_4 = 0;
  for(i=0;i<aHistogram.xAxis().bins();i++){
    for(j=0;j<aHistogram.yAxis().bins();j++){
      height_4 += aHistogram.binHeight(i,j);
    }
  }
 std::cout
    << " " << height_6 << " " << height_7 << " " << height_8 << std::endl;
  std::cout
    << " " << height_3 << " " << height_4 << " " << height_5 << std::endl;
  std::cout
    << " " << height_0 << " " << height_1 << " " << height_2 << std::endl;

  // Some bins :
  int xbins = aHistogram.xAxis().bins();
  int ybins = aHistogram.yAxis().bins();
  std::cout
    << " * ENTRIES[0,0]     = "
    << aHistogram.binEntries(0,0)
    << " * HEIGHT[0,0] = "
    << aHistogram.binHeight(0,0)
    << " * ERROR[0,0] = "
    << aHistogram.binError(0,0)
    << std::endl;
  std::cout
    << " * ENTRIES[N/2,N/2] = "
    << aHistogram.binEntries(xbins/2,ybins/2)
    << " * HEIGHT[N/2,N/2] = "
    << aHistogram.binHeight(xbins/2,ybins/2)
    << " * ERROR[N/2,N/2] = "
    << aHistogram.binError(xbins/2,ybins/2)
    << std::endl;
  std::cout
    << " * ENTRIES[N-1,N-1] = "
    << aHistogram.binEntries(xbins-1,ybins-1)
    << " * HEIGHT[N-1,N-1] = "
    << aHistogram.binHeight(xbins-1,ybins-1)
    << " * ERROR[N-1,N-1] = "
    << aHistogram.binError(xbins-1,ybins-1)
    << std::endl;
}


