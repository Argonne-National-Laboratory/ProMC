package probrowser;
import java.awt.*;
import java.util.Objects;
import java.util.regex.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.table.*;

/**
 * Main table panel with search for ProMC files.
 * This program is open source and licensed using the GNU Public license
 * 
 * @author S.Chekanov
 */


public class MainPanel extends JPanel {
    private static final Color warningColor = new Color(255, 200, 200);
    private final JTextField field;
    private final HighlightTableCellRenderer renderer;
    private TableModel model;
    private  TableRowSorter<TableModel> sorter;
    private  JTable table;

    public MainPanel(JTable table) {
        super(new BorderLayout(5, 5));
        renderer = new HighlightTableCellRenderer();
        field = new JTextField("");
        this.table=table;
        this.model= table.getModel(); 
        sorter = new TableRowSorter<TableModel>(model);
        table.setFillsViewportHeight(true);

        // this also sorts, but it breaks !
        // table.setRowSorter(sorter);
        table.setDefaultRenderer(String.class, renderer);

        field.getDocument().addDocumentListener(new DocumentListener() {
            @Override public void insertUpdate(DocumentEvent e) {
                fireDocumentChangeEvent();
            }
            @Override public void removeUpdate(DocumentEvent e) {
                fireDocumentChangeEvent();
            }
            @Override public void changedUpdate(DocumentEvent e) {  }
        });
        fireDocumentChangeEvent();
        JPanel sp = new JPanel(new BorderLayout(5, 5));
        sp.add(new JLabel("Search (Regex Pattern):"), BorderLayout.WEST);
        sp.add(field);
        sp.add(Box.createVerticalStrut(2), BorderLayout.SOUTH);
        // sp.setBorder(BorderFactory.createTitledBorder("Search"));

        setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
        add(sp, BorderLayout.NORTH);
        add(new JScrollPane(table));
        setPreferredSize(new Dimension(320, 240));
    }

    private void fireDocumentChangeEvent() {

        model= table.getModel(); 
        sorter = new TableRowSorter<TableModel>(model);
        field.setBackground(Color.WHITE);
        String pattern = field.getText().trim();
        if(pattern.isEmpty()) {
            sorter.setRowFilter(null);
            renderer.setPattern("");
        }else if(renderer.setPattern(pattern)) {
            try{
               sorter.setRowFilter(RowFilter.regexFilter(pattern));
            }catch(PatternSyntaxException ex) {
               field.setBackground(warningColor);
            }
        }

     table.repaint(); 

    }

/*
    public static void main(String[] args) {
        EventQueue.invokeLater(new Runnable() {
            @Override public void run() {
                createAndShowGUI();
            }
        });
    }
*/

/*
    public static void createAndShowGUI() {
        try{
            UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
        }catch(ClassNotFoundException | InstantiationException |
               IllegalAccessException | UnsupportedLookAndFeelException ex) {
            ex.printStackTrace();
        }
        JFrame frame = new JFrame("TableHighlightRegexFilter");
        frame.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
        frame.getContentPane().add(new MainPanel());
        frame.pack();
        frame.setLocationRelativeTo(null);
        frame.setVisible(true);
    }
*/

}

class HighlightTableCellRenderer extends JTextField implements TableCellRenderer {
    private static final Color backgroundSelectionColor = new Color(220, 240, 255);
    private static final Highlighter.HighlightPainter highlightPainter = new DefaultHighlighter.DefaultHighlightPainter(Color.YELLOW);
    private String pattern = "";
    private String prev = null;

    public boolean setPattern(String str) {
        if(str==null || str.equals(pattern)) {
            return false;
        }else{
            prev = pattern;
            pattern = str;
            return true;
        }
    }
    public HighlightTableCellRenderer() {
        super();
        setOpaque(true);
        setBorder(BorderFactory.createEmptyBorder());
        setForeground(Color.BLACK);
        setBackground(Color.WHITE);
        setEditable(false);
    }
    @Override public Component getTableCellRendererComponent(JTable table, Object value, boolean isSelected, boolean hasFocus, int row, int column) {
        String txt = Objects.toString(value, "");
        Highlighter highlighter = getHighlighter();
        highlighter.removeAllHighlights();
        setText(txt);
        setBackground(isSelected ? backgroundSelectionColor : Color.WHITE);
        if(pattern!=null && !pattern.isEmpty() && !pattern.equals(prev)) {
            Matcher matcher = Pattern.compile(pattern).matcher(txt);
            if(matcher.find()) {
                int start = matcher.start();
                int end   = matcher.end();
                try{
                    highlighter.addHighlight(start, end, highlightPainter);
                }catch(BadLocationException e) {
                    e.printStackTrace();
                }
            }
        }
        return this;
    }
}
