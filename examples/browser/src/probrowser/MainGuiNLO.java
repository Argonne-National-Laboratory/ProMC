package probrowser;

/**
 * Main GUI Class for the NLO browser.
 * This program is open source and licensed using the GNU Public license
 * 
 * @author S.Chekanov
 */

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

import probrowser.editor.SimpleEditor;
import pronlo.io.*;
import proto.*;
import javax.swing.table.*;
import javax.swing.filechooser.FileNameExtensionFilter;

/**
 * Main class for the browser to look at NLO event records.
 * 
 * @author S.Chekanov
 * 
 */
public class MainGuiNLO {

	private JFrame frame;
	private JMenuBar menuBar;
	private JSplitPane jSplit;
	private JScrollPane jPan1;
	private JScrollPane jPan2;
	private JPanel jPanelStatus;
	private MemoryMonitor memMon;
	private JList<Object> listevents;
	private JPopupMenu popupMenu;
	private JMenuItem jpop1, jpop2;
	private ArrayList<String> events;
	private FileNLO file;
	private ProMCHeaderFile.ProMCHeader header;
	private ProMCStatFile.ProMCStat stat;
	private JTable table;
	private Map<Integer, Double> mass_map = new HashMap<Integer, Double>();
	private Map<Integer, String> name_map = new HashMap<Integer, String>();
	private MyTableModelInfo model;
	private int unit, lunit;
	private JLabel statusBar;
	private long version = -1;
	private String current_event = "";
	private String mess = "";
	private long ev_current = -1;

	/**
	 * Build a frame and read the NLO file.
	 * 
	 * @param xfile
	 *            Input ProNLO File
	 **/
	public MainGuiNLO(String f) {

		current_event = "";
		model = new MyTableModelInfo();
		table = new JTable(model) {
			public static final long serialVersionUID = 125;

			public Component prepareRenderer(TableCellRenderer renderer,
					int rowIndex, int vColIndex) {
				Component c = super.prepareRenderer(renderer, rowIndex,
						vColIndex);
				Color colcell = new Color(240, 255, 255);
				if (rowIndex % 2 == 0 && !isCellSelected(rowIndex, vColIndex)) {
					c.setBackground(colcell);
				} else {
					// If not shaded, match the table's background
					c.setBackground(getBackground());
				}
				// column
				c.setForeground(Color.black);
				if (isCellSelected(rowIndex, vColIndex)) {
					c.setBackground(Color.blue);
					c.setForeground(Color.white);
				}
				return c;
			}
		};

		table.setFillsViewportHeight(true);
		table.setAutoCreateRowSorter(true);
                // table.setAutoResizeMode( JTable.AUTO_RESIZE_OFF );

		// RowSorter<TableModel> sorter =
		// new TableRowSorter<TableModel>(model);
		// table.setRowSorter(sorter);

		table.setShowHorizontalLines(true);
		table.setShowVerticalLines(true);
		table.setRowSelectionAllowed(true);
		table.setColumnSelectionAllowed(false);

		listevents = new JList<Object>();
		MouseListener mouseListener = new MouseAdapter() {
			public void mouseClicked(MouseEvent mouseEvent) {
				JList theList = (JList) mouseEvent.getSource();

				if (SwingUtilities.isRightMouseButton(mouseEvent)
						&& !theList.isSelectionEmpty()
						&& theList.locationToIndex(mouseEvent.getPoint()) == theList
								.getSelectedIndex()) {
					int index = theList.locationToIndex(mouseEvent.getPoint());
					Object o = theList.getModel().getElementAt(index);
					String key = o.toString();
					ev_current = Long.parseLong(key) - 1;
					popupMenu.show(theList, mouseEvent.getX(),
							mouseEvent.getY());
				}

				if (mouseEvent.getClickCount() == 2) {
					int index = theList.locationToIndex(mouseEvent.getPoint());
					if (index >= 0) {
						Object o = theList.getModel().getElementAt(index);
						String key = o.toString();
						ev_current = Long.parseLong(key) - 1;
						ProMC.ProMCEvent s = file.read(ev_current);
						// System.out.println("Read event: " + key);
						current_event = "  Event=" + key;
						showEventParticles(s);
					}
				}
			};
		};

		listevents.addMouseListener(mouseListener);

		jpop1 = new JMenuItem("EventInfo");
		jpop2 = new JMenuItem("Particles");
		popupMenu = new JPopupMenu();
		popupMenu.add(jpop1);
		popupMenu.add(jpop2);
		jpop1.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				statusBar.setText("Show EventInfo");
				current_event = " Event=" + Long.toString(ev_current + 1);
				ProMC.ProMCEvent s = file.read(ev_current);
				showEventInfo(s);
			}
		});

		jpop2.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ev) {
				statusBar.setText("Show Particles");
				current_event = " Event=" + Long.toString(ev_current + 1);
				ProMC.ProMCEvent s = file.read(ev_current);
				showEventParticles(s);
			}
		});

		statusBar = new JLabel("No file opened");
		if (f != null)
			openFile(f);

		frame = new JFrame("ProNLO Browser: ProMC for NLO");
		Dimension res = Toolkit.getDefaultToolkit().getScreenSize();
		Dimension size = new Dimension(Math.min(400, res.width), Math.min(400,
				res.height));
		frame.setSize(size);
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// Creates a menubar for a JFrame
		menuBar = new JMenuBar();
		frame.setJMenuBar(menuBar);
		JMenu fileMenu = new JMenu("File");
		JMenu aboutMenu = new JMenu("Help");
		JMenu metaMenu = new JMenu("MetaData");
		JMenu protoMenu = new JMenu("Data layout");

		menuBar.add(fileMenu);
		menuBar.add(metaMenu);
		menuBar.add(protoMenu);
		menuBar.add(aboutMenu);

		JMenuItem item00 = new JMenuItem(new OpenAction());
		fileMenu.add(item00);
		JMenuItem item01 = new JMenuItem(new ExitAction());
		fileMenu.add(item01);
		JMenuItem item11 = new JMenuItem(new ShowAboutAction());
		aboutMenu.add(item11);
		JMenuItem item21 = new JMenuItem(new ShowHeaderAction());
		metaMenu.add(item21);
		JMenuItem item22 = new JMenuItem(new ShowParticleDataAction());
		metaMenu.add(item22);
		JMenuItem item23 = new JMenuItem(new ShowStatisticsAction());
		metaMenu.add(item23);
		JMenuItem item24 = new JMenuItem(new ShowLogFileAction());
		metaMenu.add(item24);

		JMenuItem item31 = new JMenuItem(new ShowDescriptionProtoAction());
		protoMenu.add(item31);
		JMenuItem item32 = new JMenuItem(new ShowHeaderProtoAction());
		protoMenu.add(item32);
		JMenuItem item33 = new JMenuItem(new ShowEventProtoAction());
		protoMenu.add(item33);
		JMenuItem item34 = new JMenuItem(new ShowStatProtoAction());
		protoMenu.add(item34);

		jSplit = new JSplitPane();
		jSplit.setDividerLocation(0.3);

		jPan1 = new JScrollPane(listevents);
		jPan1.setPreferredSize(new java.awt.Dimension(60, 400));
		jPan1.setMinimumSize(new java.awt.Dimension(60, 400));

		jPan2 = new JScrollPane(table);
		jSplit.setLeftComponent(jPan1);
		jSplit.setRightComponent(jPan2);

		memMon = new MemoryMonitor();
		memMon.setPreferredSize(new java.awt.Dimension(65, 18));
		memMon.setMinimumSize(new java.awt.Dimension(23, 10));

		jPanelStatus = new JPanel();
		jPanelStatus.setPreferredSize(new Dimension(700, 20));
		jPanelStatus.setLayout(new BorderLayout());
		jPanelStatus.setBorder(new javax.swing.border.EtchedBorder());
		jPanelStatus.add(statusBar, BorderLayout.WEST);
		jPanelStatus.add(memMon, BorderLayout.EAST);

		frame.getContentPane().add(jSplit, java.awt.BorderLayout.CENTER);
		frame.getContentPane().add(jPanelStatus, java.awt.BorderLayout.SOUTH);

		frame.pack();
		frame.setVisible(true);

	}

	// open file
	private void openFile(String f) {

		file = new FileNLO(f);
		version = file.getVersion();
		// System.out.println("ProMC Version: "+Long.toString(version));
		// System.out.println("Description  : "+file.getDescription());
		// System.out.println("Nr of events=: "+Long.toString(file.getEvents()));
		mess = "ProMC version=" + Long.toString(version) + " Total events="
				+ Long.toString(file.getEvents()); // + " Info="
		// + file.getDescription();
		// System.out.println("####  ProMC Header Information ####");
		header = file.getHeader();
		unit = header.getMomentumUnit();
		lunit = header.getLengthUnit();
		// System.out.println("Momentum unit: "+Integer.toString(unit));
		// System.out.println("Length   unit: "+Integer.toString(lunit));
		// System.out.println("PID1: "+Integer.toString(header.getId1()));
		// System.out.println("PID2: "+Integer.toString(header.getId2()));
		// System.out.println("Cross section: "+Double.toString(header.getCrossSection()));
		// System.out.println("Error: "+Double.toString(header.getCrossSectionError()));

		// use it if you need to use mass
		for (int j = 0; j < header.getParticleDataCount(); j++) {
			pronlo.io.ProMCHeaderFile.ProMCHeader.ParticleData d = header
					.getParticleData(j);
			int pid = d.getId();
			double mass = d.getMass();
			String name = d.getName();
			mass_map.put(new Integer(pid), mass);
			name_map.put(new Integer(pid), name);
		}

		// System.out.println("####  ProMC Statistics Information ####");
		stat = file.getStat();
		// System.out.println("Cross section (pb): "+Double.toString(stat.getCrossSectionAccumulated()));
		// System.out.println("Error: "+Double.toString(stat.getCrossSectionErrorAccumulated()));
		// System.out.println("Luminosity (pb-1): "+Double.toString(stat.getLuminosityAccumulated()));

		model = new MyTableModelInfo();
		model.addColumn("Description");
		model.addColumn("Value");

		model.addRow(new Object[] { new String("ProMC Version"),
				getLong(version) });
		model.addRow(new Object[] { new String("Description"),
				getString(file.getDescription()) });
		model.addRow(new Object[] { new String("Last modified"),
				getString(file.getLastModified()) });
		model.addRow(new Object[] { new String("Attached logfile"),
				getString(file.getLogfileTxt()) });
		model.addRow(new Object[] { new String("Requested events"),
				new Long(file.getRequestedEvents()) });
		model.addRow(new Object[] { new String("Recorded events"),
				new Long(file.getEvents()) });
		if (header != null) {
			model.addRow(new Object[] { new String("eCM"),
					getDouble(header.getECM()) });
			model.addRow(new Object[] { new String("S"),
					getDouble(header.getS()) });
			model.addRow(new Object[] { new String("Process"),
					getString(header.getName()) });
			model.addRow(new Object[] { new String("Code"),
					getInt(header.getCode()) });
			model.addRow(new Object[] { new String("Momentum unit (GeV*unit)"),
					getInt(unit) });
			model.addRow(new Object[] { new String("Length   unit (cm*unit)"),
					getInt(lunit) });
			model.addRow(new Object[] { new String("id1"),
					getInt(header.getId1()) });
			model.addRow(new Object[] { new String("id2"),
					getInt(header.getId2()) });
			model.addRow(new Object[] { new String("pdf_id1"),
					getInt(header.getPdfId1()) });
			model.addRow(new Object[] { new String("pdf_id2"),
					getInt(header.getPdfId2()) });
			model.addRow(new Object[] { new String("x1"),
					getDouble(header.getX1()) });
			model.addRow(new Object[] { new String("x2"),
					getDouble(header.getX2()) });
			model.addRow(new Object[] { new String("ScalePDF"),
					getDouble(header.getScalepdf()) });
			model.addRow(new Object[] { new String("pdf1"),
					getInt(header.getPdf1()) });
			model.addRow(new Object[] { new String("pdf2"),
					getInt(header.getPdf2()) });
		}

		if (stat != null) {
			model.addRow(new Object[] { new String("Cross section       (pb)"),
					getDouble(stat.getCrossSectionAccumulated()) });
			model.addRow(new Object[] { new String("Cross section error (pb)"),
					getDouble(stat.getCrossSectionErrorAccumulated()) });
			model.addRow(new Object[] { new String("Luminosity          (pb)"),
					getDouble(stat.getLuminosityAccumulated()) });
		}

		table.setModel(model);

		events = new ArrayList<String>();
		for (long i = 0; i < file.size(); i++) {
			// ProMC.ProMCEvent s = file.read(i);
			// if (i%100==0) System.out.println("Read event="+Long.toString(i));
			events.add(new String(Long.toString(i + 1)));
		}
		statusBar.setText(mess);
		listevents.setListData(events.toArray());

	}

	// formatted output
	private Double getDouble(double d) {

		if (d != 0) {
			return new Double(d);
		}
		return -1.0;
	}

	// formatted output
	private Integer getInt(int d) {

		if (d != 0) {
			return new Integer(d);
		}
		return -1;
	}

	// formatted output
	private Long getLong(long d) {

		if (d != 0) {
			return new Long(d);
		}
		return -1L;
	}

	// formatted output
	private String getString(String d) {

		if (d != null) {
			return new String(d);
		}
		return " ";
	}

	// show particle info
	private void showEventInfo(ProMC.ProMCEvent s) {

		statusBar.setText(mess + " " + current_event);
		ProMC.ProMCEvent.Event ev = s.getEvent(); // event
		model = new MyTableModelInfo();
		model.addColumn("Description");
		model.addColumn("Value");

                model.addRow(new Object[] { new String("Number"),
                        getInt(ev.getNumber()) });

		model.addRow(new Object[] { new String("Process_ID"),
			getInt(ev.getProcessID()) });

                model.addRow(new Object[] { new String(" "),
                         new String(" ") });
                model.addRow(new Object[] { new String("Array (long values): "),
                         new String(" ") });
                String idata="";
                for (int i=0; i<ev.getIdataCount(); i++){
                   model.addRow(new Object[] { new String("Idata ["+Integer.toString(i)+"]"),
                        Long.toString( ev.getIdata(i) ) });

                }

                model.addRow(new Object[] { new String(" "),
                         new String(" ") });

                model.addRow(new Object[] { new String("Array with float values: "),
                         new String(" ") });

                for (int i=0; i<ev.getFdataCount(); i++){
                   model.addRow(new Object[] { new String("Fdata ["+Integer.toString(i)+"]"),
                        Float.toString( ev.getFdata(i) ) });
                }

       

		table.setModel(model);

	}

	// show particle info
	private void showEventParticles(ProMC.ProMCEvent s) {

		statusBar.setText(mess + " " + current_event);
		ProMC.ProMCEvent.Particles pa = s.getParticles(); // true particles
		model = new MyTableModelInfo();
		model.addColumn("No");
		model.addColumn("Name");
		model.addColumn("PID");
		model.addColumn("Px (GeV)");
		model.addColumn("Py (GeV)");
		model.addColumn("Pz (GeV)");
                model.addColumn("E (GeV)");
		model.addColumn("M (GeV)");
		model.setRowCount(0);
		table.setModel(model);

                // fix before v1.3  when energy was not stored
                boolean energyM=false;
                boolean statusM=false;
                boolean mM=false;
                boolean pidM=false;
                boolean pxM=false;
                boolean pyM=false;
                boolean pzM=false;
 
                if (pa.getEnergyCount()>1) energyM=true;
                if (pa.getPdgIdCount()>1)   pidM=true;
                if (pa.getPxCount()>1)   pxM=true;
                if (pa.getPyCount()>1)   pyM=true;
                if (pa.getPzCount()>1)   pzM=true;
 

		if (pa != null) {

			for (int j = 0; j < pa.getPxCount(); j++) {

			// check. Is this HEPMC?
			//try {
		         //		pa.getStatus(j);
			//	} catch (NoSuchMethodError | IndexOutOfBoundsException e) {
			//		break;
			//	}

				int pid = 0; 
                                if (pidM) pid=pa.getPdgId(j);


				String name = "none";
				if (name_map.containsKey(pid))
					name = name_map.get(pid);

       
				double m =0;
				if (mass_map.containsKey(pid))
				           m = mass_map.get(pid); // get from the header

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


				model.addRow(new Object[] { new Integer(j + 1),
						new String(name), new Integer(pid),
						new Double(px), 
						new Double(py), new Double(pz), new Double(ee), new Double(m),
						});

			}
		} // not null

	}

	private class ShowAboutAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ShowAboutAction() {
			super("About");
		}

		public void actionPerformed(ActionEvent e) {
			new AboutDialog(frame);
		}
	}

	private class ExitAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ExitAction() {
			super("Exit");
		}

		public void actionPerformed(ActionEvent e) {
			frame.setVisible(false);
			frame.dispose();

		}
	}

	private class OpenAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		OpenAction() {
			super("Open file");
		}

		public void actionPerformed(ActionEvent e) {

			JFileChooser chooser = new JFileChooser();
			chooser.setCurrentDirectory(new File("."));
			FileNameExtensionFilter filterm3u = new FileNameExtensionFilter(
					"ProMC file (.promc)", "promc");
			chooser.addChoosableFileFilter(filterm3u);

			try {
				int returnVal = chooser.showOpenDialog(frame);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					String fullPath = chooser.getSelectedFile()
							.getAbsolutePath();
					if (fullPath != null)
						openFile(fullPath);
				}

			} catch (Exception f) {
				f.printStackTrace();
				System.out.println("Error saving file.");
			}

		}
	}

	private class ShowHeaderAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ShowHeaderAction() {
			super("Header Record");
		}

		public void actionPerformed(ActionEvent e) {

			model = new MyTableModelInfo();
			model.addColumn("Description");
			model.addColumn("Value");
			model.addRow(new Object[] { new String("ProMC Version"),
					getLong(version) });
			model.addRow(new Object[] { new String("Description"),
					new String(file.getDescription()) });
			model.addRow(new Object[] { new String("Requested events"),
					getLong(file.getRequestedEvents()) });
			model.addRow(new Object[] { new String("Recorded events"),
					getLong(file.getEvents()) });
			if (header != null) {
				model.addRow(new Object[] { new String("eCM"),
						getDouble(header.getECM()) });
				model.addRow(new Object[] { new String("S"),
						getDouble(header.getS()) });
				model.addRow(new Object[] { new String("Process"),
						getString(header.getName()) });
				model.addRow(new Object[] { new String("Code"),
						getInt(header.getCode()) });
				model.addRow(new Object[] { new String("Momentum unit"),
						getInt(unit) });
				model.addRow(new Object[] { new String("Length   unit"),
						getInt(lunit) });
				model.addRow(new Object[] { new String("id1"),
						getInt(header.getId1()) });
				model.addRow(new Object[] { new String("id2"),
						getInt(header.getId2()) });
				model.addRow(new Object[] { new String("pdf_id1"),
						getInt(header.getPdfId1()) });
				model.addRow(new Object[] { new String("pdf_id2"),
						getInt(header.getPdfId2()) });
				model.addRow(new Object[] { new String("x1"),
						getDouble(header.getX1()) });
				model.addRow(new Object[] { new String("x2"),
						getDouble(header.getX2()) });
				model.addRow(new Object[] { new String("ScalePDF"),
						getDouble(header.getScalepdf()) });
				model.addRow(new Object[] { new String("pdf1"),
						getInt(header.getPdf1()) });
				model.addRow(new Object[] { new String("pdf2"),
						getInt(header.getPdf2()) });
				model.addRow(new Object[] {
						new String("Cross section       (pb)"),
						getDouble(header.getCrossSection()) });
				model.addRow(new Object[] {
						new String("Cross section error (pb)"),
						getDouble(header.getCrossSectionError()) });
			}
			// model.addRow( new Object[]{ new String("Weight"), new
			// Integer(header.getWeight()) });
			table.setModel(model);

		}
	}

	private class ShowParticleDataAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ShowParticleDataAction() {
			super("Particle Data");
		}

		public void actionPerformed(ActionEvent e) {

			model = new MyTableModelInfo();
			model.addColumn("Nr");
			model.addColumn("PID");
			model.addColumn("Name");
			model.addColumn("Mass (GeV)");
			for (int j = 0; j < header.getParticleDataCount(); j++) {
				pronlo.io.ProMCHeaderFile.ProMCHeader.ParticleData d = header
						.getParticleData(j);
				String name = d.getName();
				model.addRow(new Object[] { new Integer(j + 1),
						new Integer(d.getId()), new String(name),
						new Double(d.getMass()) });
			}
			table.setModel(model);

		}
	}

	private class ShowStatisticsAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ShowStatisticsAction() {
			super("Statistics");
		}

		public void actionPerformed(ActionEvent e) {

			model = new MyTableModelInfo();
			model.addColumn("Description");
			model.addColumn("Value");

			if (stat != null) {
				model.addRow(new Object[] { new String("Events tried"),
						new Integer(stat.getNTried()) });
				// model.addRow( new Object[]{ new String("Events failed"), new
				// Double( stat.getNFailed() ) });
				model.addRow(new Object[] { new String("Events selected"),
						new Integer(stat.getNSelected()) });
				model.addRow(new Object[] { new String("Events accepted"),
						new Integer(stat.getNAccepted()) });
				model.addRow(new Object[] {
						new String("Cross section       (pb)"),
						new Double(stat.getCrossSectionAccumulated()) });
				model.addRow(new Object[] {
						new String("Cross section error (pb)"),
						new Double(stat.getCrossSectionErrorAccumulated()) });
				model.addRow(new Object[] {
						new String("Luminosity          (pb)"),
						new Double(stat.getLuminosityAccumulated()) });
			}

			table.setModel(model);

		}
	}

	/**
	 * Show ProMCDescription.proto
	 * 
	 * @author sergei
	 * 
	 */
	private class ShowDescriptionProtoAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ShowDescriptionProtoAction() {
			super("Description Layout");
		}

		public void actionPerformed(ActionEvent e) {

			/*
			 * model = new MyTableModelInfo(); model.addColumn("Nr");
			 * model.addColumn("ProMCDescription.proto"); String tmp =
			 * file.getProMCDescriptionTxt(); StringTokenizer stringtokenizer =
			 * new StringTokenizer(tmp, "\n"); int n = 0; while
			 * (stringtokenizer.hasMoreElements()) { n++; model.addRow(new
			 * Object[] { new Integer(n), new
			 * String(stringtokenizer.nextToken()) });
			 * 
			 * }
			 * 
			 * table.setModel(model);
			 * table.getColumnModel().getColumn(0).setMinWidth(20);
			 * table.getColumnModel().getColumn(0).setPreferredWidth(40);
			 * table.getColumnModel().getColumn(1).setMinWidth(50);
			 * table.getColumnModel().getColumn(1).setPreferredWidth(600);
			 */

			String tmp = file.getProMCDescriptionTxt();
			if (tmp != null & tmp.length() > 0) {
				new SimpleEditor(tmp, false);
			} else {
				JOptionPane.showMessageDialog(frame,
						"ProMCDescription.proto not attached", "Error",
						JOptionPane.ERROR_MESSAGE);
			}

		}
	}

	/**
	 * Show ProMCDescription.proto
	 * 
	 * @author sergei
	 * 
	 */
	private class ShowHeaderProtoAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ShowHeaderProtoAction() {
			super("Header Layout");
		}

		public void actionPerformed(ActionEvent e) {

			/*
			 * model = new MyTableModelInfo(); model.addColumn("Nr");
			 * model.addColumn("ProMCHeader.proto"); String tmp =
			 * file.getProMCHeaderTxt(); StringTokenizer stringtokenizer = new
			 * StringTokenizer(tmp, "\n"); int n = 0; while
			 * (stringtokenizer.hasMoreElements()) { n++; model.addRow(new
			 * Object[] { new Integer(n), new
			 * String(stringtokenizer.nextToken()) });
			 * 
			 * }
			 * 
			 * table.setModel(model);
			 * table.getColumnModel().getColumn(0).setMinWidth(20);
			 * table.getColumnModel().getColumn(0).setPreferredWidth(40);
			 * table.getColumnModel().getColumn(1).setMinWidth(50);
			 * table.getColumnModel().getColumn(1).setPreferredWidth(600);
			 */

			String tmp = file.getProMCHeaderTxt();
			if (tmp != null & tmp.length() > 0) {
				new SimpleEditor(tmp, false);
			} else {
				JOptionPane.showMessageDialog(frame,
						"ProMCHeader.proto not attached", "Error",
						JOptionPane.ERROR_MESSAGE);
			}

		}
	}

	/**
	 * Show ProMC.proto
	 * 
	 * @author sergei
	 * 
	 */
	private class ShowEventProtoAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ShowEventProtoAction() {
			super("Event Layout");
		}

		public void actionPerformed(ActionEvent e) {

			/*
			 * model = new MyTableModelInfo(); model.addColumn("Nr");
			 * model.addColumn("ProMC.proto");
			 * 
			 * String tmp = file.getProMCTxt(); StringTokenizer stringtokenizer
			 * = new StringTokenizer(tmp, "\n"); int n = 0; while
			 * (stringtokenizer.hasMoreElements()) { n++; model.addRow(new
			 * Object[] { new Integer(n), new
			 * String(stringtokenizer.nextToken()) });
			 * 
			 * }
			 * 
			 * table.setModel(model);
			 * table.getColumnModel().getColumn(0).setMinWidth(20);
			 * table.getColumnModel().getColumn(0).setPreferredWidth(40);
			 * table.getColumnModel().getColumn(1).setMinWidth(50);
			 * table.getColumnModel().getColumn(1).setPreferredWidth(600);
			 */

			String tmp = file.getProMCTxt();
			if (tmp != null & tmp.length() > 0) {
				new SimpleEditor(tmp, false);
			} else {
				JOptionPane.showMessageDialog(frame,
						"ProMC.proto not attached", "Error",
						JOptionPane.ERROR_MESSAGE);
			}

		}
	}

	/**
	 * Show ProMC.proto
	 * 
	 * @author sergei
	 * 
	 */
	private class ShowStatProtoAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ShowStatProtoAction() {
			super("Statistics Layout");
		}

		public void actionPerformed(ActionEvent e) {

			/*
			 * model = new MyTableModelInfo(); model.addColumn("Nr");
			 * model.addColumn("ProMCHeader.proto");
			 * 
			 * String tmp = file.getProMCStatTxt(); StringTokenizer
			 * stringtokenizer = new StringTokenizer(tmp, "\n"); int n = 0;
			 * while (stringtokenizer.hasMoreElements()) { n++; model.addRow(new
			 * Object[] { new Integer(n), new
			 * String(stringtokenizer.nextToken()) });
			 * 
			 * }
			 * 
			 * table.setModel(model);
			 * table.getColumnModel().getColumn(0).setMinWidth(20);
			 * table.getColumnModel().getColumn(0).setPreferredWidth(40);
			 * table.getColumnModel().getColumn(1).setMinWidth(50);
			 * table.getColumnModel().getColumn(1).setPreferredWidth(600);
			 */

			String tmp = file.getProMCStatTxt();
			if (tmp != null & tmp.length() > 0) {
				new SimpleEditor(tmp, false);
			} else {
				JOptionPane.showMessageDialog(frame,
						"ProMCStat.proto not attached", "Error",
						JOptionPane.ERROR_MESSAGE);
			}

		}
	}

	/**
	 * Show ProMC.proto
	 * 
	 * @author sergei
	 * 
	 */
	private class ShowLogFileAction extends AbstractAction {
		private static final long serialVersionUID = 1L;

		ShowLogFileAction() {
			super("Log File");
		}

		public void actionPerformed(ActionEvent e) {

			if (file == null)
				return;

			String txt = file.readLogfile();
			if (txt != null & txt.length() > 0) {
				new SimpleEditor(txt, true);
			} else {
				JOptionPane.showMessageDialog(frame,
						"Log file \"logfile.txt\" not attached", "Error",
						JOptionPane.ERROR_MESSAGE);
			}
		}

	}

}
