/* ---------------------------------------------------------------------------
 ** This software is in the public domain, furnished "as is", without technical
 ** support, and with no warranty, express or implied, as to its usefulness for
 ** any purpose.
 **
 **  A library for HEP events storage and processing based on Google's ProtocolBuffers 
 **
 ** Author: S.Chekanov (ANL). chekanov@anl.gov
 ** Copyright  2012 
 ** -------------------------------------------------------------------------*/

package proto;

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

// for NLO records
import pronlo.io.*;


/**
 * 
 Write data structures in sequential order using Google's Prototype Buffer.
 * The same class can be used to read data from ntuples. Each data record inside
 * files is compressed on-fly using zip. A protocol Buffers file is provided
 * which can be used for C++ input. Use the CBook package to create such files
 * in C++.
 * <p>
 * 
 * @author S.Chekanov
 * 
 */
public class FileNLO {

	private FileOutputStream oof = null;
	private FileInputStream iif = null;
	private long nev = 0;
	private long requested_nev = 0;
	static final private int FILE_VERSION = 1;
	private ZipOutputStream zout;
	static final int BUFFER = 2048;
	private byte data[];
	private ZipInputStream zin;
	private ZipFile zipFile;
	private String description = "none";
	private long version = -1;
	private long timestamp = -1;
	private String ProMCDescriptionTxt;
	private String ProMCHeaderTxt;
	private String ProMCTxt;
	private String ProMCStatTxt;
	private String LastModifiedTxt;
	private String LogFile = "not found";
	private ProMCHeaderFile.ProMCHeader recordHeader = null;
	private Map<String, Object> storedProto;
	private Map<String, String> storedString;
	private boolean isURL = false;

	/**
	 * Open a file to write/read objects to/from a file in sequential order. If
	 * "w" option is set, the old file will be removed. Use close() to flash the
	 * buffer and close the file.
	 * 
	 * @param file
	 *            File name
	 * @param option
	 *            Option to create the file. If "w" - write a file (or read)
	 *            file, if "r" only read created file.
	 */
	public FileNLO(String file, String option) {

		nev = 0;
		if (option.equalsIgnoreCase("w")) {
			// System.out.println("Writing: "+file);
			try {
				(new File(file)).delete();
				oof = new FileOutputStream(file);
				zout = new ZipOutputStream(new BufferedOutputStream(oof));
				data = new byte[BUFFER];
				zipFile = null;

				// write file version
				ZipEntry entry = new ZipEntry("version");
				zout.putNextEntry(entry);
				String a = new String(Integer.toString(FILE_VERSION));
				byte[] theByteArray = a.getBytes();
				entry.setSize(theByteArray.length);
				zout.write(theByteArray);
				zout.closeEntry();

			} catch (IOException e) {
				e.printStackTrace();
			}

		} else if (option.equalsIgnoreCase("r")) {

			if (file.startsWith("http") || file.startsWith("ftp")) {

				URL url = null;
				try {
					url = new URL(file);
				} catch (MalformedURLException e) {
					// System.out.println(e.toString());
					System.out.println("Error in accessing the URL="
							+ url.toString());
				}
				try {
					System.out.println("Reading from URL: " + file);
					URLConnection urlConn = url.openConnection();
					urlConn.setDoInput(true);
					urlConn.setUseCaches(false);
					zin = new ZipInputStream(urlConn.getInputStream());
					fetchDescriptionURL();
					isURL = true;

				} catch (MalformedURLException e) {
					System.out.println(e.toString());
				} catch (IOException e) {
					System.out.println(e.toString());
				}

			} else { // normal file

				try {
					isURL = false;
					zipFile = new ZipFile(file);
					iif = new FileInputStream(file);
					zin = new ZipInputStream(iif);

					// get description
					fetchDescription();
					readMeta(); // we overweite nev

				} catch (IOException e) {
					System.err.println("Error in opening=" + file);
					e.printStackTrace();
				}

			} // end normal file

		} else {

			System.err
					.println("Wrong option!. Only \"r\" or \"w\"  is allowed");
		}

	};

	/**
	 * Is this a remote file?
	 * 
	 * @return true if remote
	 */
	public boolean getIsURL() {
		return isURL;
	}

	/**
	 * Get the description from URL file.
	 */
	public void fetchDescriptionURL() {

		storedString = new HashMap<String, String>();
		storedProto = new HashMap<String, Object>();

		ZipEntry zipEntry = null;
		try {
			zipEntry = zin.getNextEntry();
		} catch (IOException e) {
			return;
		}
		LogFile = "none";

		int tot = 0;
		while (zipEntry != null) {

			try {
				String sname = zipEntry.getName();

				if (sname.equals("promc_nevents")) {
                                        System.out.println("Reading metadata.. Wait..");
					Date d = new Date(zipEntry.getTime());
					Format formatter = new SimpleDateFormat(
							"yyyy-MM-dd HH:mm:ss");
					LastModifiedTxt = formatter.format(d);
					String snev = new Scanner(zin).next();
					nev = Integer.parseInt(snev);
					storedString.put(sname, snev);
				} else if (sname.equals("version")) {
					String sversion = new Scanner(zin).next();
					version = Integer.parseInt(sversion);
					storedString.put(sname, sversion);
				} else if (sname.equals("promc_description")) {
					description = getTextFromScanner(zin);
					storedString.put(sname, description);
				} else if (sname.equals("ProMCDescription.proto")) {
					ProMCDescriptionTxt = getTextFromScanner(zin);
					storedString.put(sname, ProMCDescriptionTxt);
				} else if (sname.equals("ProMCHeader.proto")) {
					ProMCHeaderTxt = getTextFromScanner(zin);
					storedString.put(sname, ProMCHeaderTxt);
				} else if (sname.equals("ProMC.proto")) {
					ProMCTxt = getTextFromScanner(zin);
					storedString.put(sname, ProMCTxt);
				} else if (sname.equals("ProMCStat.proto")) {
					ProMCStatTxt = getTextFromScanner(zin);
					storedString.put(sname, ProMCStatTxt);
				} else if (sname.equals("header")) {
					storedProto.put(sname,
							ProMCHeaderFile.ProMCHeader.parseFrom(zin));
				} else if (sname.equals("statistics")) {
					storedProto.put(sname,
							ProMCStatFile.ProMCStat.parseFrom(zin));
				} else if (sname.equals("description")) {
					storedProto.put(sname,
							ProMCDescriptionFile.ProMCDescription
									.parseFrom(zin));
				} else if (sname.equals("logfile.txt")) {

					if (zipEntry != null) {
						LogFile = "logfile.txt";
					    storedString.put(sname, getTextFromScanner(zin)); // too time consuming
					}

				} else {

					storedProto.put(sname, ProMC.ProMCEvent.parseFrom(zin));
					tot++;
				//if (tot % 100 == 0)
				//		System.out.println("Read record="
				//				+ Integer.toString(tot));
				}

				zipEntry = zin.getNextEntry();

			} catch (IOException e) {
			}

		} // end while

		if (tot != nev ) {  
			ErrorMessage("The number records stored in the file is not equal metadata info. The file is corrupted?");
                        return;
                }
              
               System.out.println("Done with the event scan. Wait..");
               return;
 
	}

	/*
	 * Get zipentries using URL stream.
	 */
	public Map<String, Object> getStoredProtoURL() {
		return storedProto;

	}

	/**
	 * Get version of the input file. The version is an integer written as an
	 * additional entry in the file "version". Check this by unzipping the file.
	 * 
	 * @return version of the created file.
	 */

	/*
	 * public int getVersion() {
	 * 
	 * String tmp="0"; if (zipFile == null) return -1; ZipEntry ze =
	 * zipFile.getEntry("version"); long size = ze.getSize(); if (size > 0) { //
	 * System.out.println("Length is " + size); BufferedReader br; try { br =
	 * new BufferedReader( new InputStreamReader(zipFile.getInputStream(ze)));
	 * String line; while ((line = br.readLine()) != null) { tmp=line; }
	 * br.close(); } catch (IOException e) { e.printStackTrace(); }
	 * 
	 * } return Integer.parseInt(tmp);
	 * 
	 * }
	 */

	/**
	 * Read all metadata
	 */
	public void readMeta() {
		ZipEntry entry = zipFile.getEntry("promc_nevents");
		// ZipEntry entry = zin.getEntry("promc_nevents");
		Date d = new Date(entry.getTime());
		Format formatter = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
		LastModifiedTxt = formatter.format(d);
		try {
			InputStream inputstream = zipFile.getInputStream(entry);
			nev = new java.util.Scanner(inputstream).nextLong();
		} catch (IOException e) {

		}

		description = getStringZip("promc_description");
		ProMCDescriptionTxt = getStringZip("ProMCDescription.proto");
		ProMCHeaderTxt = getStringZip("ProMCHeader.proto");
		ProMCTxt = getStringZip("ProMC.proto");
		ProMCStatTxt = getStringZip("ProMCStat.proto");

		ZipEntry en = zipFile.getEntry("logfile.txt");
		if (en != null)
			LogFile = "logfile.txt";
		else
			LogFile = "none";

	}

	/**
	 * Return info about the log file.
	 * 
	 * @return log file name if it is stored.
	 */
	public String getLogfileTxt() {

		return LogFile;
	}

	/**
	 * Return logfile text
	 * 
	 * @return text of the log file
	 */

	public String readLogfile() {

		if (isURL) {
			return storedString.get("logfile.txt");
	     }
					

		ZipEntry en = zipFile.getEntry("logfile.txt");
		if (en != null) return getStringZip("logfile.txt");
		
		return "";
	}

	/**
	 * Last modified
	 * 
	 * @return
	 */
	public String getLastModified() {
		return LastModifiedTxt;
	}

	/**
	 * Return ProMCDescription.proto
	 * 
	 * @return
	 */
	public String getProMCDescriptionTxt() {
		return ProMCDescriptionTxt;
	}

	/**
	 * Return ProMCHeader.proto
	 * 
	 * @return
	 */
	public String getProMCHeaderTxt() {
		return ProMCHeaderTxt;
	}

	/**
	 * Return ProMC.proto.
	 * 
	 * @return
	 */
	public String getProMCTxt() {
		return ProMCTxt;
	}

	/**
	 * Return ProMCStat.proto
	 * 
	 * @return
	 */
	public String getProMCStatTxt() {
		return ProMCStatTxt;
	}

	public long getVersion() {
		return version;
	}

	public String getDescription() {
		return description;
	}

	public long getEvents() {
		return nev;
	}

	public long getRequestedEvents() {
		return requested_nev;
	}

	/**
	 * Open file for reading objects from a serialized file in sequential order.
	 * 
	 * @param file
	 *            File name
	 */
	public FileNLO(String file) {

		this(file, "r");

	};

	/**
	 * Return this zip file.
	 * 
	 * @return
	 */
	public ZipFile getZipFile() {
		return zipFile;

	}

	/**
	 * Add a data structure to a file
	 * 
	 * @param ev
	 *            Data in form HEvent class.
	 * 
	 * @return true if success
	 */
	/*
	 * public boolean write(ProMC.Builder ev) {
	 * 
	 * boolean success = true; nev++; String firec = Integer.toString(nev); try
	 * { data = ev.build().toByteArray(); ZipEntry entry = new ZipEntry(firec);
	 * zout.putNextEntry(entry); entry.setSize(data.length); zout.write(data);
	 * zout.closeEntry(); } catch (IOException e) { e.printStackTrace(); success
	 * = false; return success; // no support }
	 * 
	 * return success;
	 * 
	 * };
	 */

	/**
	 * Get the number of events stored in the file.
	 * 
	 * @return number of stored objects
	 */
	public long size() {
		return nev;
	};

	/**
	 * Get number of events stored in the file. Same as size()
	 * 
	 * @return number of stored objects
	 */
	public long getNEntries() {
		return size();

	};

	/**
	 * Get a string representing file content.
	 * 
	 * @return File content.
	 */
	public String entriesToString() {

		String tmp = "";
		if (iif == null)
			return tmp;

		try {
			ZipEntry ze;
			while ((ze = zin.getNextEntry()) != null) {
				String a = ze.getName();
				if (!a.equals("info"))
					tmp = tmp + a + "\n";
				zin.closeEntry();
			}
			zin.close();

		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}

		return tmp;

	}

	/**
	 * Read next event
	 * 
	 * @return next object.
	 */
	public ProMC.ProMCEvent read() {
		nev++;
		return read(nev);
	}

	/**
	 * Get a header file information
	 * 
	 * @return
	 */
	public ProMCHeaderFile.ProMCHeader getHeader() {

		ProMCHeaderFile.ProMCHeader ob = null;
		ZipEntry entry = null;
		if (isURL) {
			return (ProMCHeaderFile.ProMCHeader) storedProto.get("header");
		} else
			entry = zipFile.getEntry("header");

		if (entry == null)
			return ob;
		InputStream zz = null;
		ProMCHeaderFile.ProMCHeader record = null;
		try {
			zz = zipFile.getInputStream(entry);
			record = ProMCHeaderFile.ProMCHeader.parseFrom(zz);
		} catch (IOException e) {
			e.printStackTrace();
			return ob;
		}

		if (record == null)
			return ob;

		return record;
	};

	/**
	 * Get statistics information.
	 * 
	 * @return
	 */
	public ProMCStatFile.ProMCStat getStat() {
		ProMCStatFile.ProMCStat record = null;
                if (isURL) {
                        return (ProMCStatFile.ProMCStat) storedProto.get("statistics");
                }
		if (zipFile == null) return record;
		ZipEntry entry = zipFile.getEntry("statistics");
		if (entry == null) return record;
		InputStream zz = null;
		try {
			zz = zipFile.getInputStream(entry);
			record = ProMCStatFile.ProMCStat.parseFrom(zz);
		} catch (IOException e) {
			e.printStackTrace();
		}
		return record;
	};

	/**
	 * Get description information.
	 * 
	 * @return
	 */
	public ProMCDescriptionFile.ProMCDescription fetchDescription() {

		ProMCDescriptionFile.ProMCDescription record = null;

		if (isURL) {
			return (ProMCDescriptionFile.ProMCDescription) storedProto
					.get("description");
		}
		if (zipFile == null)
			return record;

		ZipEntry entry = zipFile.getEntry("description");

		if (entry == null)
			return record;
		InputStream zz = null;
		try {
			zz = zipFile.getInputStream(entry);
			record = ProMCDescriptionFile.ProMCDescription.parseFrom(zz);
			description = record.getDescription();
			version = record.getVersion();
			timestamp = record.getTimestamp();
			requested_nev = record.getEvents();
		} catch (IOException e) {
			e.printStackTrace();
		}

		return record;
	};

	/**
	 * Get object from a file using its index.
	 * 
	 * @param index
	 *            of the object
	 * @return Object extracted object (or null)
	 */
	public ProMC.ProMCEvent read(long index) {

		ProMC.ProMCEvent record = null;

		if (isURL) {
			return (ProMC.ProMCEvent) storedProto.get(Long.toString(index));
		}

		ZipEntry entry = zipFile.getEntry(Long.toString(index));
		if (entry == null)
			return record;

		InputStream zz = null;
		try {
			zz = zipFile.getInputStream(entry);
			record = ProMC.ProMCEvent.parseFrom(zz);
		} catch (IOException e) {
			e.printStackTrace();
		}

		return record;
	};

	/**
	 * Read an event using a key
	 * 
	 * @param key
	 *            key
	 * @return
	 */
	public ProMC.ProMCEvent read(String key) {

		ProMC.ProMCEvent record = null;
		if (isURL) {
			return (ProMC.ProMCEvent) storedProto.get(key);
		}

		if (zipFile == null)
			return record;

		ZipEntry entry = zipFile.getEntry(key);

		if (entry == null)
			return record;

		InputStream zz = null;

		try {
			zz = zipFile.getInputStream(entry);
			record = ProMC.ProMCEvent.parseFrom(zz);
		} catch (IOException e) {
			e.printStackTrace();
		}

		return record;
	};

	/**
	 * Close the file
	 * 
	 * @return
	 */
	public boolean close() {

		boolean success = true;
		try {

			if (iif != null) {
				iif.close();
				zin.close();
				zipFile.close();
				iif = null;
				zin = null;
				zipFile = null;
			}

			if (oof != null) {
				zout.finish();
				zout.close();
				oof.flush();
				oof.close();
				oof = null;
			}
			if (zin != null) {
				zin = null;
			}

                  System.gc();

		} catch (IOException e) {
			success = false;
			e.printStackTrace();
		}
		return success;

	};

	/**
	 * Get an entry as A STRING
	 * 
	 * @param key
	 * @return
	 */
	private String getStringZip(String key) {

		String tmp = "";

		if (isURL) {
			tmp = storedString.get(key);
			return tmp;
		}

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
	private String getTextFromScanner(ZipInputStream xzin) {

		String tmp = "";
		Scanner sc = new Scanner(xzin);
		while (sc.hasNextLine()) {
			tmp = tmp + sc.nextLine() + "\n";
		}
		return tmp;

	}

	/**
	 * Generate error message
	 * 
	 * @param a
	 *            Message
	 **/

	private void ErrorMessage(String a) {
		System.out.println(a);

	}

}
