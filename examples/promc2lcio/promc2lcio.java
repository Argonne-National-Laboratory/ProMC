/*
* Reads ProMC and converts it the the LCIO format.
*/
import java.io.*;
import java.util.*;
import static java.lang.Math.sqrt;
import hep.lcio.event.*;
import hep.lcio.io.*;
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
 * Reads ProMC and converts it the the LCIO format.
 * The code is based on STDHEP convertors by Tony J. 
 * @author S.Chekanov (ANL), J.Zuzelski (ANL), and Jeremy McCormick <jeremym@slac.stanford.edu>.  
 * @version 1.3
 */
public class promc2lcio
{

	private final static String detName = "NONE";
	private final static String ecalName = "NONE";
	private final static String tpcName = "NONE";
	private static GeneratorFactory factory = new GeneratorFactory();
	private static ParticlePropertyManager pman=new ParticlePropertyManager();
	private static  ParticlePropertyProvider ppp = pman.getParticlePropertyProvider();
	private static final double c_light = 2.99792458e+8;
	private static boolean haveWarned = true;

	// temporary arrays
	private static ArrayList<int[]> apartid;
	private static ArrayList<double[]>  amomenta;
	private static ArrayList<double[]>  avertex;

	public static void main(String args[]) throws IOException
	{
		// remind user of correct usage
		if (args.length < 1)
			usage();
		if (args.length == 1 && args[0].equals("-h"))
			usage();

		String surl = args[0].trim();
		FileMC promc = new  FileMC(surl,"r");

		String outputFile=args[0]+".lcio";

		File outFile = new File(outputFile);
		if (args.length > 1) {
			outputFile=args[1].trim();
			outFile = new File(outputFile);
		}


		LCWriter lcWrt = LCFactory.getInstance().createLCWriter();

		if (outFile.exists()) {
			System.out.println("File exist! Exit run for " + outputFile + " ...");
			System.exit(1);
		} else {
			try
			{

				lcWrt.open(outputFile);

			} catch (java.io.IOException ex)
			{
				System.err.println("Error opening file: " + outputFile);
				ex.printStackTrace();
				System.exit(1);
			}
		}

		DecimalFormat formatter = new DecimalFormat("0.#####E0");
		DecimalFormat formatter1 = new DecimalFormat("0.###E0");

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


		ILCRunHeader runHdr = new ILCRunHeader();
		runHdr.setRunNumber(1000);
		runHdr.setDetectorName(detName);
		runHdr.setDescription( decription.replace("\n",""));
		//runHdr.addActiveSubdetector(ecalName);
		//runHdr.addActiveSubdetector(tpcName);
		runHdr.getParameters().setValue("ProMC:version",version) ;
                runHdr.getParameters().setValue("ProMC:lastmodified",promc.getLastModified()) ;
                runHdr.getParameters().setValue("ProMC:SigmaPB", (float)stat.getCrossSectionAccumulated() ) ;
                runHdr.getParameters().setValue("ProMC:SigmaErrorPB",(float)stat.getCrossSectionErrorAccumulated()) ;
                runHdr.getParameters().setValue("ProMC:EventsRequested",(float)promc.getRequestedEvents()) ;
                runHdr.getParameters().setValue("ProMC:LumiPBINV",(float)stat.getLuminosityAccumulated()) ;
                runHdr.getParameters().setValue("ProMC:eCM",(float)header.getECM()) ;
 
		float Fcross = Float.parseFloat(cross);
		float ERRcross = Float.parseFloat(error);
		float[] idx = { Fcross,ERRcross} ;
		runHdr.getParameters().setValues("cross_section_pb",idx) ;
		lcWrt.writeRunHeader(runHdr);
            

                // loop over events 
		for (int event=0; event<nevent; event++){
			ProMC.ProMCEvent ss = promc.read( event );
			ProMC.ProMCEvent.Event proev = ss.getEvent(); // event
			ILCEvent evt = new ILCEvent();
			evt.setRunNumber(1);            // set to 1 for consistancy with lcio2hepsim 
			evt.setEventNumber(event+1);    //automatic 
			evt.setDetectorName(detName);
			evt.setWeight( proev.getWeight()  ) ;
			evt.setTimeStamp( (int) (new Date().getTime()/1000) );

                        // add generator information per each event
                         ILCGenericObject obj = new ILCGenericObject() ;
                         double weight=proev.getWeight();
                         double scalePDF=proev.getScalePDF();
                         double scale=proev.getScale();      
                         double qed=proev.getAlphaQED();      
                         double qcd=proev.getAlphaQCD();       
                         double pdf1=proev.getPDF1();
                         double pdf2=proev.getPDF2();
                         double x1=proev.getX1();
                         double x2=proev.getX2();
                         obj.setDoubleVal(weight,0);
                         obj.setDoubleVal(scale,1);
                         obj.setDoubleVal(qed,2);
                         obj.setDoubleVal(qcd,3);
                         obj.setDoubleVal(scalePDF,4);
                         obj.setDoubleVal(pdf1,5);
                         obj.setDoubleVal(pdf2,6);
                         obj.setDoubleVal(x1,7);
                         obj.setDoubleVal(x2,8);
                         // process, ID
                         obj.setIntVal(proev.getProcessID(),0);
                         obj.setIntVal(proev.getID1(),1);
                         obj.setIntVal(proev.getID2(),2);



                         // adding parameters
                         LCParameters param = evt.getParameters(); 
                         param.setValue("EVGEN:ProcessID", (int)proev.getProcessID());
                         param.setValue("EVGEN:Process", header.getName());
                         param.setValue("EVGEN:Weight", (float)proev.getWeight());
                         param.setValue("EVGEN:Code", (int)header.getCode());
                         param.setValue("EVGEN:eCM", (float)header.getECM());
                         param.setValue("EVGEN:ID1", (int)proev.getID1());
                         param.setValue("EVGEN:ID2", (int)proev.getID2());
                         param.setValue("EVGEN:ScalePDF", (float)scalePDF);
                         param.setValue("EVGEN:Scale", (float)scale);
                         param.setValue("EVGEN:AlphaQED", (float)qed);
                         param.setValue("EVGEN:AlphaQCD", (float)qcd);
                         param.setValue("EVGEN:PDF1", (float)pdf1);
                         param.setValue("EVGEN:PDF2", (float)pdf2);
                         param.setValue("EVGEN:X1", (float)x1);
                         param.setValue("EVGEN:X2", (float)x2);
                         param.setValue("EVGEN:SigmaPB", (float)stat.getCrossSectionAccumulated());
                         param.setValue("EVGEN:SigmaErrorPB", (float)stat.getCrossSectionErrorAccumulated());
                         param.setValue("EVGEN:EventsRequested", (float)promc.getRequestedEvents());
                         // specific for DIS (how it was included to ProMC)
                         param.setValue("EVGEN:DIS:W", (float)scale);
                         param.setValue("EVGEN:DIS:Q2", (float)scalePDF);
                         param.setValue("EVGEN:DIS:ISR", 0);
                         param.setValue("EVGEN:DIS:YBJ", (float)qed);
                         param.setValue("EVGEN:DIS:XBJ", (float)qcd);



                        ILCCollection generic = new ILCCollection(LCIO.LCGENERICOBJECT);
                        generic.add(obj);
                        evt.addCollection(generic,"MCInfo");

                        
                        // extra MC parameters
                        java.util.List<java.lang.Float> fdata=proev.getFdataList();
                        java.util.List<java.lang.Long>  idata=proev.getIdataList();
                        ILCGenericObject ext =null;
                        ILCCollection extra = null;

                        if (idata != null)
                              if (proev.getIdataCount()>0) ext = new ILCGenericObject();
                        if (fdata != null)
                              if (proev.getFdataCount()>0) ext = new ILCGenericObject();


                        if (idata != null)
                         if (proev.getIdataCount()>0) {
                          for (int i=0; i<idata.size(); i++) ext.setIntVal((idata.get(i)).intValue(),i);
                          extra = new ILCCollection(LCIO.LCGENERICOBJECT);
                          extra.add(ext);
                          evt.addCollection(generic,"MCParameters");
                        }


                        if (fdata != null) 
                        if (proev.getFdataCount()>0) { 
                          if (extra == null)  extra = new ILCCollection(LCIO.LCGENERICOBJECT);
                          for (int i=0; i<fdata.size(); i++) ext.setDoubleVal(new Double(fdata.get(i)),i);
                          extra.add(ext);
                          evt.addCollection(generic,"MCParameters");
                        }

			// create and add some mc particles
			ILCCollection mcpcoll = convertILCIO(ss, unit, lunit);
			evt.addCollection(mcpcoll, LCIO.MCPARTICLE);

			int percentComplete = (int)((double)event * 100 / nevent);
			if(percentComplete % 5 == 0 && percentComplete > oldPercentComplete) {
				System.out.println(Integer.toString(percentComplete) + "% complete");
				oldPercentComplete = percentComplete;
			}




			// write the event to the file
			lcWrt.writeEvent(evt);

			// dump the event to the screen
			// LCTools.dumpEvent(evt);

		} // end loop over events





		lcWrt.close();
		promc.close();

	}



	private static void checkAndAddDaughter(IMCParticle[] particle, List ancestors, int parentID, int childID)
	{
		if (parentID == childID)
			return; // Can't be parent of self
                if (childID>=ancestors.size()) return;


		Set ancestor = (Set) ancestors.get(childID);

                if (parentID>=particle.length) return;
                IMCParticle pp = particle[parentID];
                if (pp == null) return;

		boolean added = ancestor.add(pp);
		if (added)
			particle[parentID].addDaughter(particle[childID]);
	}

	private static int fillIndexVec(int[] vec, int idx1, int idx2)
	{
		int l = 0;
		try
		{
			if (idx1 >= 0 && idx2 >= 0)
			{
				if (idx1 < idx2)
				{
					for (int i = idx1; i < (idx2 + 1); i++)
					{
						vec[l++] = i;
					}
				}
				else if (idx1 > idx2)
				{
					vec[l++] = idx1;
					vec[l++] = idx2;
				}
				// indices are equal
				else
				{
					vec[l++] = idx1;
				}
			}
			else if (idx1 >= 0)
			{
				vec[l++] = idx1;
			}
		}
		catch (ArrayIndexOutOfBoundsException x)
		{
			if (!haveWarned) System.err.println("Warning: Array index out of bounds exception caused by corrupt stdhep file ignored");
			haveWarned = true;
		}
		return l;
	}




	/**
	 * Convert a StdhepEvent to an LCCollection of MCParticle objects.
	 * @param hepevt The StdhepEvent to be converted.
	 * @return An LCCollection of MCParticle objects converted from hepevt.
	 */
	public static ILCCollection convertILCIO(ProMC.ProMCEvent ss, int unit, int lunit)
	{



		ILCCollection mcpcoll = new ILCCollection(LCIO.MCPARTICLE);
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



		apartid = new ArrayList<int[]>();
		amomenta = new ArrayList<double[]>();
		avertex = new ArrayList<double[]>();

		int n =  pa.getPxCount();
		// System.out.println("Original size="+Integer.toString(n));
		for (int j = 0; j< n; j++)  {


			int pid = 0;
			if (pidM) pid=pa.getPdgId(j);

			int status=0;
			if (statusM) status = pa.getStatus(j);

			// skip Pythia8 generator specific
			// make it look as in Pythia6
			if (pid == 90) continue;
			if (status != 1 && status != 2) status=3;

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


			// fill the arrays
			apartid.add(  new int[]{pid,status,m1,m2,d1,d2,barcode} );
			amomenta.add( new double[]{px,py,pz,ee,m} );
			avertex.add(  new double[]{x,y,z,t});

		} // end filling arrays


		// redefine the size
		n=apartid.size();
		// System.out.println("After skiping gen-dependent particles="+Integer.toString(n));

		IMCParticle particles[] = new IMCParticle[n];

		// now fill it
		for (int i=0; i<n; i++) {

			int[] aaap=(int[])apartid.get(i);
			int pid=aaap[0];
			int status=aaap[1];

			double[] aaamom=(double[])amomenta.get(i);
			double px=aaamom[0];
			double py=aaamom[1];
			double pz=aaamom[2];
			double ee=aaamom[3];
			double m=aaamom[4];

			double[] aaavtx=(double[])avertex.get(i);
			double x=aaavtx[0];
			double y=aaavtx[1];
			double z=aaavtx[2];
			double t=aaavtx[3];

			// Create new MCParticle for this Stdhep record.
			IMCParticle particle = new IMCParticle();

			// Set vertex from VHEP.
			double vertex[] = { x, y, z };
			particle.setVertex(vertex);

			// Set momentum from PHEP.
			double momentum[] ={px,py,pz };
			particle.setMomentum(momentum);

			// Lookup the particle by PDG using the Particle Property Provider.
			ParticleType type;
			try
			{
				// Get the particle type.
				type = ppp.get(pid);
				// Set the charge.
				particle.setCharge((float) type.getCharge());
			}
			catch (UnknownParticleIDException e)
			{
				// Flag the particle with NaN for unknown charge.
				particle.setCharge(Float.NaN);
			}

			// Set mass from PHEP.
			particle.setMass((float)m);

			// Set PDG from IDHEP.
			particle.setPDG(pid);
			/*
			0 empty line
			1 undecayed particle, stable in the generator
			2 particle decayed in the generator
			3 documentation line
			*/

			particle.setGeneratorStatus( status ) ;

			// Set time from VHEP(4).
			// Convert to mm/c^2 from mm/c, as in slic/StdHepToLcioConvertor .
			particle.setTime((float)(t / c_light));

			// Add MCParticle to the temp array.
			particles[i] = particle;
		}






		int[] vec = new int[n];
		//List<Set<BasicParticle>> ancestors = new ArrayList<Set<BasicParticle>>(n);
		List ancestors = new ArrayList();
		for (int i = 0; i < n; i++)
			ancestors.add(new HashSet());
		// Deal with parents
		for (int i = 0; i < n; i++)
		{

			int[] aaap=(int[])apartid.get(i);
			int m1=aaap[2]; // mother 1
			int m2=aaap[3]; // mother 2
			int idx1 = m1 - 1;
			int idx2 = m2 - 1;
			//int idx1 = pa.getMother1(i);
			//int idx2 = pa.getMother2(i);
			int l = fillIndexVec(vec, idx1, idx2);
			//System.out.println("parent: "+i+" "+idx1+" "+idx2+" "+l);
			for (int j = 0; j < l; j++)
			{
                                if (j<vec.length)  
				   checkAndAddDaughter(particles, ancestors, vec[j], i);
			}
		}
		// Deal with daughters
		for (int i = 0; i < n; i++)
		{

			int[] aaap=(int[])apartid.get(i);
			int d1=aaap[4]; // daughter 1
			int d2=aaap[5]; // daughter 2
			int idx1 = d1 % 10000 - 1;
			int idx2 = d2 % 10000 - 1;
			//int idx1 = pa.getDaughter1(i) % 10000;
			//int idx2 = pa.getDaughter2(i) % 10000;
			int l = fillIndexVec(vec, idx1, idx2);
			//System.out.println("child: "+i+" "+idx1+" "+idx2+" "+l);
			for (int j = 0; j < l; j++)
			{
	                        if (j<vec.length)	
                   		checkAndAddDaughter(particles, ancestors, i, vec[j]);
			}
		}

		// System.out.println("Add particles to the collection");
		for (int i=0; i<n; i++)
		{
			// System.out.println(Integer.toString(i)+" pdg="+Integer.toString(particles[i].getPDG()));
			// System.out.println(particles[i].getNumberOfDaughters());
			//System.out.println(particles[i].getNumberOfParents());
			mcpcoll.add(particles[i]);
		}

		return mcpcoll;
	}








	public static void usage()
	{
		System.out.println("promc2lcio: \n  Fonvert ProMC to to LCIO format.\n");
		System.out.println("Usage: \n\n >> java promc2lcio file.promc file.slcio \n");
		System.exit(0);
	}
}
