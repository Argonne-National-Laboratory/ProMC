package hepsim;


import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;
import java.nio.file.*;
import proto.*;
import promc.io.*;


/**

 Extract meta information about this file.

@author S.Chekanov (ANL)

**/

public class Meta {



        public static void main (String[] args) {

                if (args.length > 0) {

                DecimalFormat formatter = new DecimalFormat("0.#####E0");
                DecimalFormat formatter1 = new DecimalFormat("0.###E0");


                String surl = args[0].trim();
                FileMC promc = new  FileMC(surl,"r"); 

                 int nevent=-1;
                 if (args.length == 2) {
                        System.out.println("HepSim: Look at event "+args[1].trim());
                        try  {
                          nevent = Integer.parseInt(args[1])-1;
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
                  HepSim.ErrorMessage("The total number of events "+Long.toString(nev1)+" is less than "+Long.toString(nevent));
                  promc.close();
                  System.exit(1);
                }

                ProMCHeaderFile.ProMCHeader header = promc.getHeader();
                int unit = header.getMomentumUnit();
                int lunit = header.getLengthUnit();

                boolean isLog=true;
                if (promc.getLogfileTxt().indexOf("logfile")==-1) isLog=false; 

                ProMCStatFile.ProMCStat stat = promc.getStat();
                System.out.println("file          = "+surl);
                System.out.println("promc_version = "+Long.toString(version));
                System.out.println("last_modified = "+promc.getLastModified());
                System.out.println("description   = "+decription.replace("\n",""));
                System.out.println("events        = "+Long.toString(nev1));
                if (promc.getRequestedEvents() >0) 
                System.out.println("requested     = "+Long.toString(promc.getRequestedEvents()));
                System.out.println("sigma_pb      = "+Double.toString(stat.getCrossSectionAccumulated()));
                System.out.println("sigma_err_pb  = "+Double.toString(stat.getCrossSectionErrorAccumulated()));
                System.out.println("lumi_pb_inv   = "+Double.toString(stat.getLuminosityAccumulated()));
                System.out.println("varint_e      = "+Integer.toString(unit));
                System.out.println("varint_l      = "+Integer.toString(lunit));
                if (isLog==true)
                System.out.println("logfile       = "+ promc.getLogfileTxt()  );
                else
                System.out.println("logfile       = "+ "None" );
                promc.close();

                 } else {
                        HepSim.ErrorMessage("Usage: It takes 1 argument:  ProMC file location (or its URL)");
                        System.exit(1);
                }


  }

}
