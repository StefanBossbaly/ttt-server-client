import javax.swing.JFrame;
import javax.swing.UIManager;
 
/**
 *
 * 
 */
public class TicTacToeRunner {
 
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) 
    {
        try
	{
		UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
	}
	catch (Exception e)
	{
		e.printStackTrace();
	}
	JFrame frame = new TicTacToeFrame();
        //JFrame frame1 = new ChatClient();
        //frame1.setTitle("Chat Window");
        frame.setTitle("Tic Tac Toe Game");
        //frame1.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        //frame1.setVisible(true);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setVisible(true);
 
    }
}
