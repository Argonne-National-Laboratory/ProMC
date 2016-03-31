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
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.*;
import java.util.zip.Inflater;

/**
 * 
 * Static class to find a simulated files. 
 * 
 * @author S.Chekanov
 * 
 */
public class Find {


         

	// return available files
	public static void main (String[] args) {

		long tot=0;
		if (args.length > 0) {

                String search = args[0].trim();
                // System.out.println("HepSim: Looking for: "+search);
                HepSim.Init();

                try {
                                // System.out.println("Reading from URL: " + file);
                                String ses=HepSim.hepsim_www+"find.php?a="+search; 
                                URL url = new URL(ses);
                                URLConnection urlConn = url.openConnection();
                                urlConn.setUseCaches(false);

                                BufferedReader in = new BufferedReader(
                                new InputStreamReader(
                                    urlConn.getInputStream()));
                                StringBuilder response = new StringBuilder();
                                String inputLine;
                                while ((inputLine = in.readLine()) != null) { 
                                        inputLine=inputLine.trim();
                                        //if (inputLine.length()>0)
                                         response.append(inputLine+"\n");
                                }
                                in.close();
                                System.out.println(response.toString());
                        } catch (MalformedURLException e) {
                                 HepSim.ErrorMessage("Error: Malformed URL!");
                                 System.exit(1);
                        } catch (IOException e) {
                                HepSim.ErrorMessage("Error: The location was not found! Please check URL!");
                                System.exit(1);
                        }


		} else {
                        HepSim.ErrorMessage("Usage: It takes 1 argument:  search string");
                        System.exit(1);
                }


	}
}
