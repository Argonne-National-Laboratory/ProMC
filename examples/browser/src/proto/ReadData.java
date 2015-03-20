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


package proto;

import java.io.*;
import promc.io.*; 


/**
 * A simple test example to read ProMC files.
 * 
 * @author S.Chekanov
 *
 */
class ReadData {


public static void main(String[] args) throws Exception {


   //      FileMC file = new FileMC("../cpp/out/output.promc");

         String shtml="http://mc.hep.anl.gov/asc/hepsim/events/pp/100tev/higgs_pythia8/pythia100higgs_001.promc";
         FileMC file = new FileMC(shtml);

         long version=file.getVersion();
         System.out.println("ProMC Version: "+Long.toString(version));
         System.out.println("Description  : "+file.getDescription());
         System.out.println("Nr of events=: "+Long.toString(file.getEvents()));

         ProMCHeaderFile.ProMCHeader header = file.getHeader();

         for (long i=0; i<file.size(); i++) {
         ProMC.ProMCEvent ss = file.read(i); 
         ProMC.ProMCEvent.Particles pa = ss.getParticles(); 
         int bb=pa.getXCount();
         if (i%100==0) System.out.println("Read event="+Long.toString(i)+" Nr="+Integer.toString(bb));
         }

        }



}


