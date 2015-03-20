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
		if (args.length > 0) {

                String surl = args[0].trim();
                surl=HepSim.urlRedirector(surl);
                if (surl.length()<5) {
                      HepSim.ErrorMessage("Exit!");
                      System.exit(1);
                    }

                if (!HepSim.isValidURL(surl)) {
                        HepSim.ErrorMessage("First argument is not valid URL. Exit!");
                        System.exit(0);
                };

                        System.out.println("HepSim: "+surl);
                        System.out.format("%7s%45s%10s\n","Nr", "File name","size (kB)");
                        int nlinse=7+45+10;
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
                                System.out.format("%7s%45s%10s\n",Integer.toString(j+1),sfile,sizes);

			};

			double xdd=0.001*tot;
			String fileSize=Integer.toString((int)xdd);
                        for (int j=0; j<nlinse; j++) System.out.print("-");
                        System.out.println("");
                        System.out.format("%7s%42s%9s%3s\n","Summary:","Nr of files: "+Integer.toString(ar.size()),fileSize,"MB");
		} else {
                        HepSim.ErrorMessage("Usage: It takes 1 argument:  URL of HepSim data sample!");
                        System.exit(1);
                }


	}
}
