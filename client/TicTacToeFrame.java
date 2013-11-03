import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.*;
 
/*
   Creates the gameboard using JFrame as the GUI
*/
public class TicTacToeFrame extends JFrame
{
    private static final GridLayout LAYOUT = new GridLayout(3,3);
    private static final int HEIGHT = 500;
    private static final int WIDTH = 500;
 
    private JButton buttons[] = new JButton[10];
    private JButton chatBtn;
    public int player;
    public int playerCurr;
    public int x;
    public int y;
    public int pos;
    private JPanel wholePanel, boardPanel, titlePanel;
    private JLabel title;
    public int turns = 1;
    private String letter = "";
    private boolean win = false;

    
 
 
    public TicTacToeFrame()
    {
       createChatButton();
       createTitlePanel();
       if(playerCurr==1){
         createBoardPanel(1);
       }
       else{
         createBoardPanel(2);
       }
       createWholePanel();
        
        //Gets the width of the screen
        Toolkit kit = Toolkit.getDefaultToolkit();
        Dimension screenSize = kit.getScreenSize();
        int screenHeight = screenSize.height;
        int screenWidth = screenSize.width;
 
        //Sets the screen size to half the height and width of the screen, and centers it
        setSize(WIDTH, HEIGHT);
        setLocation(screenWidth / 4, screenHeight / 4);
    }
 
        
    public void createChatButton()
    {
        chatBtn = new JButton("-- Chat --");
        chatBtn.setFont(new Font(Font.SERIF, 0, 24));
 
        class ChatListener implements ActionListener
        {
 
            @Override
            public void actionPerformed(ActionEvent ae) 
            {
                
            }
 
       }
 
        ActionListener ChatListener = new ChatListener();
        chatBtn.addActionListener(ChatListener);
    }

 
    public void createTitlePanel()
    {
        title = new JLabel("Welcome to our Tic Tac Toe Game!");
        titlePanel = new JPanel();
        title.setFont(new Font(Font.SERIF, 0, 30));
        titlePanel.add(title);
    }
 
    public void createBoardPanel(int player)
    {
        boardPanel = new JPanel();
        boardPanel.setLayout(LAYOUT);
        
        if(player == 1)
        {
              letter = "X";
        }
        else  
        {
              letter = "O";
        }
             
        class ButtonListener implements ActionListener
        {
 
            @Override
            public void actionPerformed(ActionEvent ae) 
            {
                /*
                if(player == 1)
                {
                    letter = "X";
                }
                else {
                    letter = "O";
                }
             */
                JButton pressedButton = (JButton)ae.getSource();
                
                for (int j = 0; j < 10; j++) {
                     if( buttons[j] == ae.getSource() ) {
                        pos=j;
                        System.out.println(pos);
                  }
                }
                pressedButton.setText(letter);
                pressedButton.setEnabled(false);
                
               }
 
        }
        
        ActionListener buttonListener = new ButtonListener();

        for(int i=1; i<=9; i++)
        {
            buttons[i] = new JButton("");
            buttons[i].setFont(new Font(Font.SERIF, 0, 24));
            buttons[i].addActionListener(buttonListener);
            boardPanel.add(buttons[i]);
        }
    }
 
    public void createWholePanel()
    {
        wholePanel = new JPanel();
        wholePanel.setLayout(new BorderLayout());
        wholePanel.add(titlePanel, BorderLayout.NORTH);
        wholePanel.add(boardPanel, BorderLayout.CENTER);
        wholePanel.add(chatBtn, BorderLayout.SOUTH);
        add(wholePanel);
    }
    
    public int pos()
    {
      return pos;
    }
         
}


