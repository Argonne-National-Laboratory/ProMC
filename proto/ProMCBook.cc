/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
**  A library for HEP events storage and processing based on Google's ProtocolBuffers 
**
** Author: S.Chekanov (ANL). chekanov@anl.gov
** Copyright  March, 2013 
** -------------------------------------------------------------------------*/



#include "ProMCBook.h"

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if(start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}



ProMCBook::ProMCBook(){ nev=0; isToWrite=false; }

ProMCBook::~ProMCBook(){ }

ProMCBook::ProMCBook(const char* filename, const char* option, const bool inMemory){
	open(filename,option,inMemory);
}


ProMCBook::ProMCBook(const char* filename, const char* option){
         open(filename,option,false); // write as stream, not to memory 
}




// get a string from a key
string ProMCBook::getString(char* zipkey) {
                int serr=zip_stat(zipfile, zipkey, 0, &st);
                if ( serr != 0) {
                     std::cout<<"zip_stat lookup error on " <<zipkey<<std::endl;
                }
                char  buffer[st.size];
                zip_file *fzipfile = zip_fopen(zipfile, zipkey, 0);
                zip_fread(fzipfile, &buffer, st.size);
                zip_fclose(fzipfile);
                return std::string(buffer,st.size);
}



// get a string from stream-like object.
string ProMCBook::getStringFromStream(char* zipkey, bool isBinary=false) {
                  std::istream* iS=zipRead->Get_File(zipkey,isBinary);
                  string line;
                  std::ostringstream os;
                  *iS>>os.rdbuf();
                  line=os.str();
                  delete iS;
                  return line;
}





// add a string to archive
// return the current index or negative error
// We write it to the memmory, and write to the disc at the end of run.
int ProMCBook::addString(string text, string key) {

        zip_uint64_t len =text.size();
        char *a=new char[len+1]; 
        a[len]=0;
        memcpy(a,text.c_str(),len);
        struct zip_source *src = zip_source_buffer(zipfile, a, len, 0);
        if(src == NULL) { 
                 cerr<< "Error: failed to create source buffer. " << zip_strerror(zipfile) << endl;
                 return -1; };
        zip_int64_t index = -1; 
        // index=zip_file_add(zipfile, key.c_str(), src, 0);
        // zipfile = zip_open(pfilename.c_str(), ZIP_TRUNCATE, &oerr);
         index=zip_add(zipfile, key.c_str(), src);
         // zip_fclose(zipfile);
         // zip_source_free(src);
         // delete [] a;
         // zip_close(zipfile);
                         
        if(index < 0) { 
                  cerr<< "Error: failed to add file to archive. " << zip_strerror(zipfile) << endl;
                  return (long)index;       
        }
        return (long)index;
}




/* This is a function to read or write ProMC file */
void ProMCBook::open(const char* filename, const char* option, const bool inMemory=false) {

	//cout << "PoMC Version: " << PROMCBOOK_VERSION << endl;


	GOOGLE_PROTOBUF_VERIFY_VERSION;
	nev=0;
        terminate=false;

        addtype=1; // add type. 0 if created in memory; 1: append after each entry
        if (inMemory) addtype=0;// this is in memory fill


	if (strcmp(option,"w")==0) {
		isToWrite=true;
		cout << "ProMCBook: Writing file = " << filename << endl;
                char* pPath = getenv("PROMC_OUT");
                oerr=0;
                if (pPath!=NULL) { 
                    string ppath(pPath);
                    cout << "ProMCBook: Env. variable PROMC_OUT is " << ppath << endl;
                    string pname(filename);
                    pfilename=ppath+"/"+pname;
                    cout << "ProMCBook: Output file = " << pfilename << endl;
                    ifstream file(pfilename.c_str());
                    if (file) remove(pfilename.c_str() ); 

                    if (addtype==0) { // in memory 
                    zipfile = zip_open(pfilename.c_str(), ZIP_CREATE, &oerr);
                    if ( oerr != 0 ) {
                        std::cout<<"Error 1: cannot open file for writing: "<< pfilename <<std::endl;
                        exit(1);
                     }
                    }

                   if (addtype==1) { // zipios  
                     outzip = new ZipOutputStream(pfilename.c_str());
                   }
                
                   if (addtype==2) { // every entry is streamed to disk  
                    zipSream = new ZipFileWriter(pfilename.c_str());
                   } 
              

                } else { 
                    cout << "ProMCBook: Output file = " << filename << endl;
                    ifstream file(filename);
                    if (file) remove(filename);
                    pfilename=string(filename); 

                    if (addtype==0) { // in memory
                    zipfile = zip_open(filename, ZIP_CREATE, &oerr);   
                    if ( oerr != 0 ) {
                        std::cout<<"Error 1: cannot open file for writing: "<< pfilename <<std::endl;
                        exit(1);
                     }
                    }

                  if (addtype==1) { // zipios  
                     outzip = new ZipOutputStream(pfilename.c_str());
                   }
                  if (addtype==2) { // every entry is streamed to disk  
                    zipSream = new ZipFileWriter(pfilename.c_str());
                   }


                }

                
		description="none";
		std::stringstream out;
		out << current_version;
		std::string s1=out.str();
                if (addtype==0) addString(s1,"version");
                if (addtype==1) { 
                outzip->putNextEntry( "version" ) ;
                (*outzip) << s1;
                }
                if (addtype==2) {
                                 std::ostream* oS=zipSream->Add_File("version");
                                 *oS<< s1<<endl;
                                 delete oS;
                                } 


               if (addtype==0)  cout << "ProMCBook: I/O mode -> zip64 support in memory, file is written at the end of the run" << endl;
               if (addtype>0)  cout << "ProMCBook: I/O mode -> stream each event to the disk" << endl;


	}  else if (strcmp(option,"r")==0) {
		isToWrite=false;
		ifstream file(filename);
		if (!file)
		{
			cerr<< "-> Error: File =" << filename << " does not exist " << endl;
			cerr<< "->        The program will exit.." << endl;
			exit(1);
		}
		file.close();
		cout << "ProMCBook: Reading file = " << filename << endl;


/////////////////////////////////////////////////////////////////////////

                // decide which approach to use. Is it zip64?
                // we will look at Nr of entries
                 addtype=1;
                 if (inMemory) addtype=0;

                 if (addtype==1) { // check. if streaming, is it still possible using 32bit 
                   zipfile = zip_open(filename, 0, &oerr);
                   zip_int64_t zip_nentries=zip_get_num_entries(zipfile,0);
                   if (zip_nentries>65000) {
                                     addtype=0;
                                     cout << "ProMCBook: I/O mode -> zip64 detected (>65k event support)" << endl;
                                     }
                   zip_close(zipfile);
                 }

                 //cout << "ProMCBook: " << zip_nentries << "entries.." << endl;
                 // if (addtype>0)  cout << "ProMCBook: I/O mode -> zip32 detected (fastest, but limitted to 64k events)" << endl;
 

                if (addtype==0) { // in memory
                zipfile = zip_open(filename, 0, &oerr);
                serr = 0;
                zip_stat_init(&st);
      
                // Nr of entries inside  zip
                nentries=zip_get_num_entries(zipfile, 0);
                char msg1[] = "promc_nevents"; 
                string snev=getString(msg1);
                nev=atoi( snev.c_str() ); 
                char msg2[] = "promc_description";
                description=getString(msg2); 
                const char msg3[] = "description"; 
                serr=zip_stat(zipfile,  msg3, 0, &st);
                if ( serr != 0) {
                       cerr<<"zip_stat lookup error on " <<msg3<< "\n"; 
                       exit(1);
                }
               char  buffer[st.size];
               fzipfile = zip_fopen(zipfile, msg3, 0);
               zip_fread(fzipfile, &buffer, st.size);
               ProMCDescription h;
               h.ParseFromString( std::string(buffer,st.size) );
               zip_fclose(fzipfile);
               // get info
               version = h.version();
	       description=h.description();
	       timestamp=h.timestamp();
	       requested_nev=h.events();


                // check file consitency:
                // check log file. Is it attached?
                const char msg4[] = "logfile.txt";
                serr=zip_stat(zipfile,  msg4, 0, &st);
                if ( serr != 0) { // when a logfile not found  
                     if (nentries < nev) {
                          cerr<< " Error: file consistency check failed! Corrupted file?\n";
                          }
                } else {
                      if (nentries < nev) { // when log file is attached. 
                          cerr<< " Error: consistency check failed! Corrupted file?\n";
                    }
                }

                } // end in memory 



                if (addtype==1) { // Zipios stream 
                zfile = new ZipFile( filename );
                unsigned nlength=zfile->size();
                // reverse iteration over all entries
                // we loop until the last event is found.
                // this is triggered by promc_description entry which follows
                // after the last event
                ConstEntries entries =zfile->entries() ;
                ConstEntries::iterator it ;
                vector<string> metatext;
                for( it = entries.end() ; it != entries.begin();) {
                        --it;
                        // cout << *(*it) << endl ;
                        std::stringstream sout;
                        sout << *(*it);
                        string ss=sout.str();
                        metatext.push_back(ss);
                        std::size_t found = ss.find("promc_description");
                        if (found != std::string::npos) {
                                //istream * stt= zf.getInputStream(ss);
                                // cout << stt->rdbuf() << endl;
                                // cout << "Contents of entry 1: " << cout << stt->rdbuf() << endl;
                                // sentry=ss;
                                //ConstEntryPointer ent = zf.getEntry(ss, FileCollection::MATCH );
                                //if ( ent == 0 ) break;
                                //auto_ptr< istream > is( zf.getInputStream( ent ) ) ;
                                //cout << "Contents of entry 1: " << ent->getName() << " :" << endl ;
                                //cout << is->rdbuf() ;
                                break;
                        }
                }


                // calculate the total number of entries from the file
                nev=nlength-metatext.size()-3; // 3 is the count for version,description,header
                // this is intresting: to get promc_nevents, you must call previous entry promc_description
                istream * stt = zfile->getInputStream( "promc_description", FileCollection::IGNORE );
                // ConstEntryPointer ent = zfile->getEntry( "promc_description", FileCollection::IGNORE ) ;
                if ( stt != 0 ) {
                        // auto_ptr< istream > is( zfile->getInputStream( ent ) ) ;
                        //  cout << is->rdbuf() ;
                         std::stringstream sout;
                         sout << stt->rdbuf();
                         string ss=sout.str();
                         // cout << "DEBUG=" << ss << endl;
                        // int nnev = atoi(ss.c_str());
                }

                
                // read it
                inpzip = new ZipInputStream(filename);
                try {
                        // version
                        std::stringstream sout;
                        sout << inpzip->rdbuf();
                        string ss=sout.str();
                        version = atoi(ss.c_str());
                        next();

                 try {
                                ProMCDescription h;
                                std::stringstream sout;
                                sout << inpzip->rdbuf();
                                h.ParseFromIstream(&sout);
                                version = h.version();
                                description=h.description();
                                timestamp=h.timestamp();
                                requested_nev=h.events();
                        } catch( IOException &e ) {
                                cerr << "IOException caught in fetching description:" << endl ;
                                cerr << e.what() << endl ;
                        }
                        catch( ... ) {
                                cerr << "Unspecified exception caught in fetching description:" << endl ;
                        }




                } catch( IOException &e ) {
                        cerr << "IOException caught in ProMCBook:" << endl ;
                        cerr << e.what() << endl ;
                }
                catch( ... ) {
                        cerr << "Unspecified exception caught in ProMCBook:" << endl ;
                }

                } // end addtype=1 

                if (addtype==2) { // stream type of reading 
                      zipRead = new ZipFileReader(filename);
                      char msg1[] = "promc_nevents";
                      string snev=getStringFromStream(msg1);
                      nev=atoi( snev.c_str() );
                      char msg2[] = "promc_description";
                      description=getStringFromStream(msg2);
                      char msg3[] = "description";
                      ProMCDescription h;
                      std::istream* stt=zipRead->Get_File(msg3,true);
                      std::stringstream sout;
                      sout << stt->rdbuf();
                      h.ParseFromIstream(&sout);
                      delete stt;
                      version = h.version();
                      description=h.description();
                      timestamp=h.timestamp();
                      requested_nev=h.events();
                }



		cout << "ProMCBook: version     = " << version << endl;
		cout << "ProMCBook: Nr entries  = " << nev << endl;
		cout << "ProMCBook: description = " << description << endl;
               
                // cout << "Tot entries insize= " << nentries << endl;
                // calculate the total number of entries from the file
                // nev=nlength-metatext.size()-3; // 3 is the count for version,description,header
                 counter=-1; // start counting from 3, skipping  version,description,header



	} // end read 



}




/**

Return the number of records.

@return Return the size of all records (excluding metadata)
**/
unsigned int  ProMCBook::size()
{
	return  nev;
}




/**
 Get the next record.
 @return 0 if the record was extracted OK. Retun -1 if outside the scope 

**/
int  ProMCBook::next(){

       if (addtype !=1) { // zipios way 
         counter++;
         if (counter>nev) return -1;
       }


        if (addtype==1) { // zipios way 
        try {
                if (inpzip == NULL) return 1;
                if (inpzip->eof()) return 2;
                if (inpzip->fail()) return 3;
                if (inpzip->bad()) return 4;
                zipios::ConstEntryPointer entry =inpzip->getNextEntry() ;
                if(!entry->isValid()) return 5;
        } catch( IOException &e ) {
                // cerr << "IOException caught in main:" << endl ;
                // cerr << e.what() << endl ;
                return 6;
        }
        catch( ... ) {
                //cerr << "Unspecified exception caught in main:" << endl ;
                return 7;
        }
        } // end zipios way


       return 0;
}



/**
 Get the record with the header file.
 @param Header file record.
**/
ProMCHeader ProMCBook::getHeader(){

  ProMCHeader h;
  char msg[] = "header";


  if (addtype==0) { 
  serr=zip_stat(zipfile, msg, 0, &st);
  if ( serr != 0) {
           std::cout<<"No such key! zip_stat lookup error on: " <<msg<<std::endl;
           return h;
   }
   char  buffer[st.size];
   fzipfile = zip_fopen(zipfile, msg, 0);
   zip_fread(fzipfile, &buffer, st.size);
   h.ParseFromString( std::string(buffer,st.size) );
   zip_fclose(fzipfile);
   }

  if (addtype==1) { // zipios way 
   // go to next event
   next();
   try {
                std::stringstream sout;
                sout << inpzip->rdbuf();
                h.ParseFromIstream(&sout);
        } catch( IOException &e ) {
                cerr << "IOException caught in main:" << endl ;
                cerr << e.what() << endl ;
        }
        catch( ... ) {
                cerr << "Unspecified exception caught in main:" << endl ;
        }
  } // zipios way 


  if (addtype==2) {
         std::istream* stt=zipRead->Get_File(msg,true);
         std::stringstream sout;
         sout << stt->rdbuf();
         h.ParseFromIstream(&sout);
         delete stt;
   }


   return h;

}



/**
 Get a data record (event) using a random access. 
 Use a key to extract the record. The key value
 runs from "0" to size()-1. 

 @param key (long) of the record
 @return ProMCEvent record corresponding to this key.
**/
ProMCEvent ProMCBook::event(long  idx){

// http://stackoverflow.com/questions/7781898/get-an-istream-from-a-char
  ProMCEvent eve;
  std::stringstream ss;
  ss << idx;
  string key=ss.str();
  char *zipkey = (char*)key.c_str();

  if (addtype==0) {
  serr=zip_stat(zipfile, zipkey, 0, &st);
  if ( serr != 0) {
           std::cout<<"No such key! zip_stat lookup error on " <<zipkey<<std::endl;
           return eve;
   }
   char  buffer[st.size];
   fzipfile = zip_fopen(zipfile, zipkey, 0);
   zip_fread(fzipfile, &buffer, st.size);
   eve.ParseFromString( std::string(buffer,st.size) );
   zip_fclose(fzipfile);
   return eve;
   }


  // zipios++
  if (addtype==1) {
   try {
        if (idx==-1) key="header";
                istream * stt = zfile->getInputStream( key, FileCollection::IGNORE );
                std::stringstream sout;
                sout << stt->rdbuf();
                eve.ParseFromIstream(&sout);
                return eve;
              }  catch( ... ) {
                cerr << "Unspecified exception caught in main for the key:" << key << endl ;
                return eve;
              }
   } // end zipios way 



   if (addtype==2) {
         std::istream* stt=zipRead->Get_File(zipkey,true);
         std::stringstream sout;
         sout << stt->rdbuf();
         eve.ParseFromIstream(&sout);
         delete stt;
         return eve;
   }


   return eve;
}




/**
 *   Get the statistics record. Should be called last after all events.
 *   @return  
 *   
 **/
ProMCStat ProMCBook::getStatistics(){

  ProMCStat h;
  char msg[] = "statistics";


  if (addtype==0) {
  serr=zip_stat(zipfile, msg, 0, &st);
  if ( serr != 0) {
           std::cout<<"No such key! zip_stat lookup error on: " <<msg<<std::endl;
           return h;
   }
   char  buffer[st.size];
   fzipfile = zip_fopen(zipfile, msg, 0);
   zip_fread(fzipfile, &buffer, st.size);
   h.ParseFromString( std::string(buffer,st.size) );
   zip_fclose(fzipfile);
   return h;
   }

   if (addtype==1) { // start zipios 
    next(); // go to promc_description
    next(); // go to  promc_nevents
    next(); // go to the statistics record 
    try {
                std::stringstream sout;
                sout << inpzip->rdbuf();
                h.ParseFromIstream(&sout);
        } catch( IOException &e ) {
                cerr << "IOException caught in main:" << endl ;
                cerr << e.what() << endl ;
        }
        catch( ... ) {
                cerr << "Unspecified exception caught in main:" << endl ;
        }
       return h; 
   } // end zipios 


   if (addtype==2) {
         std::istream* stt=zipRead->Get_File(msg,true);
         std::stringstream sout;
         sout << stt->rdbuf();
         h.ParseFromIstream(&sout);
         delete stt;
         return h;
   }
   return h;
}

/**
  Get the next record. Make sure you can next() first.
  @return ProMCEvent record.

**/
ProMCEvent ProMCBook::get(){

      ProMCEvent eve;

      if (addtype==0) {
       std::stringstream ss;
       ss << counter;
       string key=ss.str();
       char *zipkey = (char*)key.c_str();
       serr=zip_stat(zipfile, zipkey, 0, &st);
        if ( serr != 0) {
           std::cout<<"No such event: " <<zipkey<<std::endl;
           return eve;
        }
        char  buffer[st.size];
        fzipfile = zip_fopen(zipfile, zipkey, 0);
        zip_fread(fzipfile, &buffer, st.size);
        eve.ParseFromString( std::string(buffer,st.size) );
        zip_fclose(fzipfile);
       return eve;
      }


    if (addtype==1) { // start zipios 
    try {
                std::stringstream sout;
                sout << inpzip->rdbuf();
                eve.ParseFromIstream(&sout);
                return eve;
                //   if (inpzip->good())
                //                    return eve;
        } catch( IOException &e ) {
                cerr << "IOException caught in main:" << endl ;
                cerr << e.what() << endl ;
        }
        catch( ... ) {
                cerr << "Unspecified exception caught in main:" << endl ;
        }
     } // end zipios  


    if (addtype==2) {
         std::stringstream ss;
         ss << counter;
         string key=ss.str();
         char *zipkey = (char*)key.c_str();
         std::istream* stt=zipRead->Get_File(zipkey,true);
         std::stringstream sout;
         sout << stt->rdbuf();
         eve.ParseFromIstream(&sout);
         delete stt;
         return eve; 
   }

      return eve;


}


/**
 Set a header file.
 @param h Header file.
**/
void ProMCBook::setHeader( ProMCHeader h ) {

       // in memory
       if (addtype==0) {
       std::string out;
       if (!h.SerializeToString(&out)) {
		cerr << "Failed to write header" << endl;
	}
       addString(out,"header");
       h.Clear();
       return;
       }

      if (addtype==1) {
        std::string out;
        if (!h.SerializeToString(&out)) {
                cerr << "Failed to write header" << endl;
        }
        outzip->putNextEntry( "header" ) ;
        (*outzip) << out;
        h.Clear();
        return;
       }

      // stream to disk
      if (addtype==2) {
       std::string out;
       if (!h.SerializeToString(&out)) {
                cerr << "Failed to write header" << endl;
        }
       std::ostream* oS=zipSream->Add_File("header");
       *oS<<out<<endl;
       delete oS;
       h.Clear();
       return;
       }

      return;

}


/**
  Set statistics information
  @param h Statistics info
**/
void ProMCBook::setStatistics( ProMCStat h ) {
	hStat=h;
}


/**

 Set the description information.
  @param requested_events Requested events (not the actual!)
  @param describe description

**/
void ProMCBook::setDescription( int requested_events, string  describe ) {

	description=describe;
	std::time_t t = std::time(0);  // t is an integer type
	ProMCDescription  eve;
	eve.set_version(current_version);
	eve.set_description(description);
	eve.set_events(requested_events);
	eve.set_timestamp((int)t);
	std::string out;
	if (!eve.SerializeToString(&out)) {
		cerr << "Failed to write description" << endl;
	}

        if (addtype==0)  {addString(out,"description"); return;};

        // zipios 
        if (addtype==1) {
           outzip->putNextEntry( "description" ) ;
           (*outzip) << out;
           eve.Clear();
           return;
         } 

        // stream
        if (addtype==2) {
            std::ostream* oS=zipSream->Add_File("description",true);
            *oS<<out<<endl;
            delete oS;
            return;
         }

	eve.Clear();

}





/**
 Write an event.
 Return true if no errors.
 @param eve Event to be written.
**/
bool  ProMCBook::write( ProMCEvent eve ) {

	std::string out;
	if (!eve.SerializeToString(&out)) {
		cerr << "Failed to write event" << endl;
	}
        std::stringstream sout;
        sout << nev;

        // in memory
        if (addtype==0)  { 
        addString(out,sout.str());
        }

        // zipios
        if (addtype==1)  {
        std::stringstream sout;
        sout << nev;
        outzip->putNextEntry( sout.str() ) ;
        (*outzip) << out;
        }

        // stream to disk
        if (addtype==2) {
            std::ostream* oS=zipSream->Add_File(sout.str(),true);
            *oS<<out<<endl;
            delete oS;
        }
        eve.Clear();
        nev++;

        if (nev>64999 && addtype != 0) {
                        cerr << "ProMCBook: Warning:" << endl;
                        cerr << "-> We have reached 64k limit for zip32! "<< endl;
                        cerr << "-> Gracefully exit saving all events.." << endl;
                        cerr << "-> Use \"memory\" option when creating ProMC to avoid this limit" << endl;
                        close();
                        terminate=true;
                        return false;
                        }

    return true;
}



/**
 Close all files and write metadata.
**/
void ProMCBook::close() {


        // termination is already done
       // if (terminate==true) return;

	if (isToWrite==true) {


		cout << " ##### Closing ProMC ##### " << endl;
		string protodir="";
                char* pProto = getenv("PROMC_PROTO");
                if (pProto!=NULL) {
                    string ppath(pProto);
                    cout << "ProMCBook: Env. variable PROMC_PROTO is " << ppath << endl;
                    protodir=ppath+"/";
                    cout << "ProMCBook: proto files directory = " << protodir << endl;
                }
 

		// description. always comes after the last event
		std::stringstream out2;
		out2 << description;
                if (addtype==0)  addString(out2.str(),"promc_description");

                if (addtype==1) {
                // description. always comes after the last event
                outzip->putNextEntry( "promc_description" ) ;
                (*outzip) << out2.str();
                 }

                if (addtype==2) {
                      std::ostream* oS=zipSream->Add_File("promc_description");
                     *oS<<out2.str()<<endl;
                     delete oS;
                  }

		// write number of processed events
		std::stringstream out1;
		out1 << nev;
                if (addtype==0) addString(out1.str(),"promc_nevents");

                if (addtype==1) {
                outzip->putNextEntry( "promc_nevents" ) ;
                (*outzip) << out1.str();
                }
           
                if (addtype==2) {
                      std::ostream* oS=zipSream->Add_File("promc_nevents");
                     *oS<<out1.str()<<endl;
                     delete oS;
                  }





                // write statistics 
                std::string outS;
                // add a dummy, if nothing to write 
                if (!hStat.SerializeToString(&outS)) {
                        cerr << "Failed to write statistics information" << endl;
                        ProMCStat stat; // write dummy stattistics
                        stat.SerializeToString(&outS);
                        if (addtype==1) {
                        outzip->putNextEntry( "statistics" ) ;
                        (*outzip) << outS;
                        stat.Clear();
                        }
                        if (addtype==0) addString(outS,"statistics");
                        if (addtype==2) {
                               std::ostream* oS=zipSream->Add_File("statistics",true);
                               *oS<<outS<<endl;
                               delete oS;
                         }

                } else { // write it here 
                  if (addtype==1) {
                     outzip->putNextEntry( "statistics" ) ;
                     (*outzip) << outS;
                     }
                  if (addtype==0) addString(outS,"statistics");      
                  if (addtype==2) {
                           std::ostream* oS=zipSream->Add_File("statistics",true);
                           *oS<<outS<<endl;
                           delete oS;
                   }
 
                  }

               

		bool ierr=false;

		string filename=protodir+"proto/ProMCHeader.proto";
		ifstream ifs2( filename.c_str(), ios::in | ios::binary ) ;
		if(!ifs2.fail()) {
                        stringstream strStream;
                        strStream << ifs2.rdbuf();//read the file
                        string str = strStream.str();//str holds the content of the file
                        if (addtype==0) addString(str,"ProMCHeader.proto");

                        if (addtype==1) {
                        outzip->putNextEntry("ProMCHeader.proto");
                        (*outzip) << str;
                        if (outzip->fail())
                                cout << " Problem with writing "<< filename << endl;
                        }
                        if (addtype==2) {
                               std::ostream* oS=zipSream->Add_File("ProMCHeader.proto");
                               *oS<<str<<endl;
                               delete oS;
                         }
			ifs2.close();
			//ierr=true;
		} else {
			cout << " Header file=" << filename << " not found" << endl;
			ierr=true;
		}

		filename=protodir+"proto/ProMC.proto";
		ifstream ifs3( filename.c_str(), ios::in | ios::binary ) ;
		if(!ifs3.fail()) {
                        stringstream strStream;
                        strStream << ifs3.rdbuf();//read the file
                        string str = strStream.str();//str holds the content of the file
                        if (addtype==0) addString(str,"ProMC.proto");
                        if (addtype==1) {
                           outzip->putNextEntry("ProMC.proto");
                          (*outzip) << str;
                        }
                        if (addtype==2) {
                               std::ostream* oS=zipSream->Add_File("ProMC.proto");
                               *oS<<str<<endl;
                               delete oS;
                         }
                        ifs3.close();
		} else {
			cout << " Event record file=" << filename << " not found" << endl;
			ierr=true;
		}


		filename=protodir+"proto/ProMCStat.proto";
		ifstream ifs4( filename.c_str(), ios::in | ios::binary ) ;
		if(!ifs4.fail()) {
                        stringstream strStream;
                        strStream << ifs4.rdbuf();//read the file
                        string str = strStream.str();//str holds the content of the file
                        if (addtype==0) addString(str,"ProMCStat.proto");

                        if (addtype==1) {
                         outzip->putNextEntry("ProMCStat.proto");
                         (*outzip) << str;
                        }

                        if (addtype==2) {
                               std::ostream* oS=zipSream->Add_File("ProMCStat.proto");
                               *oS<<str<<endl;
                               delete oS;
                         }
                        ifs4.close();
		} else {
			cout << " Statistics file=" << filename << " not found" << endl;
			ierr=true;
		}


		filename=protodir+"proto/ProMCDescription.proto";
		ifstream ifs5( filename.c_str(), ios::in | ios::binary ) ;
		if(!ifs5.fail()) {
                        stringstream strStream;
                        strStream << ifs5.rdbuf();//read the file
                        string str = strStream.str();//str holds the content of the file
                        if (addtype==0) addString(str,"ProMCDescription.proto");
                        if (addtype==1) {
                             outzip->putNextEntry("ProMCDescription.proto");
                             (*outzip) << str;
                        }
                        if (addtype==2) {
                               std::ostream* oS=zipSream->Add_File("ProMCDescription.proto");
                               *oS<<str<<endl;
                               delete oS;
                         }
                        ifs5.close();
		} else {
			cout << " Description file=" << filename << " not found" << endl;
			ierr=true;
		}

		if (ierr){
			cout << " -> Warning: Not self-describing file format." << endl;
			cout << "    To make it self-describing, put *.proto files to the directory proto/" << endl;
		}



		// write a log file if exists. Always the last one.
		filename="logfile.txt";
                char* pLogfile = getenv("PROMC_LOG");
                if (pLogfile!=NULL) {
                    string ppath(pLogfile);
                    cout << "ProMCBook: Env. variable PROMC_LOG is " << ppath << endl;
                    string pname(filename);
                    filename=ppath+"/"+pname;
                    cout << "ProMCBook: Log file = " << filename << endl;
                }      

		ifstream ifs1( filename.c_str(), ios::in | ios::binary );
		if(!ifs1.fail()) {
                        stringstream strStream;
                        strStream << ifs1.rdbuf();//read the file
                        string str = strStream.str();//str holds the content of the file
                        if (addtype==0) addString(str,"logfile.txt");

                        if (addtype==1) { 
                        outzip->putNextEntry("logfile.txt");
                        (*outzip) << str;
                        }
                        if (addtype==2) {
                               std::ostream* oS=zipSream->Add_File("logfile.txt");
                               *oS<<str<<endl;
                               delete oS;
                         }
                        ifs1.close();
			cout << " Info: File=" << filename << " is attached" << endl;
		} else {

			cout << " Info: File=" << filename << " not found. No logfile info is written" << endl;
		}


             if (addtype==0)  {
                               zip_close(zipfile);
                               cout << " ##### In memory ProMC file is closing .. #####" << endl;
                              }

             if (addtype==1) {  outzip->close(); delete outzip; cout << " ##### ProMC file is closed. #####" << endl;  }
             if (addtype==2) {  delete zipSream;  cout << " ##### ProMC file is closed. #####" << endl;  }


	}



        // reading
        if ( isToWrite==false) {
          if (addtype==0)  zip_close(zipfile);
          if (addtype==1) {inpzip->close(); delete inpzip; zfile->close(); delete zfile;};
          if (addtype==2) {delete zipRead;}
          cout << " ##### ProMC file is closed. #####" << endl;
      }
       
          
 
}




