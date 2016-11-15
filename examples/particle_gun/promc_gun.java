import java.io.*;
import java.util.*;
import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;
import java.nio.file.*;
import proto.*;
import promc.io.*;
import hepsim.*;
import java.util.zip.*;
import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;
import java.util.Random;
import hephysics.particle.LParticle;

/**
 * Single-particle gun.
 * @author S.Chekanov (ANL)
 * @version 1.1
 */
public class promc_gun
{

	private static FileOutputStream fout;
	private static ZipOutputStream  zout;
	private static FileMC datafile;
	public  static float  xmass=0;
	public static float  xcharge=0;
	public static int     xPID=0;


	public static void main(String args[]) throws IOException
	{
		// remind user of correct usage
		if (args.length < 4)
			usage();
		if (args.length == 1 && args[0].equals("-h"))
			usage();

		String outFilename = args[0].trim();
		// momenta in GeV
		double momentum = Double.parseDouble(args[1].trim());

		// particle ID
		xPID = Integer.parseInt(args[2].trim());

		// events to generate
		int Ntot = Integer.parseInt(args[3].trim());

		try {
			fout = new FileOutputStream(outFilename);
			zout = new ZipOutputStream(new BufferedOutputStream(fout));

		} catch(FileNotFoundException e) {
			System.err.println("Error: Cannot open file " + outFilename);
			System.exit(1);
		}


		// random staff
		Random random = new Random();
		double PI2=2*Math.PI;
		double deg= 180 / Math.PI;


       
		String txtdescription="Single particles. PID="+args[2].trim()+" max E(GeV)="+args[1].trim();
                System.out.println(txtdescription); 
                System.out.println("Max events="+Integer.toString(Ntot)); 


		// build a binary description of this file.
		ProMCDescriptionFile.ProMCDescription.Builder b_desc = ProMCDescriptionFile.ProMCDescription.newBuilder();
		b_desc.setVersion(4L);
		b_desc.setEvents(Ntot);
		b_desc.setTimestamp(1L);
		b_desc.setDescription(txtdescription);

		writeInfo("version",Long.toString(4).getBytes());

		ProMCHeaderFile.ProMCHeader.Builder b_header = ProMCHeaderFile.ProMCHeader.newBuilder();
		int unit = 1000*100;
		int lunit = 10000;
		b_header.setId1(xPID);
		b_header.setId2(xPID);
		b_header.setCrossSection(0);
		b_header.setCrossSectionError(0);
		b_header.setLengthUnit(lunit);
		b_header.setMomentumUnit(unit);
		b_header.setName("Single particles");
		b_header.setNameLengthUnit("mm");
		b_header.setNameMomentumUnit("GeV");

		// this keeps map of particle names
		readPDG(b_header);


		ProMCHeaderFile.ProMCHeader header = b_header.build();
		writeInfo("header",header.toByteArray());

		ProMCDescriptionFile.ProMCDescription desc = b_desc.build();
		writeInfo("description",desc.toByteArray());

		int oldPercentComplete = 0;
		for(int eventNo = 0; eventNo < Ntot; eventNo++) {

			// print progress periodically
			int percentComplete = (int)((double)eventNo * 100.0 / Ntot);
			if(percentComplete % 5 == 0 && percentComplete > oldPercentComplete) {
				System.out.println(Integer.toString(percentComplete) + "% complete");
				oldPercentComplete = percentComplete;
			}


			// this is a global event
			ProMC.ProMCEvent.Builder b_event = ProMC.ProMCEvent.newBuilder();

			// metadata of this event
			ProMC.ProMCEvent.Event.Builder event_builder=ProMC.ProMCEvent.Event.newBuilder();

			// particle record
			ProMC.ProMCEvent.Particles.Builder b_particles = ProMC.ProMCEvent.Particles.newBuilder();

			int count=2; // one particle per event
			for(int j = 0; j < count; j++) {

				// make random particle
				LParticle G=new LParticle();
				double theta = Math.acos( 2.0*random.nextDouble() - 1.0 );
				double phi   = PI2 *random.nextDouble();
				double p=momentum*random.nextDouble();
				G.setMass(xmass);
				G.setCharge(xcharge);
				G.setThetaPhiP(theta,phi,p);

				b_particles.addId(j);
				b_particles.addMass((int)(G.mass()*unit));
				b_particles.addMother1(0);
				b_particles.addMother2(0);
				b_particles.addPdgId(xPID);
				b_particles.addPx((int)(G.px()*unit));
				b_particles.addPy((int)(G.py()*unit));
				b_particles.addPz((int)(G.pz()*unit));
				b_particles.addEnergy((int)(G.e()*unit));
				b_particles.addStatus(1);
				b_particles.addWeight(1);
				b_particles.addCharge((int)G.getCharge());
				b_particles.addBarcode(0);
				b_particles.addX(0);
				b_particles.addY(0);
				b_particles.addZ(0);
				b_particles.addT(0);
				b_particles.addDaughter1(0);
				b_particles.addDaughter2(0);
			}
			ProMC.ProMCEvent.Particles particles = b_particles.build();
			ProMC.ProMCEvent.Event meta_event = event_builder.build();


			b_event.setEvent(meta_event);
			b_event.setParticles(particles);


			ProMC.ProMCEvent event = b_event.build();
			writeInfo(Integer.toString(eventNo), event.toByteArray());

		}

                // description
		writeInfo("promc_description",txtdescription.getBytes());


		// write number of events
		writeInfo("promc_nevents",Integer.toString(Ntot).getBytes());


		ProMCStatFile.ProMCStat.Builder b_stat = ProMCStatFile.ProMCStat.newBuilder();
		ProMCStatFile.ProMCStat stat = b_stat.build();
		writeInfo("statistics",stat.toByteArray());

		String s="Not set for this file";
		// write Google templates
		writeInfo("ProMCHeader.proto",s.getBytes());
		writeInfo("ProMC.proto",s.getBytes());
		writeInfo("ProMCStat.proto",s.getBytes());
		writeInfo("ProMCDescription.proto",s.getBytes());
		writeInfo("logfile.txt",s.getBytes());



		closeFiles();

		System.out.println("done");

	}





	public static void usage()
	{
		System.out.println("promc_gun: \n  Create a single-particle with random up to momentum and random phi.\n");
		System.out.println("Usage: \n\n >> java promc_gun output.promc [P(max energy in GeV)] [PID] [Nr_of_events] \n");
		System.exit(0);
	}




	// write a byte array to the zip entry "key" in the output file
	static void writeInfo(String key, byte[] data) {

		try {
			ZipEntry entry = new ZipEntry(key);
			zout.putNextEntry(entry);
			entry.setSize(data.length);
			zout.write(data);
			zout.closeEntry();
		} catch(IOException e) {
			System.err.println("Error: Cannot write zip entry \"" + key + "\"");
			System.exit(1);
		} catch(NullPointerException e) {
			System.err.println(e.getMessage());
		}
	}





	// close all of the open files and streams
	static void closeFiles() {
		try {
			zout.finish();
			zout.close();
			zout = null;
		} catch(IOException e) {
			System.err.println(e.getMessage());
		} catch(NullPointerException e) {}

		try {
			fout.close();
			fout = null;
		} catch(IOException e) {
			System.err.println(e.getMessage());
		} catch(NullPointerException e) {}
	}



	// algorithm from Knuth's Seminumerical Algorithms
	static int getPoissonRandom(double mean) {
		Random r = new Random();
		double L = Math.exp(-mean);
		int k = 0;
		double p = 1.0;
		do {
			p = p * r.nextDouble();
			k++;
		} while (p > L);
		return k - 1;
	}



	/// read PDG files with particle names
	private static void readPDG( ProMCHeaderFile.ProMCHeader.Builder b_header ) throws IOException {
		FileInputStream fis = new FileInputStream(new File("data/particle.tbl"));
		//Construct BufferedReader from InputStreamReader
		BufferedReader br = new BufferedReader(new InputStreamReader(fis));

		int jj=0;
		String line = null;
		while ((line = br.readLine()) != null) {
			line = line.trim();
			if (line.startsWith("//")) continue;
			if (line.length()<2)   continue;
			String[] parts = line.split("\\s+");
			// System.out.println(parts[0]+" "+parts[1]+" "+parts[2]+" "+parts[3]+" "+parts[4]+" "+parts[5]);
			ProMCHeaderFile.ProMCHeader.ParticleData.Builder b_pdg=ProMCHeaderFile.ProMCHeader.ParticleData.newBuilder();
			b_pdg.setId(Integer.parseInt(parts[0]));
			b_pdg.setName(parts[1].trim());
			b_pdg.setMass(Float.parseFloat(parts[3]));
			b_pdg.setWidth(Float.parseFloat(parts[4]));
			b_pdg.setLifetime(Float.parseFloat(parts[5]));
			int charge=Integer.parseInt(parts[2]);
			b_pdg.setCharge( charge  );
			ProMCHeaderFile.ProMCHeader.ParticleData pdg=b_pdg.build();
			jj=jj+1;
			b_header.addParticleData(pdg);
			if (xPID==Integer.parseInt(parts[0])){
			                xmass=Float.parseFloat(parts[3]);
				                xcharge= charge;
			                }
		                }
		                br.close();
		System.out.println("Definition for "+Integer.toString( jj )+ " particldes");
	}

} // end
