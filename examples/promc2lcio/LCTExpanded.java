/*
 * LCToolsExpanded.java
*/
import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;
import java.nio.file.*;
import proto.*;
import promc.io.*;


import hep.lcio.event.LCFloatVec;
import hep.lcio.event.LCIntVec;
import hep.lcio.event.LCStrVec;

import hep.lcio.event.CalorimeterHit;
import hep.lcio.event.LCCollection;
import hep.lcio.event.LCEvent;
import hep.lcio.event.LCIO;
import hep.lcio.event.MCParticle;
import hep.lcio.event.SimCalorimeterHit;
import hep.lcio.event.SimTrackerHit;
import hep.lcio.event.TPCHit;

import hep.lcio.event.*;
import hep.lcio.io.*;

//Not Needed
import hep.lcio.implementation.event.*;
import hep.lcio.implementation.io.LCFactory;
import hep.physics.event.generator.GeneratorFactory;
import hep.physics.event.generator.MCEvent;
import hep.physics.particle.*;
import hep.physics.particle.properties.*;
import hep.physics.vec.BasicHep3Vector;
import hep.physics.vec.BasicHepLorentzVector;
import hep.physics.vec.Hep3Vector;
import hep.physics.vec.HepLorentzVector;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;



/**
 * Print a given event. Run it as: java LCTExpanded mg5_ttbar_jet_001.slcio 40 
 * @author  T.Johnson, J.Zuselski (ANL) 
 */
public class LCTExpanded
{
   public static void main(String[] args)
   {
try{
   LCReader lcReader=null;
  
   lcReader = LCFactory.getInstance().createLCReader();
   lcReader.open( args[0] ) ;
   System.out.println(args[1]);
   //int nEvents = 0;
   lcReader.skipNEvents(Integer.parseInt(args[1])-1);
   LCEvent evt = lcReader.readNextEvent();
   
   
      // the event:
      System.out.println("event  : " + (evt.getEventNumber()+1) + " - run " + evt.getRunNumber() + " detector : " + evt.getDetectorName() + " - collections  : ");

      String[] strVec = evt.getCollectionNames();

      // loop over collections:
      for (int j = 0; j < strVec.length; j++)
      {
         String name = strVec[j];

         System.out.print("     " + name + " " + evt.getCollection(name).getTypeName() + " : ");

         LCCollection col = evt.getCollection(name);

         // print SimCalorimeterHit collections:
         if (evt.getCollection(name).getTypeName().equals(LCIO.SIMCALORIMETERHIT))
         {
            int nHits = col.getNumberOfElements();
            System.out.print(nHits + " hits - first hit: ");

            int nPrint = (nHits > 0) ? 1 : 0;

            if (nPrint == 0)
               System.out.println();
            for (int i = 0; i < nPrint; i++)
            {
               SimCalorimeterHit hit = (SimCalorimeterHit) col.getElementAt(i);

               float[] x = hit.getPosition();

               System.out.print("    hit -  e: " + hit.getEnergy() + "  pos: " + x[0] + ", " + x[1] + ", " + x[2]);

               if (hit.getNMCParticles() > 0)
                  System.out.print(" - mc: " + hit.getParticleCont(0).getPDG());

               System.out.println();
            }
         }

         // print CalorimeterHit collections:
         else if (evt.getCollection(name).getTypeName().equals(LCIO.CALORIMETERHIT))
         {
            int nHits = col.getNumberOfElements();
            System.out.print(nHits + " hits - first hit: ");

            int nPrint = (nHits > 0) ? 1 : 0;

            if (nPrint == 0)

               System.out.println();
            for (int i = 0; i < nPrint; i++)
            {
               CalorimeterHit hit = (CalorimeterHit) col.getElementAt(i);

               float[] x = hit.getPosition();

               System.out.print("    hit -  e: " + hit.getEnergy() + "  pos: " + x[0] + ", " + x[1] + ", " + x[2]);
               System.out.println();
            }
         }
         else if (evt.getCollection(name).getTypeName().equals(LCIO.SIMTRACKERHIT))
         {
            int nHits = col.getNumberOfElements();
            System.out.print(nHits + " hits : ");

            int nPrint = (nHits > 0) ? 1 : 0;

            if (nPrint == 0)
               System.out.println();
            for (int i = 0; i < nPrint; i++)
            {
               SimTrackerHit hit = (SimTrackerHit) col.getElementAt(i);

               double[] x = hit.getPosition();

               int pdg = -999;
               MCParticle part = hit.getMCParticle();
               if( part != null)
                 pdg = part.getPDG();

               System.out.print("    hit -  dEdx: " + hit.getdEdx() + "  mc: " + pdg + "  pos: " + x[0] + ", " + x[1] + ", " + x[2]);
            }
            System.out.println();
         }
         else if (evt.getCollection(name).getTypeName().equals(LCIO.TPCHIT))
         {
         // check whether we are dealing with raw data
            int flag = col.getFlag();
            int mask = 1<<LCIO.TPCBIT_RAW;
            boolean raw = (flag & mask) == mask;

             
            int nHits = col.getNumberOfElements();
            System.out.print(nHits + " hits : ");

            int nPrint = (nHits > 0) ? 1 : 0;

            if (nPrint == 0)
               System.out.println();
            for (int i = 0; i < nPrint; i++)
            {
               TPCHit hit = (TPCHit) col.getElementAt(i);
	       if(raw){
		   int nRawWords = hit.getNRawDataWords();
		   System.out.print( nRawWords +" raw data words:");
		   for(int rawWord = 0; rawWord< nRawWords; ++rawWord) {
		       System.out.print( " | " + hit.getRawDataWord(rawWord) );
		   }
		   System.out.println(" |");
	       }
            else
            {
               System.out.print("    hit -  id: " + hit.getCellID()
                + "  charge: " + hit.getCharge() + ", quality: " + hit.getQuality() );
                }
            }
             
         }
         else if (evt.getCollection(name).getTypeName().equals(LCIO.LCFLOATVEC))
         {
            int nHits = col.getNumberOfElements();
            System.out.print(nHits + " vectors: ");

            int nPrint = (nHits > 0) ? 1 : 0;

            if (nPrint == 0)
               System.out.println();
            for (int i = 0; i < nPrint; i++)
            {
               LCFloatVec vec = (LCFloatVec) col.getElementAt(i);

               System.out.print(" values(" + i + "): ");

               float[] data = vec.toArray();
               for (int k = 0; k < data.length; k++)
                  System.out.print(data[k] + ", ");
               System.out.println();
            }
         }
         else if (evt.getCollection(name).getTypeName().equals(LCIO.LCINTVEC))
         {
            int nHits = col.getNumberOfElements();
            System.out.print(nHits + " vectors: ");

            int nPrint = (nHits > 0) ? 1 : 0;

            if (nPrint == 0)
               System.out.println();
            for (int i = 0; i < nPrint; i++)
            {
               LCIntVec vec = (LCIntVec) col.getElementAt(i);

               System.out.print(" values(" + i + "): ");

               int[] data = vec.toArray();
               for (int k = 0; k < data.length; k++)
                  System.out.print(data[k] + ", ");
               System.out.println();
            }            
         }         
         else if (evt.getCollection(name).getTypeName().equals(LCIO.LCSTRVEC))
         {
            int nHits = col.getNumberOfElements();
            System.out.print(nHits + " vectors: ");

            int nPrint = (nHits > 0) ? 1 : 0;

            if (nPrint == 0)
               System.out.println();
            for (int i = 0; i < nPrint; i++)
            {
               LCStrVec vec = (LCStrVec) col.getElementAt(i);

               System.out.print(" values(" + i + "): ");

               String[] data = vec.toArray();
               for (int k = 0; k < data.length; k++)
                  System.out.print(data[k] + ", ");
               System.out.println();
            }            
         }         
         else if (evt.getCollection(name).getTypeName().equals(LCIO.MCPARTICLE))
         {
            int nHits = col.getNumberOfElements();
            System.out.print(nHits + " particles : ");

            // int nPrint = (nHits > 0) ? 1 : 0;
             int nPrint =  nHits;

            if (nPrint == 0)
               System.out.println();
            
            
            
            String ID="%5s";
            String L="%10s";
            String E="%10s";
            System.out.println("Units: Energy in GeV,  Length in mm,  Time in sec");
            System.out.format("%6s%10s%6s"+E+E+E+E+E+L+L+L
            +"\n","Nr","Name","PID","Px","Py","Pz","E","Mass","X","Y","Z");

            int nlinse=6+10+6+5*(5)+5*(10)+4*(10);
            for (int linz=0; linz<20; linz++) System.out.print("-");
            System.out.print("\n");

            
            
            for (int i = 0; i < nPrint; i++)
            {
               MCParticle part = (MCParticle) col.getElementAt(i);
               double Vrtx[] = part.getVertex();
               double Mntm[] = part.getMomentum();
               
               formatHelper(i," ",part.getPDG(),Mntm[0],Mntm[1],Mntm[2],part.getEnergy(),part.getMass(),Vrtx[0],Vrtx[1],Vrtx[2]);
               
               /*
               System.out.println(i+" PDG= " + part.getPDG()
               +" x:"+Vrtx[0]+" y:"+Vrtx[1]+" z:"+Vrtx[2]
               +" px:"+Mntm[0]+" py:"+Mntm[1]+" pz:"+Mntm[2]
               );
               */
               
               List<MCParticle> Mothers = part.getParents();
               List<MCParticle> Daughters = part.getDaughters();
               //list.toArray(Mothers);
               //list.toArray(Daughters);
               
               for (int Mcount =0; Mcount<(Mothers.size());Mcount++){
                 MCParticle mother = (MCParticle)Mothers.get(Mcount);
                 double mVrtx[] = mother.getVertex();
                 double mMntm[] = mother.getMomentum();
                 String Mname = "Mother"+Integer.toString(Mcount);
                 
                 formatHelper(-1,name,mother.getPDG(),mMntm[0],mMntm[1],mMntm[2],mother.getEnergy(),mother.getMass(),mVrtx[0],mVrtx[1],mVrtx[2]);
                 
                 /*
                 System.out.println("     mPDG:"+mother.getPDG()
                 +"     x:"+mVrtx[0]+" y:"+mVrtx[1]+" z:"+mVrtx[2]+" px:"+mMntm[0]+" py:"+mMntm[1]+" pz:"+mMntm[2]
                 );
                 */
               }
               for (int Dcount =0; Dcount<(Daughters.size());Dcount++){
                 MCParticle daught = (MCParticle)Daughters.get(Dcount);
                 double dVrtx[] = daught.getVertex();
                 double dMntm[] = daught.getMomentum();
                 String Dname = "Daughter"+Integer.toString(Dcount);
                 
                 formatHelper(-2,name,daught.getPDG(),dMntm[0],dMntm[1],dMntm[2],daught.getEnergy(),daught.getMass(),dVrtx[0],dVrtx[1],dVrtx[2]);
                 
                 /*
                 System.out.println("     dPDG:"+daught.getPDG()
                 +"     x:"+dVrtx[0]+" y:"+dVrtx[1]+" z:"+dVrtx[2]+" px:"+dMntm[0]+" py:"+dMntm[1]+" pz:"+dMntm[2]
                 );
                 */
               }
               System.out.println();
               //Expanded for mothers and daughters  
            }
         }
         else
         {
            System.out.println();
         }
      }
      System.out.println();
   } catch (Exception e) { System.out.println(e.toString()); }
   
   } 
   
   
   public static void formatHelper(int id,String name,int pid,double px,double py,double pz,double ee,double m,double x,double y,double z){
     DecimalFormat formatter = new DecimalFormat("0.#####E0");
     DecimalFormat formatter1 = new DecimalFormat("0.###E0");
     String ID="%5s";
     String L="%10s";
     String E="%10s";

     String sid;
     if (id<0){
     sid=" ";
     }else{
     sid=Integer.toString(id);
     }
     String spid=Integer.toString(pid);
     //String sstatus=Integer.toString(status);
     //String sm1=Integer.toString(m1);
     //String sm2=Integer.toString(m2);
     //String sd1=Integer.toString(d1);
     //String sd2=Integer.toString(d2);
     String spx=formatter1.format(px);
     String spy=formatter1.format(py);
     String spz=formatter1.format(pz);
     String see=formatter1.format(ee);
     String sm=formatter1.format(m);
     String sx=formatter1.format(x);
     String sy=formatter1.format(y);
     String sz=formatter1.format(z);
     //String st=formatter1.format(t);
     if(id==-1){
     name = "Mother";
     }else if (id==-2){
     name = "Daughter";
     }else{
     name = name.substring(0, Math.min(name.length(), 10));
     }
     
     System.out.format("%6s%10s%6s"+E+E+E+E+E+L+L+L+"\n",sid,name,spid,spx,spy,spz,see,sm,sx,sy,sz);
   
   }


/*Author: Joel Zuzelski

  public static void (String fname, int eNum){

    LCReader lcReader=null;
  
    try {
    lcReader = LCFactory.getInstance().createLCReader();
    lcReader.open( fname ) ;

    //int nEvents = 0;
    LCEvent evt = lcReader.readNextEvent(eNum); //Needs to be eNum
    double px; py; pz;
    //Array mother
    //Array daughter

    if( evt != null ) {
      
      LCcollection col = evt.getCollection("MCParticle");
      //nMc=col.getNumberOfElements();
      //LCTools.dumpEvent( evt ) ;



                if (pa.getMother1Count()>1)   m1M=true;
                if (pa.getMother2Count()>1)   m2M=true;
                if (pa.getDaughter1Count()>1) d1M=true;
                if (pa.getDaughter2Count()>1) d2M=true;
                
                if (pa.getXCount()>1)         xM=true;
                if (pa.getYCount()>1)         yM=true;
                if (pa.getZCount()>1)         zM=true;
                if (pa.getPxCount()>1)        pxM=true;
                if (pa.getPyCount()>1)        pyM=true;
                if (pa.getPzCount()>1)        pzM=true;



      apartid = new ArrayList<int[]>();
      amomenta = new ArrayList<double[]>();
      avertex = new ArrayList<double[]>();
      
      } // end filling arrays

      int n =  pa.getPxCount();
      for (int j = 0; j< n; j++)  {


                                int pid = 0;
                                if (pidM) pid=pa.getPdgId(j);
                               
                                int status=0;
                                if (statusM) status = pa.getStatus(j);

                                // skip Pythia8 generator specific 
                                // make it look as in Pythia6
                                if (pid == 90) continue; 
                                if (status != 1 && status != 2) status=3;

                                double px = 0;
                                if (pxM) px=pa.getPx(j) / (double) unit;

                                double py = 0;
                                if (pyM) py=pa.getPy(j) / (double) unit;

                                double pz = 0;
                                if (pzM) pz=pa.getPz(j) / (double) unit;

                                int m1=0;
                                if (m1M) m1=pa.getMother1(j);

                                int m2=0;
                                if (m2M) m2 = pa.getMother2(j);

                                int d1=0;
                                if (d1M) d1 = pa.getDaughter1(j);

                                int d2=0;
                                if (d2M) d2 = pa.getDaughter2(j);

                                 double x=0;
                                 if (xM) x = pa.getX(j) / (double) lunit;

                                 double y=0;
                                 if (yM) y = pa.getY(j) / (double) lunit;

                                 double z=0;
                                 if (zM) z = pa.getZ(j) / (double) lunit;


      System.out.print("X: "+px);
      System.out.print(" Y: "+py);
      System.out.print(" Z: "+pz);

      System.out.print(" | PX: "+px);
      System.out.print(" PY: "+py);
      System.out.println(" PZ: "+pz);

 //Need Daughters and Mothers

      nEvents ++ ;
    
    lcReader.close() ;
    } catch (IOException e) { System.out.println(e.toString()); }
    }
  }
}
*/

}
