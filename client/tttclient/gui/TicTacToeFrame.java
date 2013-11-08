package tttclient.gui;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.*;

import tttclient.handler.EndCommandHandler;
import tttclient.handler.ErrorCommandHandler;
import tttclient.handler.MoveCommandHandler;
import tttclient.serverinterface.TicTacToeSendInterface;

/*
 Creates the gameboard using JFrame as the GUI
 */
public class TicTacToeFrame extends JFrame implements MoveCommandHandler,
		ErrorCommandHandler, EndCommandHandler {
	private TicTacToeSendInterface sendInterface;

	private static final GridLayout LAYOUT = new GridLayout(3, 3);
	private static final int HEIGHT = 500;
	private static final int WIDTH = 500;

	private JButton buttons[] = new JButton[9];
	private JButton chatBtn;
	private JPanel wholePanel, boardPanel, titlePanel;
	private JLabel title;

	public TicTacToeFrame(TicTacToeSendInterface sendInterface) {
		this.sendInterface = sendInterface;

		createChatButton();
		createTitlePanel();
		createBoardPanel();
		createWholePanel();

		// Gets the width of the screen
		Toolkit kit = Toolkit.getDefaultToolkit();
		Dimension screenSize = kit.getScreenSize();
		int screenHeight = screenSize.height;
		int screenWidth = screenSize.width;

		// Sets the screen size to half the height and width of the screen, and
		// centers it
		this.setSize(WIDTH, HEIGHT);
		this.setLocation(screenWidth / 4, screenHeight / 4);
	}

	/*
	 * This method invokes the chat server NEEDS WORK
	 */
	public void createChatButton() {
		chatBtn = new JButton("-- Chat --");
		chatBtn.setFont(new Font(Font.SERIF, 0, 24));

		class ChatListener implements ActionListener {
			@Override
			public void actionPerformed(ActionEvent ae) {

			}
		}
		ActionListener ChatListener = new ChatListener();
		chatBtn.addActionListener(ChatListener);
	}

	/*
	 * This creates just the title pane Its the Frame window title
	 */
	public void createTitlePanel() {
		title = new JLabel("Welcome to our Tic Tac Toe Game!");
		titlePanel = new JPanel();
		title.setFont(new Font(Font.SERIF, 0, 24));
		titlePanel.add(title);
	}

	/*
	 * This is the heart of the program. It intializes the board in the panel It
	 * then determines what player you are It then creates a class for a
	 * buttonListener This listens for button pressing Once it is pressed it
	 * uses the source of the pressed button To find what position was pressed
	 * After that it puts a letter in that button Then makes the button false so
	 * it cannot be altered
	 */
	public void createBoardPanel() {
		boardPanel = new JPanel();
		boardPanel.setLayout(LAYOUT);

		/*
		 * Creates the buttons for the board It then puts an action listener
		 * called button listener It listens for any presses of a button Then
		 * adds the buttons to the board panel
		 */
		ActionListener buttonListener = new ButtonListener(this.sendInterface);

		for (int i = 0; i < buttons.length; i++) {
			buttons[i] = new JButton("");
			buttons[i].setFont(new Font(Font.SERIF, 0, 24));
			buttons[i].addActionListener(buttonListener);
			boardPanel.add(buttons[i]);
		}
	}

	/*
	 * This method creates the entire JFrame you see It combines the title
	 * panel, board panel, and chat button
	 */
	public void createWholePanel() {
		wholePanel = new JPanel();
		wholePanel.setLayout(new BorderLayout());
		wholePanel.add(titlePanel, BorderLayout.NORTH);
		wholePanel.add(boardPanel, BorderLayout.CENTER);
		wholePanel.add(chatBtn, BorderLayout.SOUTH);
		add(wholePanel);
	}

	/*
	 * This method gets the coordinates from the server Then tells me what
	 * position that is equal to
	 */
	public int coordToPos(int x, int y) {
		return (x * 3) + y;
	}

	/*
	 * This method changes the pos I get from the user Into the corresponding x
	 * and y corrdinates So then I can send them to the server
	 */
	public int xPosToCoord(int pos) {
		return (pos / 3);
	}

	public int yPosToCoord(int pos) {
		return (pos % 3);
	}

	/*
	 * This method closes the button pressed by the other player It gets the
	 * input of the pos that was selected from the server Then it gets what
	 * player pushed it from the server It then uses a local letter variable to
	 * avoid messing with The global letter variable in the program. It then
	 * sets the pos with that letter and locks that button out.
	 */
	public void closeButton(int pos, int player) {
		String tempLetter;
		if (player == 1) {
			tempLetter = "X";
		} else {
			tempLetter = "O";
		}
		buttons[pos].setText(tempLetter);
		buttons[pos].setEnabled(false);
	}

	private class ButtonListener implements ActionListener {
		private TicTacToeSendInterface sendInterface;

		public ButtonListener(TicTacToeSendInterface sendInterface) {
			this.sendInterface = sendInterface;
		}

		@Override
		public void actionPerformed(ActionEvent ae) {
			JButton pressedButton = (JButton) ae.getSource();

			for (int i = 0; i < buttons.length; i++) {
				if (buttons[i] == pressedButton) {
					int x = xPosToCoord(i);
					int y = yPosToCoord(i);

					sendInterface.sendMoveCommand(x, y);

					return;
				}
			}
		}
	}

	@Override
	public void handleEndCommand(int status, int player) {

		for (int i = 0; i < buttons.length; i++) {
			buttons[i].setEnabled(false);
		}

		if (status == 0) {
			if (player != 0) {
				JOptionPane.showMessageDialog(this,
						"The game has ended! Player " + player + " has won!",
						"Game Ended!", JOptionPane.INFORMATION_MESSAGE);
			} else {
				JOptionPane.showMessageDialog(this,
						"The game has ended! It was a draw!", "Game Ended!",
						JOptionPane.INFORMATION_MESSAGE);
			}
		} else {
			JOptionPane.showMessageDialog(this,
					"The game has ended because a player disconnected. Player " + player
							+ " has won!", "Game Ended!",
					JOptionPane.INFORMATION_MESSAGE);
		}
	}

	@Override
	public void handleErrorCommand(int error) {
		if (error == 1) {
			JOptionPane.showMessageDialog(this, "Not your turn!", "Error",
					JOptionPane.ERROR_MESSAGE);
		} else if (error == 2) {
			JOptionPane.showMessageDialog(this, "Location is out of bounds!",
					"Error", JOptionPane.ERROR_MESSAGE);
		} else if (error == 3) {
			JOptionPane.showMessageDialog(this,
					"Location is currently occupied!", "Error",
					JOptionPane.ERROR_MESSAGE);
		} else {
			JOptionPane.showMessageDialog(this, "Unspecified error!", "Error",
					JOptionPane.ERROR_MESSAGE);
		}

	}

	@Override
	public void handleMoveCommand(int x, int y, int player) {
		int position = coordToPos(x, y);
		closeButton(position, player);
	}
}