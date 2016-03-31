package hepsim;


import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;
import java.nio.file.*;
import proto.*;
import promc.io.*;
import java.util.zip.*;


/**
*
* Create a new file from the ProMC input using N events.
* The logfile unchanged, but all metadata will be updated (events and luminosity).
* Only first events will be extracted. It is also a distiller since zip32 will moved
* to zip64 (more than 64k entries) 
*
* @author S.Chekanov (ANL)
*
**/

public class Extract {


        static final int BUFFER = 4096;
        static private byte data[];
        static FileOutputStream oof;
        static ZipOutputStream  zout;


        public static void main (String[] args) {

                if (args.length > 0) {

                DecimalFormat formatter = new DecimalFormat("0.#####E0");
                DecimalFormat formatter1 = new DecimalFormat("0.###E0");


                String surl = args[0].trim();
                FileMC promc = new  FileMC(surl,"r"); 

                 int nevent=0;
                 if (args.length == 2) {
                        System.out.println("HepSim: Extract "+args[1].trim()+" events");
                        try  {
                          nevent = Integer.parseInt(args[1]);
                        } catch (NumberFormatException e) {
                           HepSim.ErrorMessage("Error: Cannot parse event number. It is not integer number! Exit!");
                           promc.close();
                           System.exit(1);
                        }
                }



                long    version=promc.getVersion();
                String decription=promc.getDescription();
                long nev1=promc.getNEntries();
                long nev2=promc.getEvents();
                if (nev1 != nev2) {
                      HepSim.ErrorMessage("There is a problem with this file: inconsistent number of entries. The file was truncated!");
                      promc.close();
                      System.exit(1);
                }


                if (nevent>nev1){
                  HepSim.ErrorMessage("The total number of events "+Long.toString(nev1)+" is less than required "+Long.toString(nevent));
                  promc.close();
                  System.exit(1);
                }

                
                // get description
                ProMCDescriptionFile.ProMCDescription desc = promc.fetchDescription();


                ProMCHeaderFile.ProMCHeader header = promc.getHeader();
                int unit = header.getMomentumUnit();
                int lunit = header.getLengthUnit();

                boolean isLog=true;
                if (promc.getLogfileTxt().indexOf("logfile")==-1) isLog=false; 


                ProMCStatFile.ProMCStat stat = promc.getStat();
                System.out.println("File          = "+surl);
                String cross=formatter.format( stat.getCrossSectionAccumulated() );
                String error=formatter.format( stat.getCrossSectionErrorAccumulated()  );
                //System.out.println("ProMC version = "+Long.toString(version));
                //System.out.println("Last modified = "+promc.getLastModified());
                //System.out.println("Description   = "+decription.replace("\n",""));
                System.out.println("Events        = "+Long.toString(nev1));
                if (promc.getRequestedEvents() >0) System.out.println("Requested     = "+Long.toString(promc.getRequestedEvents()));
                //System.out.println("Sigma    (pb) = "+cross+" \u00B1 "+error );
                System.out.println("Lumi   (pb-1) = "+formatter.format(stat.getLuminosityAccumulated()));
                //System.out.println("Varint units  = E:"+Integer.toString(unit)+" L:"+Integer.toString(lunit) );
                //System.out.println("Log file:     = "+ promc.getLogfileTxt()  );

                if (isLog==false)
                   System.err.println(HepSim.ANSI_YELLOW+"Warning: Log file \"logfile.txt\" was not attached!"+HepSim.ANSI_RESET);
                //else
                //   System.out.println(HepSim.ANSI_GREEN+"####  The file is healthy!  ####"+HepSim.ANSI_RESET );


                nev1=nevent;
                String zipFile = surl.replace(".promc","_trimmed.promc");


                // create output file and write records
                try {
               data = new byte[BUFFER];
               (new File(zipFile)).delete();
               oof = new FileOutputStream(zipFile);
               zout = new ZipOutputStream(new BufferedOutputStream(oof));

                // write version of ProMC 
               writeInfo("version",Long.toString(version));


               // write header
               ProMCDescriptionFile.ProMCDescription.Builder desc_header = desc.toBuilder();
               try {
                        data = desc_header.build().toByteArray();
                        ZipEntry entry = new ZipEntry("description");
                        zout.putNextEntry(entry);
                        entry.setSize(data.length);
                        zout.write(data);
                        zout.closeEntry();
                } catch (IOException e) {
                        e.printStackTrace();
                }


               
               // write header
               ProMCHeaderFile.ProMCHeader.Builder rec_header = header.toBuilder();
               try {
                        data = rec_header.build().toByteArray();
                        ZipEntry entry = new ZipEntry("header");
                        zout.putNextEntry(entry);
                        entry.setSize(data.length);
                        zout.write(data);
                        zout.closeEntry();
                } catch (IOException e) {
                        e.printStackTrace();
                }



               // loop over all events 
               for (int event=0; event<nevent; event++){ 
                 ProMC.ProMCEvent ss = promc.read( event );
                 ProMC.ProMCEvent.Builder record = ss.toBuilder(); 

                // write record 
                try {
                        data = record.build().toByteArray();
                        ZipEntry entry = new ZipEntry(Integer.toString(event));
                        zout.putNextEntry(entry);
                        entry.setSize(data.length);
                        zout.write(data);
                        zout.closeEntry();
                } catch (IOException e) {
                        e.printStackTrace();
                }

                } // end loop over events
               

              // write txt description 
               writeInfo("promc_description",decription);
 
                // write number of events
                writeInfo("promc_nevents",Long.toString(nev1));


              // write statistics 
               ProMCStatFile.ProMCStat.Builder rec_stat = stat.toBuilder();
               try {
                        data = rec_stat.build().toByteArray();
                        ZipEntry entry = new ZipEntry("statistics");
                        zout.putNextEntry(entry);
                        entry.setSize(data.length);
                        zout.write(data);
                        zout.closeEntry();
                } catch (IOException e) {
                        e.printStackTrace();
                }

                // write Google templates 
                writeInfo("ProMCHeader.proto",promc.getProMCHeaderTxt());
                writeInfo("ProMC.proto",promc.getProMCTxt());
                writeInfo("ProMCStat.proto",promc.getProMCStatTxt());
                writeInfo("ProMCDescription.proto",promc.getProMCDescriptionTxt());
                writeInfo("logfile.txt",promc.readLogfile());

                // close
                zout.finish();
                zout.close();
                oof.flush();
                oof.close();
                oof = null;

                System.out.println(HepSim.ANSI_GREEN+"Created "+zipFile+"  with " + Long.toString(nev1) + " events"+HepSim.ANSI_RESET );
	        } catch (IOException ioe) {
	            System.out.println("Error creating zip file: " + ioe);
	        }



                promc.close();

                 } else {
                        HepSim.ErrorMessage("Usage: It takes 1 or 2 arguments:  ProMC file location (or its URL) and event to look at (optional)");
                        System.exit(1);
                }


  }


// write a string "info" to a file  
static void writeInfo(String key, String info) {

               try {
               ZipEntry entry = new ZipEntry(key);
               zout.putNextEntry(entry);
               String a = new String(info);
               byte[] theByteArray = a.getBytes();
               entry.setSize(theByteArray.length);
               zout.write(theByteArray);
               zout.closeEntry();
                } catch (IOException e) {
                        e.printStackTrace();
                }

   }



}
