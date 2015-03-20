// * This code is licensed under:
// * Copyright (c) 2005 by S.Chekanov (chekanov@mail.desy.de). 
// * All rights reserved.

package probrowser;

// Imports
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.table.*;



// define table

// based on DefaultTableModel
class MyTableModelInfo extends DefaultTableModel {


         public static final long serialVersionUID = 10;

         private boolean DEBUG = true; // for debuging this table



        public Class getColumnClass(int c) {
            return getValueAt(0, c).getClass();
        }


        /*
         * Don't need to implement this method unless your table's
         * editable.
         */

        public boolean isCellEditable(int row, int col) {
            //Note that the data/cell address is constant,
            //no matter where the cell appears onscreen.
            // set 0 to some value
                return false;
        }

/*

         private void printDebugData() {
            int numRows = getRowCount();
            int numCols = getColumnCount();

            for (int i=0; i < numRows; i++) {
                System.out.print("    row " + i + ":");
                for (int j=0; j < numCols; j++) {
                    System.out.print("  " + getValueAt(i,j));
                }
                System.out.println();
            }
            System.out.println("--------------------------");
        }
*/
}
