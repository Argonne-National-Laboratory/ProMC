package probrowser;



import java.io.*;



public class Main
 {

 /**
 *  Open ProMC file. 
 **/

 public static void main(String args[]) {

  boolean goodVM=true;

        String ver=System.getProperty("java.version").substring(0,3);
        if (ver.indexOf("1.3") >-1)  goodVM=false;
        if (ver.indexOf("1.4") >-1)  goodVM=false;
        if (ver.indexOf("1.5") >-1)  goodVM=false;
        if (ver.indexOf("1.6") >-1)  goodVM=false;
        if (goodVM == false) {
              javax.swing.JOptionPane.showMessageDialog(null,"The Browser requires Java 7 or later. Exit.");
              System.exit(0);
          }


  if(args.length > 0) {
            System.out.println("Open file="+args[0]);
            new MainGui(args[0]);
    } else {
            new MainGui(null); 
         }


}


}

