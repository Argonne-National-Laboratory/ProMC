package hepsim;


import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;
import java.nio.file.*;
import proto.*;
import promc.io.*;


/**

Validate a ProMC file.
It validates the file and print separate events when requested.

@author S.Chekanov (ANL)

**/

public class Info {



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
                System.out.println("File          = "+surl);
                String cross=formatter.format( stat.getCrossSectionAccumulated() );
                String error=formatter.format( stat.getCrossSectionErrorAccumulated()  );
                System.out.println("ProMC version = "+Long.toString(version));
                System.out.println("Last modified = "+promc.getLastModified());
                System.out.println("Description   = "+decription.replace("\n",""));
                System.out.println("Events        = "+Long.toString(nev1));
                if (promc.getRequestedEvents() >0) System.out.println("Requested     = "+Long.toString(promc.getRequestedEvents()));
                System.out.println("Sigma    (pb) = "+cross+" \u00B1 "+error );
                System.out.println("Lumi   (pb-1) = "+formatter.format(stat.getLuminosityAccumulated()));
                System.out.println("Varint units  = E:"+Integer.toString(unit)+" L:"+Integer.toString(lunit) );
                System.out.println("Log file:     = "+ promc.getLogfileTxt()  );

                if (isLog==false)
                   System.err.println(HepSim.ANSI_YELLOW+"Warning: Log file \"logfile.txt\" was not attached!"+HepSim.ANSI_RESET);
                else
                   System.out.println(HepSim.ANSI_GREEN+"####  The file is healthy!  ####"+HepSim.ANSI_RESET );




               // now look at this event
               if (nevent>-1){ 

                System.out.println("Particle record for event="+Integer.toString(nevent+1));

                   Map<Integer, Double> mass_map = new HashMap<Integer, Double>();
                   Map<Integer, String> name_map = new HashMap<Integer, String>();

                    // use it if you need to use mass
                for (int j = 0; j < header.getParticleDataCount(); j++) {
                        promc.io.ProMCHeaderFile.ProMCHeader.ParticleData d = header
                                        .getParticleData(j);
                        int pid = d.getId();
                        double mass = d.getMass();
                        String name = d.getName();
                        mass_map.put(new Integer(pid), mass);
                        name_map.put(new Integer(pid), name);
                }


               boolean isNLO=false;
               ProMC.ProMCEvent ss = promc.read( nevent );
               ProMC.ProMCEvent.Particles pa = ss.getParticles(); // true particles
               String mess_nlo= "Is this a NLO/NNLO events?  \n If yes, please run it as \"java -cp browser_promc.jar probrowser.NLO file.promc\"";
               try {
                       int aa=pa.getMother1Count();
                       int bb=pa.getXCount();
                       if (aa==0 && bb==0) {isNLO=true; System.out.println( mess_nlo); } 
                   } catch (NoSuchMethodError | IndexOutOfBoundsException e) {
                               System.out.println(mess_nlo);
                               isNLO=true;
                }

                 // fix before v1.3  when energy was not stored
                boolean energyM=false;
                boolean statusM=false;
                boolean m1M=false;
                boolean m2M=false;
                boolean d1M=false;
                boolean d2M=false;
                boolean mM=false;
                boolean xM=false;
                boolean yM=false;
                boolean zM=false;
                boolean tM=false;
                boolean bM=false;
                boolean idM=false;
                boolean pidM=false;
                boolean pxM=false;
                boolean pzM=false;
                boolean pyM=false;

                /*
                java.lang.reflect.Method mmm[] = pa.getClass().getDeclaredMethods();
                      for (int i = 0; i < mmm.length; i++) { 
                      //System.out.println(mmm[i].getName());
                      if (mmm[i].getName().equals("getEnergy")) energyM=true; 
                 }
                 */
                if (pa.getEnergyCount()>1) energyM=true;
                if (pa.getMassCount()>1) mM=true;
                if (pa.getStatusCount()>1) statusM=true;
                if (pa.getMother1Count()>1) m1M=true;
                if (pa.getMother2Count()>1) m2M=true;
                if (pa.getDaughter1Count()>1) d1M=true;
                if (pa.getDaughter2Count()>1) d2M=true;
                if (pa.getXCount()>1) xM=true;
                if (pa.getYCount()>1) yM=true;
                if (pa.getZCount()>1) zM=true;
                if (pa.getTCount()>1) tM=true;
                if (pa.getBarcodeCount()>1) bM=true;
                if (pa.getPdgIdCount()>1)   pidM=true;
                if (pa.getPxCount()>1)   pxM=true;
                if (pa.getPyCount()>1)   pyM=true;
                if (pa.getPzCount()>1)   pzM=true;
                if (pa.getIdCount()>1)       idM=true;

                 if (pa != null) {

                        long ntot=0;
                        double ene=0;

                        String ID="%5s";
                        String L="%10s";
                        String E="%10s";
                        System.out.println("Units: Energy in GeV,  Length in mm,  Time in mm/c");
                        System.out.format("%6s%12s%6s"+ID+ID+ID+ID+ID+E+E+E+E+E+L+L+L+L+"\n","Nr","Name","PID","Stat","M1","M2","D1","D2","Px","Py","Pz","E","Mass","X","Y","Z","T"); 
             
                       int nlinse=6+10+6+5*(5)+5*(10)+4*(10);
                       for (int j=0; j<nlinse; j++) System.out.print("-");
                       System.out.print("\n"); 


                       // check if the record starts from 0
                       boolean start_zero=false;
                       if (pa.getId(0)==0) start_zero=true; 
                       // check special treatment in case of Pythia8
                       // in case of Pythia8, generator line is at line 0 and not counted 
                       if (pa.getPdgId(0) == 90) start_zero=false; 


                        for (int j = 0; j < pa.getPxCount(); j++) {

                        // check. Is this HEPMC?
                        //try {
                         //             pa.getStatus(j);
                        //      } catch (NoSuchMethodError | IndexOutOfBoundsException e) {
                        //              break;
                        //      }

                                int pid = 0;
                                if (pidM) pid=pa.getPdgId(j);


                                String name = "none";
                                if (name_map.containsKey(pid))
                                        name = name_map.get(pid);


                                int status=0;
                                if (statusM) status = pa.getStatus(j);


                                double m =0;
                                if (mM) m=pa.getMass(j) / (double) unit;
                                else if (mass_map.containsKey(pid))
                                           m = mass_map.get(pid); // get from the header


                                double px = 0;
                                if (pxM) px=pa.getPx(j) / (double) unit;

                                double py = 0;
                                if (pyM) py=pa.getPy(j) / (double) unit;

                                double pz = 0;
                                if (pzM) pz=pa.getPz(j) / (double) unit;


                                double ee = 0;
                                // fix before v1.3  when energy was not stored   
                                if (energyM) ee = pa.getEnergy(j) / (double) unit;
                                else ee=Math.sqrt(px*px+py*py+pz*pz+m*m);

                                int m1=0;
                                if (m1M) m1=pa.getMother1(j);

                                int m2=0;
                                if (m2M) m2 = pa.getMother2(j);

                                int d1=0;
                                if (d1M) d1 = pa.getDaughter1(j);

                                int d2=0;
                                if (d2M) d2 = pa.getDaughter2(j);

                                int barcode=0;
                                if (bM) barcode = pa.getBarcode(j);

                                 int id = pa.getId(j);
                                 if (start_zero) id=id+1;

                                 double x=0;
                                 if (xM) x = pa.getX(j) / (double) lunit;

                                 double y=0;
                                 if (yM) y = pa.getY(j) / (double) lunit;

                                 double z=0;
                                 if (zM) z = pa.getZ(j) / (double) lunit;


                                 double t=0;
                                 if (tM) t = pa.getT(j) / (double) lunit;


                                  String sid=Integer.toString(id);
                                  String spid=Integer.toString(pid);
                                  String sstatus=Integer.toString(status);
                                  String sm1=Integer.toString(m1);
                                  String sm2=Integer.toString(m2); 
                                  String sd1=Integer.toString(d1);
                                  String sd2=Integer.toString(d2); 
                                  String spx=formatter1.format(px);
                                  String spy=formatter1.format(py);
                                  String spz=formatter1.format(pz);
                                  String see=formatter1.format(ee);
                                  String sm=formatter1.format(m);
                                  String sx=formatter1.format(x);
                                  String sy=formatter1.format(y);
                                  String sz=formatter1.format(z);
                                  String st=formatter1.format(t);

                                  name = name.substring(0, Math.min(name.length(), 11));

                                  System.out.format("%6s%12s%6s"+ID+ID+ID+ID+ID+E+E+E+E+E+L+L+L+L+"\n",sid,name,spid,sstatus,sm1,sm2,sd1,sd2,spx,spy,spz,see,sm,sx,sy,sz,st);
 
                                  //String txt = sid + "  "+name+"  "+spid+"  "+sstatus+"  "+sm1+"  "+sm2+"  "+sd1+"  "+sd2+"  "+spx+"  "+spy+"  "+spz+"  "+see+"  "+sm+"  "+sx+"  "+sy+"  "+sz+"  "+st;
                                  ntot++;
                                  ene=ene+ee;
                                  //System.out.println(txt);
                                }   // end loop over event
                                  for (int j=0; j<nlinse; j++) System.out.print("-");
                                  System.out.print("\n");
                                  System.out.println("Nr of stored particles="+Long.toString(ntot));
                                  System.out.println("Event energy    (GeV) ="+formatter.format(ene));
                                 }


               } // end event lookup



            










                promc.close();

                 } else {
                        HepSim.ErrorMessage("Usage: It takes 1 or 2 arguments:  ProMC file location (or its URL) and event to look at (optional)");
                        System.exit(1);
                }


  }

}
