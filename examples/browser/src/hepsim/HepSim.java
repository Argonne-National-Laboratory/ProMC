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
import java.util.*;
import java.util.zip.Inflater;
import java.net.*;

/**
 * 
 * Static class to handle HepSim data using URL.
 * 
 * @author S.Chekanov
 * 
 */
public class HepSim {


public static final String ANSI_RESET = "\u001B[0m";
public static final String ANSI_BLACK = "\u001B[30m";
public static final String ANSI_RED = "\u001B[31m";
public static final String ANSI_GREEN = "\u001B[32m";
public static final String ANSI_YELLOW = "\u001B[33m";
public static final String ANSI_BLUE = "\u001B[34m";
public static final String ANSI_PURPLE = "\u001B[35m";
public static final String ANSI_CYAN = "\u001B[36m";
public static final String ANSI_WHITE = "\u001B[37m";




	static final private int FILE_VERSION = 1;
	static final int BUFFER = 2048;
	private byte data[];
	private static ZipInputStream zin;
	private static ZipFile zipFile;
	private long version = -1;

        // main HepSim URL
        public static String hepsim_www="http://atlaswww.hep.anl.gov/hepsim/";

        public static ArrayList<String> mirrors = new ArrayList<String>();




/**
 * Pings a HTTP URL. This effectively sends a HEAD request and returns <code>true</code> if the response code is in 
 * the 200-399 range.
 * @param surl The HTTP URL to be pinged.
 * @param timeout The timeout in millis for both the connection timeout and the response read timeout. Note that
 * the total timeout is effectively two times the given timeout.
 * @return <code>true</code> if the given HTTP URL has returned response code 200-399 on a HEAD request within the
 * given timeout, otherwise <code>false</code>.
 */
public static boolean ping(String surl, int timeout) {
    // Otherwise an exception may be thrown on invalid SSL certificates:
    surl = surl.replaceFirst("https", "http");

/*
    try {
        HttpURLConnection connection = (HttpURLConnection) new URL(url).openConnection();
        connection.setConnectTimeout(timeout);
        connection.setReadTimeout(timeout);
        connection.setRequestMethod("HEAD");
        int responseCode = connection.getResponseCode();
        return (200 <= responseCode && responseCode <= 399);
    } catch (IOException exception) {
        return false;
    }
*/

     try{
                URL url = new URL(surl);
                HttpURLConnection connection = (HttpURLConnection)url.openConnection();
                //HttpsURLConnection connection = (HttpsURLConnection)url.openConnection();
                connection.setRequestMethod("GET");
                connection.connect();
                int code = connection.getResponseCode();  
                // System.out.println("code: "+code);
                if (code>=200 && code<410) return true; 
            } catch (Exception e) {
                // e.printStackTrace();
                return false;
            }

            return false;
}


     

  
  /**
   * Get the current execution path.
   */
   private static String GetExecutionPath(){
    String absolutePath = (HepSim.class).getProtectionDomain().getCodeSource().getLocation().getPath();
    absolutePath = absolutePath.substring(0, absolutePath.lastIndexOf("/"));
    absolutePath = absolutePath.replaceAll("%20"," "); // Surely need to do this here
    return absolutePath;
    }


         /**
         * Determin available mirrors to read the database.
         */
        static public void Init(){

        String inifile=GetExecutionPath()+File.separator+"mirrors.conf";

        try {
 
        FileInputStream fstream = new FileInputStream(inifile);
        BufferedReader br = new BufferedReader(new InputStreamReader(fstream));
        String strLine;
        //Read File Line By Line
         while ((strLine = br.readLine()) != null)   {
             String xline=strLine.trim();
             if (xline.length()<2)     continue;
             if(xline.startsWith("#")) continue; 
             mirrors.add(xline);
         }
         br.close();
        } catch (IOException e) {
                         mirrors.add(hepsim_www);
                         System.out.println("# No file mirrors.conf file. Use default="+hepsim_www); 
			// e.printStackTrace();
		}


        if (mirrors.size()>0) {
        // ping servers, take only server that respond
        for (int j=0; j<mirrors.size(); j++){
            boolean response=ping((String)mirrors.get(j),30);
            // System.out.println((String)mirrors.get(j)+" "+Boolean.toString(response));
            if (response){
                  hepsim_www=(String)mirrors.get(j);
                  System.out.println("HepSim server: "+hepsim_www); 
                  break;
             } 

        }
       }



        }

	/**
	 * Open a file to write/read objects to/from a file in sequential order. If
	 * "w" option is set, the old file will be removed. Use close() to flash the
	 * buffer and close the file.
	 * 
	 * @param file
	 *            input URL location of data sample.
	 * @return array with file names and sizes.
	 */
	static public  ArrayList<ArrayList<String>>  getArray(String file) {

		String files = "";
		URL url = null;
		if (file.startsWith("http") || file.startsWith("ftp")) {
			try {
				url = new URL(file+"/files.zip");
			} catch (MalformedURLException e) {
				ErrorMessage("Error in accessing the URL="+url.toString());
			}

			try {
				// System.out.println("Reading from URL: " + file);
				URLConnection urlConn = url.openConnection();
				urlConn.setUseCaches(false);
				zin = new ZipInputStream(urlConn.getInputStream());
				files=fetchDescriptionURL("files");
				zin.close();
				zin = null;
			} catch (MalformedURLException e) {
                                 ErrorMessage("Error: Malformed URL!");
                                 System.exit(1);
			} catch (IOException e) {
                                ErrorMessage("Error: The location of data sample was not found! Please check URL!"); 
                                System.exit(1);
				// System.err.println(e.toString());
			}



		} else {
			ErrorMessage("Wrong URL! It should start as http or ftp");
                        System.exit(1);
		}

		String [] res = files.split("\n");
		ArrayList<ArrayList<String>> listOfLists = new ArrayList<ArrayList<String>>();
		for (int j=0; j<res.length; j++) {
                        String xline=res[j].trim();
                        if (xline.length()<2)     continue; 
                        if (xline.startsWith("#")) continue; 

			String[] xfiles = xline.split("\\s+");

			if (xfiles.length <2) continue;
			String sfile=xfiles[1].trim();
			sfile=sfile.replace("\n","");
			String size=xfiles[0].trim();
			ArrayList<String> out = new ArrayList<String>();
			out.add(sfile);
			out.add(size);
			listOfLists.add(out);
		}

                // already sorted on the server site
		// Collections.sort(listOfLists,new ColumnComparator());

		return listOfLists;

	};



	/**
	* Return file list without sizes.
	* @param data
	*            input URL location of data sample.
	* @return list of files on the server without sizes.
	*/
	static public ArrayList<String> getList(String data) {
		ArrayList<ArrayList<String>>   ar= getArray(data);
		ArrayList<String> tmp = new ArrayList<String>();
		for (int j=0; j<ar.size(); j++) {
			ArrayList<String> s1= ar.get(j);
			tmp.add(s1.get(0));
		};
		return tmp;
	}


       /**
        * Return list with unique integer values from a given filelist. The names are identified by start and end strings.
        * This is useful when using different pT and mass ranges. The output list will be sorted in increasing order. 
        * @param list initial list 
        * @param  start reg. expression for start string
        * @param  end   reg expression for end  
        * @return list of pT or mass ranges 
        */
        static public List<Integer> getRanges(ArrayList<String> list, String start, String end) {
                // ArrayList<String> tmp = new ArrayList<String>();
                Set<String> tmp = new  HashSet<String>();

                for (int j=0; j<list.size(); j++) {
                        String s1= list.get(j);
                        String[] parts1 = s1.split(start);
                        if (parts1.length>1) {
                                String s2= parts1[1]; 
                                String[] parts2 = s2.split(end);
                                if (parts2.length>1) tmp.add(parts2[0]); 
                              }
                };

                List<String>  slist = new ArrayList<String>(tmp);
                List<Integer> ilist = new ArrayList<Integer>();
                for (int j=0; j<slist.size(); j++) {

                       String sval=slist.get(j);
                       int foo=0;
                       try {
                         foo = Integer.parseInt(sval);
                          } catch (NumberFormatException e) {
                            System.err.println("Error converting string:"+sval+" to integer");
                           }

                       ilist.add(new Integer(foo));
                }

                Collections.sort(ilist);
                return ilist;
        }


	/**
	* Return file list with sizes.
	* @param data
	*            input URL location of data sample.
	* @return list of files on the server including file sizes.
	*/
	static public ArrayList<String[]> getListArraySizes(String data) {
		ArrayList<ArrayList<String>>   ar= getArray(data);
		ArrayList<String[]> tmp = new ArrayList<String[]>();
		for (int j=0; j<ar.size(); j++) {
			ArrayList<String> s1= ar.get(j);
			String[] tt = new String[2];
			tt[0]=s1.get(0);
			tt[1]=s1.get(1);
			tmp.add(tt);
		};
		return tmp;
	}



	/**
	*  Return file list with sizes.
	* 
	* @param data 
	*            input URL location of data sample.
	* @return list with file names and their sizes.
	*/
	static public ArrayList<ArrayList<String>> getListSizes(String data) {
		return getArray(data);
	}


        /**
        Veryfy URL
        **/
        static public boolean isValidURL(String url) {

                URL u = null;

                try {
                        u = new URL(url);
                } catch (MalformedURLException e) {
                        return false;
                }

                try {
                        u.toURI();
                } catch (URISyntaxException e) {
                        return false;
                }

                return true;
        }



	/**
	 * Get the description from URL file.
	        * @param key a key to fetch 
	 */
	private static  String fetchDescriptionURL(String key) {

		String tmp="";
		try {
			ZipEntry zipEntry = zin.getNextEntry();

			while (zipEntry != null) {
				String sname = zipEntry.getName();
				if (sname.equals(key)) {
					tmp= getTextFromScanner(zin);
					// tmp=tmp.replace("./","");
				}

				zin.closeEntry();
				zipEntry = zin.getNextEntry();

			} // end while

			zin.close();

		} catch (IOException e) {
			ErrorMessage("Error in openning the file?");
		}


		return tmp;

	}




	/**
	 * Get an entry as A STRING
	 * 
	 * @param key
	 * @return
	 */
	static private String getStringZip(String key) {

		String tmp = "";

		if (zipFile == null)
			return tmp;

		ZipEntry entry = null;
		try {
			entry = zipFile.getEntry(key);

			if (entry != null) {
				InputStream inputstream = zipFile.getInputStream(entry);
				Scanner sc = new Scanner(inputstream);
				while (sc.hasNextLine()) {
					tmp = tmp + sc.nextLine() + "\n";
				}
			}
		} catch (IOException e) {
			tmp = "Self-description " + key + " is not found";
		}

		return tmp;
	}

	/**
	 * Get a text from the scanner.
	 * 
	 * @param xzin
	 * @return
	 */
	static private String getTextFromScanner(ZipInputStream xzin) {
		String tmp = "";
		Scanner sc = new Scanner(xzin);
		while (sc.hasNextLine()) {
			tmp = tmp + sc.nextLine() + "\n";
		}
		return tmp;

	}


        /**
         * Smart URL redirector.
         * 
         * @param surl URL of info page, or dataset name 
         * @return list of final download URL. First URL is the main one.  
         */
        static public String[] urlRedirector(String surl){

                // identify URL links using the search
                // http://atlaswww.hep.anl.gov/hepsim/geturl.php?name=higgs_ttbar_mg5
                //http://atlaswww.hep.anl.gov/hepsim/geturl.php?item=2

                // apply redirection, if info page is used 
                // this finds based on info page

                Init();
                //System.out.println("Call urlRedirector="+surl);

                // if this is HTTP, just get the main
                 if (surl.indexOf("http:")>-1 && surl.indexOf("info.php?")<0) {
                    //System.out.println("Direct call to "+surl);
                    return new String[]{surl};
                 }

                int smart=0;
                if (surl.indexOf("info.php?")>-1) {
                   surl=surl.replace("info.php?","geturl.php?");
                   smart=1;
                 }

                 // based on names of the dataset 
                 if (surl.indexOf("http")<0 && surl.indexOf("info.php?")<0) {
                   surl=hepsim_www+"/geturlmirrors.php?name="+surl;
                   smart=2;
                 }


                // fetch correct URL based on how smart you are
                if (smart>0) {
                 try {
                    URL url = new URL(surl);
                    // System.out.println(surl);
                    BufferedReader in = new BufferedReader(new InputStreamReader(url.openStream()));
                    String line;
                    String NEWURL="";
                    while ((line = in.readLine()) != null) {
                         NEWURL=line;
                    } in.close();

                    //System.out.println("NEW:"+NEWURL);
                    NEWURL=NEWURL.trim();
                    NEWURL=NEWURL.replace("\n","");
                    surl=NEWURL;
                    String[] parts =  surl.split(";");


                    if (parts.length==0) {
                      if (smart==2) ErrorMessage("Name of the dataset was not found.");
                      if (smart==1) ErrorMessage("URL to the Info page cannot find the dataset.");
                      return null; 
                    } 
                    return parts;
                }
                catch (MalformedURLException e) {
                    ErrorMessage("Connection problem with the HepSim repository."); 
                    System.out.println("MalformedURLException: "+e.getMessage());
                    return null; 

                }
                catch (IOException e) {
                    ErrorMessage("Connection problem with the HepSim repository.");   
                    System.out.println("I/O Error: " + e.getMessage());
                    return null; 
                }
                }

                return  null;

        }


	/**
	 * Generate error message. 
	 * 
	 * @param a
	 *            Message
	 **/

	static public void ErrorMessage(String a) {
		System.err.println(ANSI_RED+a+ANSI_RESET);

	}

	/*
	   public static class ColumnComparator implements Comparator<ArrayList<String[]>>{
	    public int compare(ArrayList<String[]> ar1, ArrayList<String[]> ar2){
	        String[] s1=ar1.get(0);
	        String[] s2=ar2.get(0);
	        return s1[0].compareTo(s2[0]);
	    }
}
	*/




	static private class ColumnComparator implements Comparator<ArrayList<String>>{
		public int compare(ArrayList<String> ar1, ArrayList<String> ar2){
			return ar1.get(0).compareTo(ar2.get(0));
		}
	}

	// return available files
	public static void main (String[] args) {

		long tot=0;
		if (args.length > 0) {

			System.out.println("File name                size (kB)");
			ArrayList<ArrayList<String>>   ar= getArray( args[0] );
			ArrayList<String> tmp = new ArrayList<String>();
			for (int j=0; j<ar.size(); j++) {
				ArrayList<String> s1= ar.get(j);

				String sfile=(s1.get(0)).trim();
				String sizes=(s1.get(1)).trim();
				long xs = Long.parseLong(sizes);
				tot=tot+xs;
				System.out.println(sfile+"\t"+sizes);

			};

			double xdd=0.001*tot;
			String fileSize=Integer.toString((int)xdd);
			System.out.println(ANSI_GREEN+"-> Summary: Nr of files="+Integer.toString(ar.size())+"  Total size="+fileSize+" MB"+ANSI_RESET);
		}

	}
}
