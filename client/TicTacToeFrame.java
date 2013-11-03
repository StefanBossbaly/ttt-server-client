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
    private String letter = "";
 
    public TicTacToeFrame()
    {   
       createChatButton();
       createTitlePanel();
       /*
         This next part takes PlayerCurr which I get
         From the server.  It then passes that value
         To the createBoardPanel method that need a int player
         Value inputted to it.  This way it always has
         The current player being run.
       */
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
 
    /*
      This method invokes the chat server
      NEEDS WORK
    */    
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

    /*
      This creates just the title pane
      Its the Frame window title
    */
    public void createTitlePanel()
    {
        title = new JLabel("Welcome to our Tic Tac Toe Game!");
        titlePanel = new JPanel();
        title.setFont(new Font(Font.SERIF, 0, 30));
        titlePanel.add(title);
    }
    
    /*
      This is the heart of the program.
      It intializes the board in the panel
      It then determines what player you are
      It then creates a class for a buttonListener
      This listens for button pressing
      Once it is pressed it uses the source of the pressed button
      To find what position was pressed
      After that it puts a letter in that button
      Then makes the button false so it cannot be altered
    */
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
                JButton pressedButton = (JButton)ae.getSource();
                
                for (int j = 0; j < 10; j++) {
                     if( buttons[j] == ae.getSource() ) {
                        pos=j;
                        System.out.println(pos);                        
                  }
                }
                /*
                  Need to put in the methods to send to Stefan
                  To check to see if it is the players turn
                  To put their letter in the space selected
                */
                /*
                  posToCoord(pos);
                  sendMoveCommand(x,y);
                  if(it is a valid move){
                     Block out and fill button
                  }
                */
                pressedButton.setText(letter);
                pressedButton.setEnabled(false);
                /*
                  Need to implment the closeButton Method
                  This would then get the pos from the coorToPos 
                  Method. Then I call the closeButton Method
                  To block out the block from the other user
                */
                /*
                x =
                y =
                player =
                coordToPos(x,y);
                closeButton(pos, player);
                */  
               }
        }
        /*
         Creates the buttons for the board
         It then puts an action listener called button listener
         It listens for any presses of a button
         Then adds the buttons to the board panel
        */
        ActionListener buttonListener = new ButtonListener();

        for(int i=1; i<=9; i++)
        {
            buttons[i] = new JButton("");
            buttons[i].setFont(new Font(Font.SERIF, 0, 24));
            buttons[i].addActionListener(buttonListener);
            boardPanel.add(buttons[i]);
        }
    }
    /*
      This method creates the entire JFrame you see
      It combines the title panel, board panel, and chat button
    */
    public void createWholePanel()
    {
        wholePanel = new JPanel();
        wholePanel.setLayout(new BorderLayout());
        wholePanel.add(titlePanel, BorderLayout.NORTH);
        wholePanel.add(boardPanel, BorderLayout.CENTER);
        wholePanel.add(chatBtn, BorderLayout.SOUTH);
        add(wholePanel);
    }
    
    /*
      This method gets the coordinates from the server
      Then tells me what position that is equal to
    */
    public void coordToPos(int x, int y){
      if(x==0 && y==0){
         pos = 1;
      }
      else if(x==0 && y==1){
         pos = 2;
      }
      else if(x==0 && y==2){
         pos = 3;
      }
      else if(x==1 && y==0){
         pos = 4;
      }
      else if(x==1 && y==1){
         pos = 5;
      }
      else if(x==1 && y==2){
         pos = 6;
      }
      else if(x==2 && y==0){
         pos = 7;
      }
      else if(x==2 && y==1){
         pos = 8;
      }
      else if(x==2 && y==2){
         pos = 9; 
      }
    }
    
    /*
      This method changes the pos I get from the user
      Into the corresponding x and y corrdinates
      So then I can send them to the server
    */
    public void posToCoord(int pos) {
      if(pos == 1){
         x = 0;
         y = 0;
      }
      else if(pos == 2){
         x = 0;
         y = 1;
      }
      else if(pos == 3){
         x = 0;
         y = 2;
      }
      else if(pos == 4){
         x = 1;
         y = 0;
      }
      else if(pos == 5){
         x = 1;
         y = 1;
      }
      else if(pos == 6){
         x = 1;
         y = 2;
      }
      else if(pos == 7){
         x = 2;
         y = 0;
      }
      else if(pos == 8){
         x = 2;
         y = 1;
      }
      else if(pos == 9){
         x = 2;
         y = 2; 
      }
    }
    
    /*
      This method closes the button pressed by the other player
      It gets the input of the pos that was selected from the server
      Then it gets what player pushed it from the server
      It then uses a local letter variable to avoid messing with
      The global letter variable in the program.
      It then sets the pos with that letter and locks that button out.
    */
    public void closeButton(int pos, int player){
      String tempLetter;
      if(player == 1){
         tempLetter = "X";
      }
      else
      {
         tempLetter = "O";
      }
      buttons[pos].setText(tempLetter);       
      buttons[pos].setEnabled(false);                
    }         
}