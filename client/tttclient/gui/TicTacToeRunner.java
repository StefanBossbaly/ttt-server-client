package tttclient.gui;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import javax.swing.JFrame;
import javax.swing.UIManager;

import tttclient.serverinterface.TicTacToeRecieveInterface;
import tttclient.serverinterface.TicTacToeSendInterface;

/**
 *
 * 
 */
public class TicTacToeRunner {

	/*
	 * @param args the command line arguments
	 */
	public static void main(String[] args) throws UnknownHostException,
			IOException {
		try {
			UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
		} catch (Exception e) {
			e.printStackTrace();
		}

		// Establish a connection
		final Socket socket = new Socket("localhost", 3700);

		TicTacToeSendInterface send = new TicTacToeSendInterface(socket);

		final TicTacToeFrame frame = new TicTacToeFrame(send);
		frame.setTitle("Tic Tac Toe Game");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setVisible(true);

		(new Thread() {
			public void run() {
				try {
					TicTacToeRecieveInterface recieveInterface = new TicTacToeRecieveInterface(
							socket);
					recieveInterface.registerMoveCommandHandler(frame);
					recieveInterface.registerErrorCommandHandler(frame);
					recieveInterface.registerEndCommandHandler(frame);

					while (true) {
						recieveInterface.acceptInput();
					}
				} catch (IOException e) {
					throw new RuntimeException("Should never happen");
				}
			}
		}).start();
	}
}
