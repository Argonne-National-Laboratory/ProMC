/* ---------------------------------------------------------------------------
 ** This software is in the public domain, furnished "as is", without technical
 ** support, and with no warranty, express or implied, as to its usefulness for
 ** any purpose.
 **
 **  A library for HEP events storage and processing based on Google's ProtocolBuffers 
 **
 ** Author: S.Chekanov (ANL). chekanov@anl.gov
 ** Copyright  2014
 ** -------------------------------------------------------------------------*/

package hepsim;

import java.io.*;
import java.text.Format;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Scanner;
import java.util.zip.ZipEntry;
import java.util.zip.ZipFile;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.*;
import java.util.zip.Inflater;

/**
 * 
 * Static class to run external command over sample.
 * This is usually to run over Delphes. 
 * @author S.Chekanov
 * 
 */
public class Exec {



	// return available files
	public static void main (String[] args) {

		String root="";
		String surl=null;
		String mess="";
		String delphes="";
		String delphes_tcl="";
		int   nfiles=-1;

		if (args.length > 0) {

			for (String arg : args) {
				//System.out.println(arg);
				String ss = (arg.toLowerCase()).trim();
				if (ss.startsWith("http")) surl=ss;
				if (ss.indexOf(".root")>-1)  root=ss;
				if (ss.indexOf("delphespromc")>-1)  delphes=arg.trim();
				if (ss.indexOf(".tcl")>-1)  delphes_tcl=arg.trim();
				mess=mess+arg+" ";
			}

			// peak up the number of files to process. Last argument!
			try {
				nfiles = Integer.parseInt(args[args.length-1]);
                                System.out.println("HepSim: Nr of input files limitted to "+Integer.toString(nfiles));
			} catch (NumberFormatException e) {
                                nfiles=-1;
				//do something! anything to handle the exception.
			}



			if (surl == null) {
				System.err.println(HepSim.ANSI_RED+"HepSim: Wrong usage. No URL of HepSim was detected. Exit!"+HepSim.ANSI_RESET);
				System.exit(1);
			}

		} else {
			System.err.println(HepSim.ANSI_RED+"HepSim: It takes arguments for execution. One should be URL of HepSim. Exit!"+HepSim.ANSI_RESET);
			System.exit(1);
		}



		if (!HepSim.isValidURL(surl)) {
			System.err.println(HepSim.ANSI_RED+"HepSim: First argument is not valid URL. Exit!"+HepSim.ANSI_RESET);
			System.exit(0);
		};

		long tot=0;
		System.out.println("HepSim: Looking for "+surl);
		ArrayList<ArrayList<String>>   ar= HepSim.getArray( surl );
		ArrayList<String> tmp = new ArrayList<String>();

		// trim list
		ArrayList<ArrayList<String>>   arr= new ArrayList<ArrayList<String>>();
		for (int j=0; j<ar.size(); j++) {
                         if (nfiles>-1)
                                if (j>nfiles-1) break;
                        arr.add(ar.get(j));
		}

		for (int j=0; j<arr.size(); j++) {
			ArrayList<String> s1= arr.get(j);
			String sfile=(s1.get(0)).trim();
			String sizes=(s1.get(1)).trim();
			long xs = Long.parseLong(sizes);
			tot=tot+xs;
			// System.out.println(sfile+"\t"+sizes);

		};

		double xdd=0.001*tot;
		String fileSize=Integer.toString((int)xdd);
		System.out.println("HepSim: Input: Nr of files="+Integer.toString(arr.size())+"  Total size="+fileSize+" MB");
		System.out.println(HepSim.ANSI_GREEN+"HepSim: Start running.."+HepSim.ANSI_RESET);


		// is this Delphes?
		String theDir="";
		String theDirOut="";
		if (delphes.length()>1 && delphes_tcl.length()>0) {
			theDir="hepsim_input";
			makeDir(theDir);
			theDirOut="hepsim_output";
			makeDir(theDirOut);
		}



		// Download and run over files
		for (int j=0; j<arr.size(); j++) {
			ArrayList<String> s1= arr.get(j);
			String sfile=(s1.get(0)).trim();
			String inputURL=surl+"/"+sfile;
			String destFile=theDir+File.separator+sfile;
			File dest= new File(destFile);
			Get d= new Get(inputURL,dest);


			// Delphes case
			if (delphes.length()>1 && delphes_tcl.length()>0) {
				String base=sfile.replace(".promc","");
				mess=mess.replace(surl,destFile);
				String outputbase=root.replace(".root","");
				String outputroot=root.replace(".root","_"+base+".root");
				mess=mess.replace(root,outputroot); // make new output file  name
				mess=delphes+" "+delphes_tcl+" "+theDirOut+File.separator+outputroot+" "+destFile;

			}

			System.out.println(mess);

			try {
				Process p = Runtime.getRuntime().exec(mess);
				String s;
				BufferedReader stdInput = new BufferedReader(new
				                          InputStreamReader(p.getInputStream()));
				BufferedReader stdError = new BufferedReader(new
				                          InputStreamReader(p.getErrorStream()));

				// read the output from the command
				// System.out.println("Here is the standard output of the command:\n");
				while ((s = stdInput.readLine()) != null) {
					System.out.println(s);
				}

				// read any errors from the attempted command
				while ((s = stdError.readLine()) != null) {
					System.out.println(s);
					// if ((s.trim()).length()>0) System.exit(1);
				}

			}
			catch (IOException e) {
				System.err.println(HepSim.ANSI_RED+"HepSim: Exception happened:"+HepSim.ANSI_RESET);
				e.printStackTrace();
				System.exit(-1);
			}



			// remove downloaded input..
			try{
				if(!dest.delete()){
					System.out.println("HepSim: Cleaning of tmp file has failed. Exit!");
					System.exit(1);
				}
			}catch(Exception e){
                                System.err.println(HepSim.ANSI_RED+"HepSim: Exception happened:"+HepSim.ANSI_RESET); 
				e.printStackTrace();
			}



		}


		// cleanup
		purgeDirectory(new File(theDir));

                System.out.println(HepSim.ANSI_GREEN+"HepSim: End! Output files are in "+theDirOut+HepSim.ANSI_RESET);

	}


	static void purgeDirectory(File dir) {
		for (File file: dir.listFiles()) {
			if (file.isDirectory()) purgeDirectory(file);
			file.delete();
		}
		dir.delete();
	}



	static void purgeDirectoryAndRemove(File dir) {
		for (File file: dir.listFiles()) {
			if (file.isDirectory()) purgeDirectory(file);
			file.delete();
		}
		dir.delete();
	}


	static void makeDir(String theD) {
		File theDir = new File(theD);
		if (!theDir.exists()) {
			boolean result = false;
			try{
				theDir.mkdir();
				result = true;
			} catch(SecurityException se){
				//handle it
			}
			if(result) {
				System.out.println("HepSim: creating directory: " + theDir);
			}
		}
	}



}
