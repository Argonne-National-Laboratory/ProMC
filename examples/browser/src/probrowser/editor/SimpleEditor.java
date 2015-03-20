/**
 * Simple editor
 */

package probrowser.editor;

import javax.swing.*;
import javax.swing.text.*;
import java.awt.*;
import java.io.*;
import java.awt.event.*;
import java.util.Hashtable;

public class SimpleEditor extends JFrame {

	private static final long serialVersionUID = 1L;

	private ProtoSyntaxDocument archiveModel;

	private Action openAction = new OpenAction();
	private Action saveAction = new SaveAction();
	private JTextComponent textComp;
	private Hashtable actionHash = new Hashtable();

	public static void main(String[] args) {
		SimpleEditor editor = new SimpleEditor();
		editor.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		editor.setVisible(true);
	}

	// Create an editor.
	public SimpleEditor() {
		super("ProMC Editor");
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		textComp = createTextComponent();
		textComp.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 14));
		makeActionsPretty();

		Container content = getContentPane();
		content.add(textComp, BorderLayout.CENTER);
		// content.add(createToolBar(), BorderLayout.NORTH);
		setJMenuBar(createMenuBar());
		setSize(320, 240);
		setVisible(true);
	}

	/**
	 * Show ProMC proto files
	 * 
	 * @param pro_txt
	 */
	public SimpleEditor(String pro_txt, boolean isLog) {

		super("ProMC Editor");
		setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		if (isLog == true)
			textComp = createTextComponentLog();
		else
			textComp = createTextComponent();

		textComp.setFont(new Font(Font.MONOSPACED, Font.PLAIN, 14));

		makeActionsPretty();
		Container content = getContentPane();

		JScrollPane scroller = new JScrollPane(textComp);
		textComp.setText(pro_txt);
		if (isLog==false) {
			                 scroller.getViewport().setViewPosition(new Point(0,0));
			                 textComp.setCaretPosition(0);
		}
		
		content.add(scroller, BorderLayout.CENTER);
		content.add(createToolBar(), BorderLayout.NORTH);
		setJMenuBar(createMenuBar());
		setSize(600, 400);
		setVisible(true);
	}

	// Create the JTextComponent subclass.
	protected JTextComponent createTextComponent() {

		// archiveModel = new DefaultStyledDocument();
		archiveModel = new ProtoSyntaxDocument();
		JTextPane ta = new JTextPane(archiveModel);
		ta.setEditable(false);
		return ta;
	}

	// Create the JTextComponent subclass.
	protected JTextComponent createTextComponentLog() {

		archiveModel = new ProtoSyntaxDocument();
		JTextArea ta = new JTextArea();
		ta.setLineWrap(true);
		ta.setEditable(false);
		return ta;
	}

	// Add icons and friendly names to actions we care about.
	protected void makeActionsPretty() {
		Action a;

		/*
		 * 
		 * 
		 * a = textComp.getActionMap().get(DefaultEditorKit.cutAction);
		 * java.net.URL imgURL = getClass().getResource("icons/cut.gif");
		 * a.putValue(Action.SMALL_ICON, new ImageIcon(imgURL));
		 * a.putValue(Action.NAME, "Cut");
		 * 
		 * a = textComp.getActionMap().get(DefaultEditorKit.copyAction);
		 * java.net.URL imgURL2 = getClass().getResource("icons/copy.gif");
		 * a.putValue(Action.SMALL_ICON, new ImageIcon(imgURL2));
		 * a.putValue(Action.NAME, "Copy");
		 * 
		 * a = textComp.getActionMap().get(DefaultEditorKit.pasteAction);
		 * java.net.URL imgURL3 = getClass().getResource("icons/paste.gif");
		 * a.putValue(Action.SMALL_ICON, new ImageIcon(imgURL3));
		 * a.putValue(Action.NAME, "Paste");
		 * 
		 * a = textComp.getActionMap().get(DefaultEditorKit.selectAllAction);
		 * a.putValue(Action.NAME, "Select All");
		 */

	}

	// Create a simple JToolBar with some buttons.
	protected JToolBar createToolBar() {
		JToolBar bar = new JToolBar();
		// java.net.URL imgOpen = getClass().getResource("icons/open.gif");
		// java.net.URL imgSave = getClass().getResource("icons/save.gif");
		// Add simple actions for opening & saving.
		// bar.add(getOpenAction()).setIcon(new ImageIcon(imgOpen));
		// bar.add(getSaveAction()).setIcon(new ImageIcon( imgSave));;
		// bar.addSeparator();

		// Add cut/copy/paste buttons.
		// bar.add(textComp.getActionMap().get(DefaultEditorKit.cutAction)).setText("");
		// bar.add(textComp.getActionMap().get(
		// DefaultEditorKit.copyAction)).setText("");
		// bar.add(textComp.getActionMap().get(
		// DefaultEditorKit.pasteAction)).setText("");
		return bar;
	}

	// Create a JMenuBar with file & edit menus.
	protected JMenuBar createMenuBar() {
		JMenuBar menubar = new JMenuBar();
		JMenu file = new JMenu("File");
		JMenu edit = new JMenu("Edit");
		menubar.add(file);
		menubar.add(edit);

		file.add(getOpenAction());
		file.add(getSaveAction());
		file.add(new ExitAction());
		edit.add(textComp.getActionMap().get(DefaultEditorKit.cutAction));
		edit.add(textComp.getActionMap().get(DefaultEditorKit.copyAction));
		edit.add(textComp.getActionMap().get(DefaultEditorKit.pasteAction));
		edit.add(textComp.getActionMap().get(DefaultEditorKit.selectAllAction));
		return menubar;
	}

	// Subclass can override to use a different open action.
	protected Action getOpenAction() {
		return openAction;
	}

	// Subclass can override to use a different save action.
	protected Action getSaveAction() {
		return saveAction;
	}

	protected JTextComponent getTextComponent() {
		return textComp;
	}

	// ********** ACTION INNER CLASSES ********** //

	// A very simple exit action
	public class ExitAction extends AbstractAction {
		/**
	 * 
	 */
		private static final long serialVersionUID = 1L;

		public ExitAction() {
			super("Exit");
		}

		public void actionPerformed(ActionEvent ev) {
			setVisible(false);
			dispose();
		}
	}

	// An action that opens an existing file
	class OpenAction extends AbstractAction {
		/**
	 * 
	 */
		private static final long serialVersionUID = 1L;

		public OpenAction() {
			super("Open");

		}

		// Query user for a filename and attempt to open and read the file into
		// the
		// text component.
		public void actionPerformed(ActionEvent ev) {
			JFileChooser chooser = new JFileChooser();
			if (chooser.showOpenDialog(SimpleEditor.this) != JFileChooser.APPROVE_OPTION)
				return;
			File file = chooser.getSelectedFile();
			if (file == null)
				return;

			FileReader reader = null;
			try {
				reader = new FileReader(file);
				textComp.read(reader, null);
			} catch (IOException ex) {
				JOptionPane.showMessageDialog(SimpleEditor.this,
						"File Not Found", "ERROR", JOptionPane.ERROR_MESSAGE);
			} finally {
				if (reader != null) {
					try {
						reader.close();
					} catch (IOException x) {
					}
				}
			}
		}
	}

	// An action that saves the document to a file
	class SaveAction extends AbstractAction {
		/**
	 * 
	 */
		private static final long serialVersionUID = 1L;

		public SaveAction() {
			super("Save");
		}

		// Query user for a filename and attempt to open and write the text
		// component’s content to the file.
		public void actionPerformed(ActionEvent ev) {
			JFileChooser chooser = new JFileChooser();
			if (chooser.showSaveDialog(SimpleEditor.this) != JFileChooser.APPROVE_OPTION)
				return;
			File file = chooser.getSelectedFile();
			if (file == null)
				return;

			FileWriter writer = null;
			try {
				writer = new FileWriter(file);
				textComp.write(writer);
			} catch (IOException ex) {
				JOptionPane.showMessageDialog(SimpleEditor.this,
						"File Not Saved", "ERROR", JOptionPane.ERROR_MESSAGE);
			} finally {
				if (writer != null) {
					try {
						writer.close();
					} catch (IOException x) {
					}
				}
			}
		}
	}
}
