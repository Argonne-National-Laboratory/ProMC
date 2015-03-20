// * This code is licensed under:
// * JHPlot License, Version 1.0
// * - for license details see http://hepforge.cedar.ac.uk/jhepwork/ 
// *
// * Copyright (c) 2005 by S.Chekanov (chekanov@mail.desy.de). 
// * All rights reserved.



using namespace std;

#include <iostream>
#include "CBook/CBook.h"
#include "CBook/H1D.h"
#include "CBook/H2D.h"
#include "HCL/Axis.h"
#include "HCL/Histogram.h"

#include <iostream>
#include <fstream>
#include <iomanip>
using std::ofstream;
using std::ifstream;
using std::setw;

#define IOS_IN std::ios::in
#define IOS_OUT std::ios::out
#define STREAM_OPEN(s) (s.is_open())


#include <vector>
std::vector< H1D* > H1;
std::vector< H2D* > H2;
ofstream *data_file;


CBook* CBook::_singleton = 0;

CBook::CBook()
{ }

CBook* CBook::open(const char* filename)
{
   if (_singleton == 0)
   {
    
      data_file = new ofstream(filename);
     _singleton = new CBook;
   }      // end if
   return _singleton;
}    // end Instance()


// get size of H2 histogram
unsigned int  CBook::sizeH1D()
{
    return  H1.size();
}

// get size of H1 histogram
unsigned int  CBook::sizeH2D()
{
    return  H2.size();
}

// get size of H1 histogram
unsigned int  CBook::size()
{
    return  H1.size()+H2.size();
}


// get size of H1 histogram
void CBook::printH1D(unsigned int h)
{
    if (h<0 || h> H1.size()) {
     std::cout << "WRONG HISTOGRAM ID " <<std::endl;
     return;
    } 
    H1D* h1 = H1[h];
    print(*h1);
}


// get size of H1 histogram
void  CBook::printH2D(unsigned int h)
{
    if (h<0 || h> H2.size()) {
     std::cout << "WRONG HISTOGRAM ID " <<std::endl;
     return;
    }
    HCL::Histogram2D* h2 = H2[h];
    print(*h2);
}


// print all
void CBook::print()
{

        if (H1.size()>0) {
        std::cout << "\nPrint H1D histograms"<<std::endl;
        for(unsigned int i = 0; i < H1.size(); i++ ) {
         H1D* h1 = H1[i];
         print(*h1);
         }
        }


        if (H2.size()>0) {
        std::cout << "\nPrint H1D histograms"<<std::endl;
        for(unsigned int i = 0; i < H2.size(); i++ ) {
         HCL::Histogram2D* h2 = H2[i];
         print(*h2);
         }
        }

        if (H1.size()==0 && H2.size()==0) std::cout << "Histograms were not booked"<<std::endl;
}

// close the file
void CBook::close()
{
    data_file->close();
    clear();
}


// print all
void CBook::clear()
{


         if (H1.size()==0 && H2.size()==0) {
                  std::cout << "CBook: Nothing to clear"<<std::endl;
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
       //std::cout << "CBook: histograms were removed"<<std::endl; 
}






// write to XML file
void CBook::write()
{

  
    (*data_file) << "<jhepwork>\n";
    (*data_file) << "<created-by>C++/C CBook</created-by>\n\n";


        for(unsigned int i = 0; i < H1.size(); i++ ) {
         H1D* h1 = H1[i];
         unsigned int bins = h1->axis().bins();
         (*data_file) << "<histogram-h1d title=\""<< h1->title()<<"\" id=\"" << i << "\">\n";
         (*data_file) << "<x-axis>\n"; 
         (*data_file) << "  <range bins=\""  <<  bins  <<  
                      "\" min=\"" << h1->axis().lowerEdge() << 
                      "\" max=\"" << h1->axis().upperEdge() << "\"/>\n";

         double under=h1->binHeight(HCL::Axis::UNDERFLOW_BIN);
         double over=h1->binHeight(HCL::Axis::OVERFLOW_BIN);

         (*data_file) << "  <out-of-range underflow=\""<< under <<  "\"" <<
                       " overflow=\"" <<   over  << "\"/>\n";
         (*data_file) << "</x-axis>\n"; 


         // statistics
         (*data_file) << "<statistics>\n"; 
         (*data_file) << "  <all-entries>" << h1->allEntries()  
                   << "  </all-entries>\n"; 
         (*data_file) << "  <in-range-entries>" << (h1->allEntries())-under-over  
                   << "  </in-range-entries>\n"; 
         (*data_file) << "  <out-of-range-entries>" << under+over  
                    << "  </out-of-range-entries>\n";

         (*data_file) << "  <mean>" << h1->mean() 
                   << "  </mean>\n";

         (*data_file) << "  <rms>" << h1->rms()
                   << "  </rms>\n";
         (*data_file) << "</statistics>\n"; 


          // bin content
         (*data_file) << "<bincontents order=\"xLow,xHigh,y\">\n";
         (*data_file) << "  binLower,binHigh,hight,error,entries\n";
         (*data_file) << "</bincontents>\n";

          (*data_file) << "<data>\n";
         // append data
         for(unsigned int j = 0; j< bins; j++ ) {
              (*data_file) << 
              h1->axis().binLowerEdge(j) <<"," <<  
              h1->axis().binUpperEdge(j) << "," <<  
              h1->binHeight(j) << "," << h1->binError(j) << 
             "," << h1->binEntries(j) << "\n";
              }
         (*data_file) << "</data>\n";
// end
         (*data_file) << "</histogram-h1d>\n\n";
         };





// 2D histograms
        for(unsigned int i = 0; i < H2.size(); i++ ) {
         H2D* h1 = H2[i];
         // bins :
         unsigned  int xbins = h1->xAxis().bins();
         unsigned  int ybins = h1->yAxis().bins();

         (*data_file) << "<histogram-h2d title=\""<< h1->title()<<"\" id=\"" << i  << "\">\n";

         // x-axis
         (*data_file) << "<x-axis>\n";
         (*data_file) << "  <range bins=\""  <<  xbins  <<
                      "\" min=\"" << h1->xAxis().lowerEdge()  <<
                      "\" max=\"" << h1->xAxis().upperEdge()  << "\"/>\n";

         (*data_file) << "<variable-width-bins>\n";
         for(unsigned int j = 0; j< xbins; j++ ) {
              (*data_file) << h1->xAxis().binLowerEdge(j) << "," 
                        << h1->xAxis().binUpperEdge(j) <<"\n";
              }
        (*data_file) << "</variable-width-bins>\n";
        (*data_file) << "</x-axis>\n";


 // y-axis
         (*data_file) << "<y-axis>\n";
         (*data_file) << "  <range bins=\""  <<  ybins  <<
                      "\" min=\"" << h1->yAxis().lowerEdge()  <<
                      "\" max=\"" << h1->yAxis().upperEdge()  << "\"/>\n";

         (*data_file) << "<variable-width-bins>\n";
         for(unsigned int j = 0; j< ybins; j++ ) {
              (*data_file) << h1->yAxis().binLowerEdge(j) << ","
                        << h1->yAxis().binUpperEdge(j) <<"\n";
              }
        (*data_file) << "</variable-width-bins>\n";
        (*data_file) << "</y-axis>\n";



  // out of range
  //   6 | 7 | 8
  //  -----------
  //   3 | 4 | 5
  //  -----------
  //   0 | 1 | 2

  double height[9];

  height[0] = h1->binHeight(HCL::Axis::UNDERFLOW_BIN,
                                        HCL::Axis::UNDERFLOW_BIN);
  height[2] = h1->binHeight(HCL::Axis::OVERFLOW_BIN,
                                         HCL::Axis::UNDERFLOW_BIN);
  height[6] = h1->binHeight(HCL::Axis::UNDERFLOW_BIN,
                                         HCL::Axis::OVERFLOW_BIN);
  height[8] = h1->binHeight(HCL::Axis::OVERFLOW_BIN,
                                         HCL::Axis::OVERFLOW_BIN);

  height[1] = 0;
  height[7] = 0;
  for(unsigned int i=0;i<xbins;i++){
    height[1] += h1->binHeight(i,HCL::Axis::UNDERFLOW_BIN);
    height[7] += h1->binHeight(i,HCL::Axis::OVERFLOW_BIN);
  }

  height[3] = 0;
  height[5] = 0;
  for(unsigned int j=0;j<ybins;j++){
    height[3] += h1->binHeight(HCL::Axis::UNDERFLOW_BIN,j);
    height[5] += h1->binHeight(HCL::Axis::OVERFLOW_BIN,j);
  }

  height[4] = 0;
  for(unsigned int i=0;i<xbins;i++){
    for(unsigned int j=0;j<ybins;j++){
      height[4] += h1->binHeight(i,j);
    }
  }

        // out of range
         (*data_file) << "<out-of-range-data>\n"; 
         double rout=0;
         for(unsigned int j = 0; j<9; j++ ) {
         (*data_file) << height[j] << "\n";
         rout += height[j];
         }
         (*data_file) << "</out-of-range-data>\n";
         rout=rout-height[4];


         // statistics
         (*data_file) << "<statistics>\n";
         (*data_file) << "  <all-entries>" << h1->allEntries()
                   << "  </all-entries>\n";
         (*data_file) << "  <in-range-entries>" << height[4] 
                   << "  </in-range-entries>\n";
         (*data_file) << "  <out-of-range-entries>" << rout 
                    << "  </out-of-range-entries>\n";
          (*data_file) << "  <x-mean>" << h1->meanX()
                   << "  </x-mean>\n";
          (*data_file) << "  <x-rms>" << h1->rmsX()
                   << "  </x-rms>\n";
         (*data_file) << "  <y-mean>" << h1->meanY()
                   << "  </y-mean>\n";
         (*data_file) << "  <y-rms>" << h1->rmsY()
                   << "  </y-rms>\n";
         (*data_file) << "</statistics>\n";

         // bin content
         (*data_file) << "<bincontents order=\"xy\">\n";
         (*data_file) << "  bin,height,error,entries\n";
         (*data_file) << "</bincontents>\n";
 

         // append data
         (*data_file) << "<data>\n";
         int ii=0;
         for(unsigned int j1 = 0; j1<xbins; j1++ ) {
          for(unsigned int j2 = 0; j2<ybins; j2++ ) {
              (*data_file) << j1 << ","<< j2<< "," << 
              h1->binHeight(j1,j2) <<"," <<
              h1->binError(j1,j2) <<"," <<
              h1->binEntries(j1,j2) <<"\n";
              ii++;
       }  }
         (*data_file) << "</data>\n";
// end
         (*data_file) << "</histogram-h2d>\n\n";
         };


         (*data_file) << "</jhepwork>";


}




//////////////////////////////////////////////////////////////////////////////
void CBook::print(
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
void CBook::print(
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


