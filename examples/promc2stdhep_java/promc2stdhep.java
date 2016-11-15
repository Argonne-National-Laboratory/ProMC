
import java.io.*;
import java.util.*;
import static java.lang.Math.sqrt;

import hep.io.stdhep.StdhepWriter;
import hep.io.stdhep.StdhepEvent;
import hep.io.stdhep.StdhepBeginRun;
import hep.io.stdhep.StdhepEndRun;

import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;
import java.nio.file.*;
import proto.*;
import promc.io.*;
import hepsim.*;
import java.util.zip.*;

/**
 * Reads in ProMC output pair file and creates events in stdhep file format.
 *
 * Format for pairs in HepEVT:
 * Line with number of particles in "event", always 1 and could be removed.
 * Next Line:
 *    1   PDGID 0 0 px py pz mass (== 0.00051099906)
 *
 * example:
 *   1
 *        1         -11           0           0   -0.025880581742   -0.011074878615     0.86548391668     0.00051099906
 *
 * @author S.Chekanov (ANL). Based on Norman A. Graf's stdhep convertor 
 * @version 1.1
 */
public class promc2stdhep 
{

    public static void main(String args[]) throws IOException
    {
        // remind user of correct usage
        if (args.length < 1)
            usage();
        if (args.length == 1 && args[0].equals("-h"))
            usage();

         String surl = args[0].trim();
         FileMC promc = new  FileMC(surl,"r");


        StdhepWriter w = null; 
        String outputFile=args[0]+".stdhep"; 
        File outFile = new File(outputFile);
        if (args.length > 1) { 
             outputFile=args[1].trim(); 
             outFile = new File(outputFile);
        }

        if (outFile.exists()) {
                System.out.println("File exist! Exit run for " + outputFile + " ...");
                System.exit(1);
            } else {
            try
            {
                w = new StdhepWriter(outputFile, "Imported Stdhep Events v1.0", "From file: "+surl, 10);
                w.setCompatibilityMode(false);
            } catch (java.io.IOException ex)
            {
                System.err.println("Error opening file: " + outputFile);
                ex.printStackTrace();
                System.exit(1);
            }
        }

     DecimalFormat formatter = new DecimalFormat("0.#####E0");
     DecimalFormat formatter1 = new DecimalFormat("0.###E0");

        /*--------------------------------------------------------*/
        /* NEVHEP          - event number (or some special meaning*/
        /*                    (see documentation for details)     */
        /* NHEP            - actual number of entries in current  */
        /*                    event.                              */
        /* ISTHEP[IHEP]    - status code for IHEP'th entry - see  */
        /*                    documentation for details           */
        /* IDHEP [IHEP]    - IHEP'th particle identifier according*/
        /*                    to PDG.                             */
        /* JMOHEP[IHEP][0] - pointer to position of 1st mother    */
        /* JMOHEP[IHEP][1] - pointer to position of 2nd mother    */
        /* JDAHEP[IHEP][0] - pointer to position of 1st daughter  */
        /* JDAHEP[IHEP][1] - pointer to position of 2nd daughter  */
        /* PHEP  [IHEP][0] - X momentum [Gev/c]                   */
        /* PHEP  [IHEP][1] - Y momentum [Gev/c]                   */
        /* PHEP  [IHEP][2] - Z momentum [Gev/c]                   */
        /* PHEP  [IHEP][3] - Energy [Gev]                         */
        /* PHEP  [IHEP][4] - Mass[Gev/c^2]                        */
        /* VHEP  [IHEP][0] - X vertex [mm]                        */
        /* VHEP  [IHEP][1] - Y vertex [mm]                        */
        /* VHEP  [IHEP][2] - Z vertex [mm]                        */
        /* VHEP  [IHEP][3] - production time [mm/c]               */
        /*========================================================*/

        //IstHep convention:
        //                            0      - final state particle if JdaHEP=0
        //                                     intermediate particle if JdaHEP>0
        //                                     (NEUGEN extension; was "null")
        //                            1      - final state particle
        //                            2      - intermediate state
        //                            3      - documentation line
        //                            4-10   - reserved for future
        //                            11-200 - reserved for specific model use
        //                            201+   - reserved for users

        int _eventnum = 0;
        //Dummy values...
        int nevtreq = 1;
        int nevtgen = 1;
        int nevtwrt = 1;
        float stdecom = 2.F;
        float stdxsec = 2.F;
        double stdseed1 = 137.;
        double stdseed2 = 138.;
        int oldPercentComplete = 0;




                long    version=promc.getVersion();
                String decription=promc.getDescription();
                long nev1=promc.getNEntries();
                long nev2=promc.getEvents();
                if (nev1 != nev2) {
                      HepSim.ErrorMessage("There is a problem with this file: inconsistent number of entries. The file was truncated!");
                      promc.close();
                      System.exit(1);
                }

                long nevent=nev1;
 
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

                   w.writeRecord(new StdhepBeginRun((int)promc.getRequestedEvents(), (int)nevent, (int)nevent, stdecom, (float)stat.getCrossSectionAccumulated(), unit, lunit));


                 for (int event=0; event<nevent; event++){
                 ProMC.ProMCEvent ss = promc.read( event );
                 ProMC.ProMCEvent.Particles pa = ss.getParticles(); // true particles

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

                int _nhep = 0;                    // number of particles in this event
                int _nmax = pa.getPxCount();      // total number of particles 
                int[] _isthep = new int[_nmax];
                int[] _idhep = new int[_nmax];
                int[] _jmohep = new int[2 * _nmax];
                int[] _jdahep = new int[2 * _nmax];
                double[] _phep = new double[5 * _nmax];
                double[] _vhep = new double[4 * _nmax];











                for (int j = 0; j < pa.getPxCount(); j++) {

                                int pid = 0;
                                if (pidM) pid=pa.getPdgId(j);

                                int status=0;
                                if (statusM) status = pa.getStatus(j);

                                double m =0;
                                if (mM) m=pa.getMass(j) / (double) unit;

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


                                int id=0;
                                if (idM) id = pa.getId(j);

                                 double x=0;
                                 if (xM) x = pa.getX(j) / (double) lunit;

                                 double y=0;
                                 if (yM) y = pa.getY(j) / (double) lunit;

                                 double z=0;
                                 if (zM) z = pa.getZ(j) / (double) lunit;


                                 double t=0;
                                 if (tM) t = pa.getT(j) / (double) lunit;




                    _isthep[_nhep] = (int) status;
                    _idhep[_nhep] = (int) pid; 
                    _jmohep[0+ 2 * _nhep] = (int) m1;
                    _jmohep[1+ 2 * _nhep] = (int) m2;
                    _jdahep[0+ 2 * _nhep] = (int) d1;
                    _jdahep[1+ 2 * _nhep] = (int) d2;

                    _phep[0 + 5 * _nhep] = px; 
                    _phep[1 + 5 * _nhep] = py;
                    _phep[2 + 5 * _nhep] = pz;
                    _phep[3 + 5 * _nhep] = ee; //E
                    _phep[4 + 5 * _nhep] = m; // mass
                    _vhep[0 + 4 * _nhep] = x; //x
                    _vhep[1 + 4 * _nhep] = y; //y
                    _vhep[2 + 4 * _nhep] = z; //z
                    _vhep[3 + 4 * _nhep] = t; //t
                    _nhep++;
                    if(_nhep>_nmax)
                    {
                        throw new RuntimeException("\nAre you sure you want to create an event with more than "+_nmax+" particles?\nIf so, please recompile.");
                    }

                    } // end run over particles

                   StdhepEvent stdevent = new StdhepEvent(_eventnum++, _nhep, _isthep, _idhep, _jmohep, _jdahep, _phep, _vhep);
                   w.writeRecord(stdevent);

                   int percentComplete = (int)((double)_eventnum * 100 / nevent);
                   if(percentComplete % 5 == 0 && percentComplete > oldPercentComplete) {
                                System.out.println(Integer.toString(percentComplete) + "% complete");
                                oldPercentComplete = percentComplete;
                        }

                  

                } // end loop over events


                w.writeRecord(new StdhepEndRun((int)promc.getRequestedEvents(), nevtgen, nevtwrt, stdecom, (float)stat.getCrossSectionAccumulated(), stdseed1, stdseed2));


                // close STDHEP file
                try
                {
                    System.out.println(" Closing file: " + outputFile+" with "+_eventnum+" events");
                    w.close();
                } catch (java.io.IOException ex)
                {
                    System.err.println("Error closing file: " + outputFile);
                    ex.printStackTrace();
                    System.exit(1);
                }





            promc.close();

        }



    public static void usage()
    {
        System.out.println("promc2stdhep: \n  Convert ProMC to stdhep format.\n");
        System.out.println("Usage: \n\n >> java promc2stdhep file.promc file.stdhep \n");
        System.exit(0);
    }
}
