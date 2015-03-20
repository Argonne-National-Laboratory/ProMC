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

import java.io.*;
import proto.*;
import promc.io.*;

class ReadProMC {

public static void println(Object... args) {
  System.out.println(args[0]);
}


public static void main(String[] args) throws Exception {

         if ( args.length>0) {
         println("Reading file="+args[0]);

         FileMC file = new FileMC(args[0]);
         long version=file.getVersion();
         println("ProMC Version: "+Long.toString(version));
         println("Description  : "+file.getDescription());
         println("Nr of events=: "+Long.toString(file.getEvents()));

         ProMCHeaderFile.ProMCHeader header = file.getHeader();
         double unit=(double)header.getMomentumUnit();
         double lunit=(double)header.getLengthUnit();
         println("Momentum unit (GeV*unit)"+Double.toString(unit));
         println("Length   unit (cm*unit)"+Double.toString(lunit));


         println("Nr of events=: "+Long.toString(file.size()));
         for (long i=0; i<file.getEvents(); i++) {
         ProMC.ProMCEvent entry = file.read(i); 
         //ProMC.ProMCEvent.Event event =entry.getEvent(); // event  
         ProMC.ProMCEvent.Particles particles = entry.getParticles();   // true particles 

         // fill PT of electron
         for (int j=0; j<particles.getPxCount(); j++){
             double px=particles.getPx(j)/unit; // get pX 
          }

         if (i%10==0) println("Read event="+Long.toString(i));
         }


        } else {
            println("No input file was given!");
            }

       } 

}


