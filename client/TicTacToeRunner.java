import javax.swing.JFrame;
 
/**
 *
 * @author TSPARR
 */
public class TicTacToeRunner {
 
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) 
    {
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