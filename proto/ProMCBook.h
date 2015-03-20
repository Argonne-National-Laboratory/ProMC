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


#ifndef ProMCBook_H 
#define ProMCBook_H

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <exception>
#include "ProMC.pb.h"
#include "ProMCHeader.pb.h"
#include "ProMCStat.pb.h"
#include "ProMCDescription.pb.h"
#include "zip.h"
#include "CBook/ZIP.h"

#include "zipios++/zipios-config.h"
#include "zipios++/meta-iostreams.h"
#include "zipios++/zipoutputstream.h"
#include "zipios++/zipfile.h"
#include "zipios++/zipinputstream.h"


using namespace promc;
using namespace std;
using namespace zipios;

using std::cerr ;
using std::cout ;
using std::endl ;
using std::ios ;
using std::string ;
using std::exception ;
using std::istream;
using std::ofstream;
using std::ifstream;
using std::setw;


class ProMCBook{
public:


     /**
     * Main constructor.
     */
     ProMCBook();
     ~ProMCBook();


     /**
     * Initialize ProMC file. This constructor writes
     * each event on the disk (not in-memory mode).
     * This is the fastest approach for large files.
     * @param[filename] file name
     * @param[option]  options. "w" write, "r" read file 
     */
     ProMCBook(const char* filename, const char * option);


     /**
     * Initialize ProMC file. This constructor writes
     * each event on the disk (not in-memory mode).
     * This is the fastest approach for large files.
     * @param[filename] file name
     * @param[option]  options. "w" write, "r" read file 
     * @param[inMemory] set to true if all events are stored in the memory. The file will be written at the end (after "close()" call). This mode supports files with more than 64k events. 
     */
     ProMCBook(const char* filename, const char * option,bool inMemory);


     /**
     * Initialize and open ProMC file. This constructor writes
     * each event on the disk (not in-memory mode).
     * This is the fastest approach for large files.
     * @param[filename] file name
     * @param[option]  options. "w" write, "r" read file 
     * @param[inMemory] set to true if all events are stored in the memory. The 
file will be written at the end (after "close()" call). This mode supports files with more
     **/ 
     void open(const char* filename, const char * option, bool inMemory);


     /**
     * Get a ProtocolBuffer message representing an event. Use "next()"
     * to navigate to next event. 
     * @return current event 
     **/ 
     ProMCEvent get();

      
     /**
     * Acceess a given ProtocolBuffer message representing an event.
     * It uses random access for fast access.
     * @param[idx] input index of the event  
     * @return event with the number idx  
     **/
     ProMCEvent event(long idx);


     /**
     * Navigate to the next event. Use get() to get the event.
     * @return if negtive, event has reached the end.  
     **/
     int  next();


     /**
     * Timestamp of created file.
     * @return timestamp  
     **/
     int  getTimestamp() { return timestamp; };


     /**
     * Version of the ProMC file. 
     * @return version of the file.  
     **/
     int  getVersion() { return version; }; 


     /**
     * Get the description string. 
     * @return description.  
     **/
     string getDescription() { return description; };

     /**
     * Set the description of the ProMc file. 
     * @param[events] expected number of events to store.
     * @param[s] String with a description.   
     **/
     void setDescription(int events, string s);


     /**
     * Get the number of stored events. 
     * @return[nev] Number of events stored.
     **/
     int  getEvents() { return nev; };

     /**
     * Get the number of requisted events. 
     **/
     int  getEventsRequested() { return requested_nev; };

     /**
     * Get the header file. 
     **/
     ProMCHeader getHeader();


     /**
     * Set the header file. 
     * @param[h] set the header for this file.
     **/
     void  setHeader(ProMCHeader h);

     /**
     * Set  statistics info on the file. 
     * @param[h] Statistics information. 
     **/
     void  setStatistics(ProMCStat h);

     /**
     * Get  statistics info on the file. 
     * @return Statistics information. 
     **/
     ProMCStat getStatistics();  

     
     /**
     * Close the file.
     **/
     void  close();

     /**
     * Write ProMC event. 
     * @param[e] ProtocolBuffer event. 
     **/
     bool  write(ProMCEvent e ); 

     unsigned int  size(); // all size
     static const int current_version=4;


private:

     int  addtype; // I/O type. 
                   // 0: create in memory and write at the end (libzip);
                   // 1: we append each entry to file. File is growing after each entry.
                   //    This is zipios method to write files (as for v1.32)
                   // 2: new zip stream (less reliable)!
     int  timestamp;
     int  version;
     string getString(char* key); // get a string from a key
     string getStringFromStream(char* key, bool isBinary); // get a string from a key
     int addString(string data, string key); // add a string 
     string description; 
     ProMCStat hStat;
     unsigned long nev;      // number of stored events
     unsigned long nentries; // total number of entries insize zip
     unsigned long requested_nev; 
     unsigned long counter;  // current event counter 
     bool isToWrite;
     zip *zipfile;
     zip_file *fzipfile;
     int oerr; 
     int serr;
     bool terminate;
     struct zip_stat st;
     string pfilename;
     // new streaming
     ZipFileWriter* zipSream; // for streaming events to the disk 
     ZipFileReader * zipRead; // for reading
     // zipios staff (as in v1.32)
     ZipOutputStream *outzip;
     ZipInputStream *inpzip;
     ZipFile *zfile;

};

#endif
