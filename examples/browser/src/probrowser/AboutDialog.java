// * Copyright (c) 2005 by S.Chekanov (chekanov@mail.desy.de). 
// * All rights reserved.
package probrowser;

import java.awt.*;
import java.net.URL;

import javax.swing.*;



public class AboutDialog extends JDialog{
    /**
	 * 
	 */
    private static final long serialVersionUID = 1L;
    private JTabbedPane tabbedPane;
    private JButton closeButton;
    private	JPanel	panel1;
    private	JPanel	panel2;
    private  Component    win;

    public AboutDialog( Component  win ) {
    this.win=win;

    setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);

   setTitle("About");
   setModal( true );
   setResizable( true );


   // Get the system resolution
   Dimension res = Toolkit.getDefaultToolkit().getScreenSize();

   // make sure the dialog is not too big
   Dimension size = new Dimension(Math.min(400, res.width), Math.min(500, res.height));

   setSize(size);
//   setLocationRelativeTo(parent);

  JPanel topPanel = new JPanel();
  JPanel lowerPanel = new JPanel();
  lowerPanel.setPreferredSize(new Dimension(400, 35));


  closeButton = new JButton();
  closeButton.setText("Exit");
  closeButton.addActionListener(new java.awt.event.ActionListener() {
         public void actionPerformed(java.awt.event.ActionEvent evt) {
             setVisible(false); dispose();
        }  });

  lowerPanel.add(closeButton, null);
  topPanel.setLayout( new BorderLayout() );
  getContentPane().add( topPanel, java.awt.BorderLayout.CENTER );
  getContentPane().add( lowerPanel, java.awt.BorderLayout.SOUTH );



   tabbedPane = new javax.swing.JTabbedPane();
   // Create the tab pages
	createPage1();
        createPage2();

		// Create a tabbed pane
	tabbedPane = new JTabbedPane();
	tabbedPane.addTab( "About" , panel1 );
	tabbedPane.addTab( "License", panel2 );
	topPanel.add( tabbedPane, BorderLayout.CENTER );

     Util.centreWithin( win, this );


// set visible and put on center
     this.setVisible(true);
    }




// create about
      public void createPage1() {

      panel1 = new JPanel();
      panel1.setLayout(new BorderLayout());
      JEditorPane epane = new JEditorPane();
      epane.setOpaque( false );
      epane.setContentType( "text/html;charset=ISO-8859-1" );
      epane.setAutoscrolls(true);
      epane.setEditable(false);
      String k1="<h2> ProMC Browser. </h2>";
      String k2="<b>Author: S.Chekanov (ANL/ASC. HEP division)</b><br>";
      String k3="<b>Version: 1.1 </b><br>";
      String all=k1+k2+k3;
      epane.setText(all);
      JScrollPane jsp = new JScrollPane(epane);
      panel1.add(jsp);

      }



// create about
      public void createPage2() {

      panel2 = new JPanel();
      panel2.setLayout(new BorderLayout());
      JEditorPane epane = new JEditorPane();
      epane.setOpaque( false );
      epane.setContentType( "text/html;charset=ISO-8859-1" );
      epane.setAutoscrolls(true);
      epane.setEditable(false);

     try {
         epane.setPage(  getHTMLUrl( "license.html" )  );  
      }
      catch ( Exception e )
      {
         System.err.println("Couldn't create URL");
         epane.setContentType( "text/plain" );
      }
 
      JScrollPane jsp = new JScrollPane(epane);
       panel2.add(jsp);

      }

      /**
       * Returns a HTML page which is found in a valid image URL. The basis of the
       * url is where 'intro' is created, which can't be but the place where
       * JChess resides.
       *
       * @param name
       *            name of the HTML page
       * @return the URL to the page
       */
      public URL getHTMLUrl(String name) {
              URL url = null;
              try {
                      url = this.getClass().getResource("/probrowser/" + name);
              } catch (Exception e) {
              }
              return url;
      }



}
