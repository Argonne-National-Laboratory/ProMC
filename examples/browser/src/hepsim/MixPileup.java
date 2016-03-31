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
 *  Mix events with pileups using a fixed and or a poisson distributions.
 *  Events can be mixed using a constant mu, or using a Poisson distribution.
 *  It is also a distiller since zip32 will moved
 *  to zip64 (more than 64k entries). 
 *  The barcode of particle shows the origin of the event. 0 means the signal event, other values show the minbias events (1..N).
 *  @author S.Chekanov (ANL) and Daniel Wilbern. 
 *
 **/

public class MixPileup {

	private static FileOutputStream fout;
	private static ZipOutputStream  zout;
	private static FileMC datafile;
	private static FileMC biasfile;

	// Argument 1: Number of events from bias data file to combine with signal event (prefix with 'p' character for Poisson distributon)
	// Argument 2: Filename of data file
	// Argument 3: Filename of bias data file
	// Argument 4 (optional): Filename of output file
	public static void main (String[] args) {

		// get and process arguments
		if(args.length != 3 && args.length != 4) {
			System.err.println("Error: Incorrect number of arguments. Exit.");
			System.out.println("Argument 1: Number of events from bias data file to combine with signal event.  Prefix with 'p' character for Poisson distribution.\n"
					+ "Argument 2: Filename of data file.\n"
					+ "Argument 3: Filename of bias data file.\n"
					+ "Argument 4 (optional): Filename of output file.");
			System.exit(1);
		}

		boolean poisson = false;
		if(args[0].charAt(0) == 'p')
			poisson = true;
		int num_events = 0;
		try {
			if(poisson)
				num_events = Integer.parseInt(args[0].substring(1));
			else
				num_events = Integer.parseInt(args[0]);
		} catch(NumberFormatException e) {
			System.err.println("Error: Cannot parse event number. It is not integer number! Exit!");
			System.exit(1);
		}

		String outFilename;
		if(args.length == 4)
			outFilename = args[3];
		else
			outFilename = "pileup.promc";

		// open files and streams
		(new File(outFilename)).delete();
		try {
			fout = new FileOutputStream(outFilename);
		} catch(FileNotFoundException e) {
			System.err.println("Error: Cannot open file " + outFilename);
			System.exit(1);
		}

		String txt = "ProMC: Mixing "+Integer.toString(num_events)+ " (const) events";
		if (poisson) txt = "ProMC: Mixing "+Integer.toString(num_events)+ " (Poisson average) events";
		System.out.println(txt);
		System.out.println("  Signal   Input="+args[1].trim());
		System.out.println("  MinBias  Input="+args[2].trim());
		System.out.println("           Output="+outFilename);

		zout = new ZipOutputStream(new BufferedOutputStream(fout));
		FileMC datafile = new FileMC(args[1].trim(),"r");
		FileMC biasfile = new FileMC(args[2].trim(),"r");

		if(!checkFileForConsistency(datafile) || !checkFileForConsistency(biasfile)) {
			closeFiles();
			System.exit(1);
		}

		// write ProMC version no.
		long version = datafile.getVersion();
		writeInfo("version",Long.toString(version).getBytes());

		// write description
		ProMCDescriptionFile.ProMCDescription desc = datafile.fetchDescription();
		ProMCDescriptionFile.ProMCDescription bias = biasfile.fetchDescription();
		writeInfo("description",desc.toByteArray());

		// write header
		ProMCHeaderFile.ProMCHeader header = datafile.getHeader();
		int unit = header.getMomentumUnit();
		int lunit = header.getLengthUnit();
		ProMCHeaderFile.ProMCHeader header_bias = biasfile.getHeader();

		if (unit != header_bias.getMomentumUnit()) {
			System.err.println("Error: Energy units in the signal and MinBias files are inconsistent!");
			System.err.println("Error: Signal file energy unit:"+Integer.toString(unit));
			System.err.println("Error:   Bias file energy unit:"+Integer.toString(header_bias.getMomentumUnit())); 
			closeFiles();
			System.exit(1);

		}

		if (lunit != header_bias.getLengthUnit()) {
			System.err.println("Error: Lenght units in the signal and MinBias files are inconsistent!");
			System.err.println("Error: Signal file length unit:"+Integer.toString(lunit));
			System.err.println("Error:   Bias file length unit:"+Integer.toString(header_bias.getLengthUnit())); 
			closeFiles();
			System.exit(1);

		}

		writeInfo("header",header.toByteArray());

		// loop over events in data file
		long n_data_events = datafile.getNEntries();
		long n_bias_events = biasfile.getNEntries();
		int n_events = num_events;
		int next_particle_id = 0;
		int oldPercentComplete = 0;
		for(int eventNo = 0; eventNo < n_data_events; eventNo++) {
			ProMC.ProMCEvent event = datafile.read(eventNo);
			ProMC.ProMCEvent.Builder builder = event.toBuilder();
			ProMC.ProMCEvent.Particles particles = event.getParticles();
			if(poisson)
				n_events = getPoissonRandom(num_events);

			// print progress periodically
			int percentComplete = (int)((double)eventNo * 100 / n_data_events);
			if(percentComplete % 5 == 0 && percentComplete > oldPercentComplete) {
				System.out.println(Integer.toString(percentComplete) + "% complete");
				oldPercentComplete = percentComplete;
			}

			next_particle_id = 1+particles.getId(particles.getIdCount()-1);

			// loop n_events times, randomly selecting an event from the bias file and combinining
			// it into the builder each time
			for(int i = 1; i <= n_events; i++) {
				int rand = (int)(Math.random()*n_bias_events);
				ProMC.ProMCEvent event2 = biasfile.read(rand);
				ProMC.ProMCEvent.Particles particles2 = event2.getParticles();
				ProMC.ProMCEvent.Particles.Builder builder2 = particles2.toBuilder();

				// update the barcode values of these particles so they may be uniquely identified
				// among particles from other events with the same ID
				// Also, add the total number of particles seen so far to mother and daughter IDs to make them unique
				int count = particles2.getBarcodeCount();
				for(int j = 0; j < count; j++) {
					builder2.setBarcode(j,i);
					builder2.setId(j,particles2.getId(j)+next_particle_id);
					builder2.setMother1(j,particles2.getMother1(j)+next_particle_id);
					builder2.setMother2(j,particles2.getMother2(j)+next_particle_id);
					builder2.setDaughter1(j,particles2.getDaughter1(j)+next_particle_id);
					builder2.setDaughter2(j,particles2.getDaughter2(j)+next_particle_id);
				}
				next_particle_id = 1+builder2.getId(builder2.getIdCount()-1);
				builder.mergeParticles(builder2.build());
			}
			// write the event
			event = builder.build();
			writeInfo(Integer.toString(eventNo), event.toByteArray());
		}

		// write txt description
		String txtdescription = datafile.getDescription() + "  + PILEUP WITH "+Integer.toString(num_events)+ " (const)";
		if (poisson) txtdescription=datafile.getDescription() + "  + PILEUP WITH "+Integer.toString(num_events)+ " (Poisson average)";
		writeInfo("promc_description",txtdescription.getBytes());

		// write number of events
		writeInfo("promc_nevents",Long.toString(n_data_events).getBytes());

		// write statistics
		ProMCStatFile.ProMCStat stat = datafile.getStat();
		writeInfo("statistics",stat.toByteArray());

		// write Google templates
		writeInfo("ProMCHeader.proto",datafile.getProMCHeaderTxt().getBytes());
		writeInfo("ProMC.proto",datafile.getProMCTxt().getBytes());
		writeInfo("ProMCStat.proto",datafile.getProMCStatTxt().getBytes());
		writeInfo("ProMCDescription.proto",datafile.getProMCDescriptionTxt().getBytes());
		writeInfo("logfile.txt",datafile.readLogfile().getBytes());

		closeFiles();

		System.out.println("done");
	}

	// return true if all is well inside the file infile, false otherwise
	static boolean checkFileForConsistency(FileMC infile) {

		if(infile == null) {
			System.err.println("Error: a file wasn't opened properly or is not a ProMC file.");
			return false;
		}

		long nev1 = infile.getNEntries();
		long nev2 = infile.getEvents();

		if(nev1 != nev2) {
			System.err.println("Error: There is a problem with a file: inconsistent number of entries.  The file was truncated!");
			return false;
		}

		return true;
	}

	// close all of the open files and streams
	static void closeFiles() {
		try {
			datafile.close();
			datafile = null;
		} catch(NullPointerException e) {}

		try {
			biasfile.close();
			biasfile = null;
		} catch(NullPointerException e) {}

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
			closeFiles();
			System.exit(1);
		} catch(NullPointerException e) {
			System.err.println(e.getMessage());
		}
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
}

