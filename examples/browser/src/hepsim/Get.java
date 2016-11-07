package hepsim;


import java.io.*;
import java.net.*;
import java.util.*;
import java.text.*;
import java.nio.file.*;


/**
Downloading ProMC files in multiple threads from the primary HepSim library.
Use GetMirrors to check the mirrors.

@author S.Chekanov (ANL)

**/

public class Get {

private static String toDir="";

	/**
	* Start downloading
        * @param inputURL input HepSim URL
        * @param dest destination
	**/
	public Get(final String inputURL, final File dest) {

		URL website = null;
		try {
			website = new URL(inputURL);
			Files.copy(website.openStream(), dest.toPath(), StandardCopyOption.REPLACE_EXISTING);
		} catch (MalformedURLException e) {
			HepSim.ErrorMessage("No such location: "+inputURL);
		} catch (IOException e) {
			HepSim.ErrorMessage("Error for "+dest);
		}



	}



	/**
	Make chunks
	**/
	private static List<List<String[]>> createBatch(List<String[]> originalList,
	                int chunkSize) {
		List<List<String[]>> listOfChunks = new ArrayList<List<String[]>>();
		for (int i = 0; i < originalList.size() / chunkSize; i++) {
			listOfChunks.add(originalList.subList(i * chunkSize, i * chunkSize
			                                      + chunkSize));
		}
		if (originalList.size() % chunkSize != 0) {
			listOfChunks.add(originalList.subList(originalList.size()
			                                      - originalList.size() % chunkSize, originalList.size()));
		}
		return listOfChunks;
	}




	
	
	// main program

	public static void main(String args[]){


		DecimalFormat formatter = new java.text.DecimalFormat("####.##");

		int Nthreads=2; // number of threads for download
		int xlength = args.length;
		// System.out.println(length);
                int nfiles=-1;
                String pattern="";


		if (xlength <1) {
			HepSim.ErrorMessage("Usage: The command takes 1, 2, 3, 4 or 5 arguments:  URL, OUTPUT directory, Nr of threads (optional), Nr of files (optional), pattern. Exit!");
			System.exit(1);

		}

                // output goes to the same name
                if (xlength ==1) {
                   toDir=args[0].trim();

                   // if this is URL, not datasetname, extract the name
                   if (HepSim.isValidURL(toDir)) {
                      String sss[] = toDir.split("/");
                      toDir=sss[sss.length-1];
                      toDir=toDir.replace("/",""); 
                   } 

                }

                // here the output is given explicitly 
                if (xlength>1) {
                   toDir=args[1].trim();
                }


		if (xlength >= 3) {
			System.out.println("HepSim: Downloading using "+args[2].trim()+" threads");
                        try  {
                          Nthreads = Integer.parseInt(args[2]);
                        } catch (NumberFormatException e) {
                           HepSim.ErrorMessage("Error: Cannot parse number of threads. It is not an integer number! Exit!");
                           System.exit(1);
                        }

                        if (Nthreads>6)  { HepSim.ErrorMessage("Too many threads given.  Fall back to 2!"); Nthreads=2; }
		}

               if (xlength >=4 ) {
                        try  {
                           nfiles = Integer.parseInt(args[3]);
                        } catch (NumberFormatException e) {
                           HepSim.ErrorMessage("Error: Cannot parse the number of files for download: Not an integer number! Exit!");
                           System.exit(1);
                        }
                }

                if (xlength == 5) {
                          pattern=args[4].trim(); 
                }

                if (xlength > 5) {
                        HepSim.ErrorMessage("Too many arguments! This command takes 2, 3, 4 or 5 arguments:  URL, OUTPUT directory, Nr of threads (optional), Nr of files (optional), pattern. Exit!");
                        System.exit(1);

                }



		String surl=args[0].trim();
                //System.out.println(surl);
                String[] parts = surl.split("%");

                // if reconstructed files, find correct URL
                if (parts.length==2){
                       surl=parts[0];
                } else if (parts.length>2){ 
                      HepSim.ErrorMessage("HepSim does not support download of multiple datasets at the same time.");
                      System.exit(1);
                  }

                String[] surl_array=HepSim.urlRedirector(surl);
                surl=surl_array[0];
                //System.out.println(surl);
                if (surl_array != null) surl=surl_array[0];

                // create reconstructed-level URL
                if (parts.length==2){
                   surl=surl+"/"+parts[1];
                 }


                // another check
                if (surl.length()<5) {
                      HepSim.ErrorMessage("Exit!");
                      System.exit(1); 
                    }




		if (!HepSim.isValidURL(surl)) {
			HepSim.ErrorMessage("HepSim: This is not valid URL link for data download. Exit!");
			System.exit(0);
		};



		File newDirectory = new File(toDir);
		if(!newDirectory.exists()) {
			boolean result = newDirectory.mkdir();
			if(result) {
				System.err.println("HepSim: The directory "+toDir+" is created!");
			}
		} else {
			System.out.println(HepSim.ANSI_YELLOW+"Warning: The directory \""+toDir+"\" already exists!"+HepSim.ANSI_RESET);
		}




		System.out.println("\nStart downloading in "+Integer.toString(Nthreads)+" threads");
		System.out.println("From: "+surl);
		System.out.println("To:   "+toDir);
                if (pattern.length()>1) System.out.println("Pattern:   "+pattern);

		ArrayList<String[]> data=HepSim.getListArraySizes(surl);


                //find pattern
                ArrayList<String[]>   data_pattern= new ArrayList<String[]>();
                for (int j=0; j<data.size(); j++) {
                        if (pattern.length()>1) {
                            String[] xname=data.get(j);
                            if ( xname[0].indexOf(pattern)>-1) data_pattern.add(data.get(j)); 
                         } else   
                        data_pattern.add(data.get(j));
                }



                // trim list
                ArrayList<String[]>   data_new= new ArrayList<String[]>();


                if (nfiles==-1) System.out.println("Nr files:   "+"All "+Integer.toString(data_pattern.size())+" files will be downloaded");
                else    System.out.println("Nr files:   "+"A subsample with "+Integer.toString(nfiles)+" files will be downloaded");
               

                // trimp
                for (int j=0; j<data_pattern.size(); j++) {
                         if (nfiles>-1)
                                if (j>nfiles-1) break;
                        data_new.add(data_pattern.get(j));
                }


		List<List<String[]>> batchfiles=createBatch(data_new, Nthreads);


		//List<String> fileNames = new ArrayList<String>();
		//fileNames.add("http://mc.hep.anl.gov/asc/hepsim/events/pp/14tev/qcd/qcd_herwigpp_pt1tev/mu140/qcd_herwigpp_pt1tev_mu140_001.promc");
		//fileNames.add("http://mc.hep.anl.gov/asc/hepsim/events/pp/14tev/qcd/qcd_herwigpp_pt1tev/mu140/qcd_herwigpp_pt1tev_mu140_002.promc");


		final int bntot=batchfiles.size();

		// loop over bunches in group of  Nthreads;
		for (int n=0; n<bntot; n++) {

			List<String[]> batch=batchfiles.get(n);
			// System.out.println(batch.size());
			List<String> fileNames = new ArrayList<String>();

			int isdone=(int)(  100.0*(n/(double)bntot)  );

			System.out.print(formatter.format(isdone)+"% -> ");
			for (int j=0; j<batch.size(); j++) {
				String[] dd= batch.get(j);
				fileNames.add(dd[0]);
				System.out.print(dd[0]+" ");
			}
			System.out.println("");

			// if files have correct sizes, skip

			long done=0;
			for (int j=0; j<batch.size(); j++) {
				String[] dd= batch.get(j);
				// System.out.println(dd[0]);
				// System.out.println(dd[1]);
				long onlinesize=Long.parseLong(dd[1]);
				File f = new File(toDir+File.separator+dd[0]);
				if (f.exists()){
					long kilobytes = (f.length() / 1024);
					// System.out.println(kilobytes);
					// 4 bytes is a tolerance between java and du
					if (Math.abs(kilobytes-onlinesize)<5) done++;
				}
			}

			// skip this batch since we have done this
		if (done == Nthreads) {System.out.println("Already done: "); continue; };


			List<Thread> threads = new ArrayList<Thread>();
			for (String name : fileNames) {

				final String NAME=name;
				final String inputURL=surl+"/"+name;
				final String destFile=toDir+File.separator+NAME;

				Thread t = new Thread() {
					           @Override public void run() {
						           File dest= new File(destFile);
						           Get d= new Get(inputURL,dest);
					           }

					           // downloadFile(name, toPath); }
				           };
				t.start();
				threads.add(t);


			}
			for (Thread t : threads) {

				try {
					t.join();
				} catch(InterruptedException e) {
					// this is executed when an exception (in this example InterruptedException) occurs
					HepSim.ErrorMessage("Download was interrupted!");
				}

			}


		} // end running in batches



	} // end main




}
