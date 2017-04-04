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
 * Static class to list HepSim data using URL.
 * Pass Download URL, or info page, or dataset name.
 * @author S.Chekanov
 * 
 */
public class Ls {



	// return available files
	public static void main (String[] args) {

		long tot=0;
                boolean simple_list=false;
                boolean simple_list_url=false;

		if (args.length > 0) {

                String surl = args[0].trim();
                String[] parts = surl.split("%");

                // if reconstructed files, find correct URL
                if (parts.length==2){
                       surl=parts[0];
                } else if (parts.length>2){
                      HepSim.ErrorMessage("HepSim does not support listing multiple datasets at the same time.");
                      System.exit(1);
                  }


                String[] surl_array=HepSim.urlRedirector(surl);
                if (surl_array != null) {
                 surl=surl_array[0];
                 if (surl_array.length>1){
                 System.out.println(HepSim.ANSI_BLUE+"Available mirrors:"+HepSim.ANSI_RESET);
                                for (int j=0; j<surl_array.length; j++){
                                        System.err.print(HepSim.ANSI_BLUE+"["+Integer.toString(j)+"]"+HepSim.ANSI_RESET);
                                        System.out.println("  "+surl_array[j]);
                                }
                  }
                 } // end not null


                // simple listing of files without decoration
                if (args.length==2){
                    String s = args[1].trim();
                    if (s.equals("simple")) simple_list=true;
                    if (s.equals("simple-url")) {simple_list=true; simple_list_url=true;}

                    if (s.equals("simple") == false && s.equals("simple-url") == false) {
                     HepSim.ErrorMessage("HepSim does not recognize second argument. Should be \"simple\" or  \"simple-url\"");
                     System.exit(1);
                    }

                } 

                  
                // create reconstructed-level URL
                if (parts.length==2){
                   surl=surl+"/"+parts[1];
                 }


                if (surl.length()<5) {
                      HepSim.ErrorMessage("Exit! Wrong URL!");
                      System.exit(1);
                    }

                if (!HepSim.isValidURL(surl)) {
                        HepSim.ErrorMessage("First argument is not valid URL. Exit!");
                        System.exit(0);
                };

                    if (simple_list==false) {
                        System.out.println("URL: "+surl);
                        System.out.format("%7s%42s%13s\n","Nr", "File name","     bytes");
                        int nlinse=7+42+13;
                        for (int j=0; j<nlinse; j++) System.out.print("-");
                        System.out.println("");
 

			ArrayList<ArrayList<String>>   ar= HepSim.getArray( surl );
			ArrayList<String> tmp = new ArrayList<String>();
			for (int j=0; j<ar.size(); j++) {
				ArrayList<String> s1= ar.get(j);
				String sfile=(s1.get(0)).trim();
				String sizes=(s1.get(1)).trim();
				long xs = Long.parseLong(sizes);
				tot=tot+xs;
                                System.out.format("%7s%42s%13s\n",Integer.toString(j+1),sfile,sizes);

			};

			double xdd=tot/(1024*1024.0);
			String fileSize=Integer.toString((int)xdd);
                        for (int j=0; j<nlinse; j++) System.out.print("-");
                        System.out.println("");
                        System.out.format("%7s%42s%9s%3s\n","Summary:","Nr of files: "+Integer.toString(ar.size()),fileSize,"MB");

                    } else { // end printing fancy text 
                        ArrayList<ArrayList<String>>   ar= HepSim.getArray( surl );
                        ArrayList<String> tmp = new ArrayList<String>();
                        for (int j=0; j<ar.size(); j++) {
                                ArrayList<String> s1= ar.get(j);
                                String sfile=(s1.get(0)).trim();
                                if (simple_list_url==false)System.out.println(sfile);
                                if (simple_list_url==true) System.out.println(surl+"/"+sfile);

                        };

                     } 


		} else {
                        HepSim.ErrorMessage("Usage: It takes 1 argument:  URL of HepSim data sample! (or) ");
                        HepSim.ErrorMessage(" When 2 arguments: \"[URL] simple\" -  prints file list without decoration");
                        HepSim.ErrorMessage("                   \"[URL] simple-url\" - prints simple file list with URL");
                       System.exit(1);
                }


	}
}
