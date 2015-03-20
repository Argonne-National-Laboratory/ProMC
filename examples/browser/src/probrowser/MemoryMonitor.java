package probrowser;


import java.util.*;
import javax.swing.SwingUtilities;


/** Simple memory monitor.
 * <p>
 * $Id: MemoryMonitor.java,v 1.13 2005/01/02 12:20:40 ccavanaugh Exp $
 * 
 * @author Craig Cavanaugh and  S.Chekanov  
 */
public class MemoryMonitor extends javax.swing.JProgressBar {


    int total;
    int used;
    int oldUsed;
    float fraction;

    final int diff = 10;

    Runtime runtime = Runtime.getRuntime();
    Timer timer = new Timer(true);

    public MemoryMonitor() {
        super();
        setMinimum(0);
        setStringPainted(true);
        
        /* A reusable runnable */
        final Runnable update = new Runnable() {
            public void run() {
                setMaximum(total);
                setValue(used);
//                setString(used + " Kb / " + total + " Kb");                
                 String str = (used / 1024) + "/"
                                + (total / 1024) + "Mb";
                  setString(str); 
           
            }
        };

        timer.schedule(new TimerTask() {
            public void run() {
                total = (int) (runtime.totalMemory() / 1024);
                used = total - (int) (runtime.freeMemory() / 1024);
                fraction = ((float)used) / total; 
 
                if (used < oldUsed - diff || used > oldUsed + diff) {                    
                    SwingUtilities.invokeLater(update);
                    oldUsed = used;
                }
            }
        }, 9, 2000);
    }
}
