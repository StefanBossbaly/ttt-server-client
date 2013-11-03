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
 
    private JButton buttons[] = new JButton[11], quitBtn, chatBtn;
    private JPanel wholePanel, boardPanel, titlePanel;
    private JLabel title;
    private int turns = 0;
    private String letter = "";
    private boolean win = false;
 
 
    public TicTacToeFrame()
    {
       createChatButton();
       createTitlePanel();
       createBoardPanel();
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
 
        
    private void createChatButton()
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

 
    private void createTitlePanel()
    {
        title = new JLabel("Welcome EXTREME Tic Tac Toe Game!");
        titlePanel = new JPanel();
        title.setFont(new Font(Font.SERIF, 0, 30));
        titlePanel.add(title);
    }
 
    private void createBoardPanel()
    {
        boardPanel = new JPanel();
        boardPanel.setLayout(LAYOUT);
 
        class ButtonListener implements ActionListener
        {
 
            @Override
            public void actionPerformed(ActionEvent ae) 
            {
                turns++;
                if(turns % 2 == 0)
                {
                    letter = "O";
                }
                else {
                    letter = "X";
                }
 
                JButton pressedButton = (JButton)ae.getSource();
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
 
    private void createWholePanel()
    {
        wholePanel = new JPanel();
        wholePanel.setLayout(new BorderLayout());
        wholePanel.add(titlePanel, BorderLayout.NORTH);
        wholePanel.add(boardPanel, BorderLayout.CENTER);
        wholePanel.add(chatBtn, BorderLayout.SOUTH);
        add(wholePanel);
    }
         
}


