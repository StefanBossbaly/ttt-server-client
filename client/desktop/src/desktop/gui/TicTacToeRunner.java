package desktop.gui;

import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

import javax.swing.JFrame;
import javax.swing.UIManager;

import base.serverinterface.TicTacToeSendInterface;
import base.thread.TicTacToeRecieveThread;

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
		final Socket socket = new Socket("192.168.1.7", 32600);

		TicTacToeSendInterface send = new TicTacToeSendInterface(socket);

		final TicTacToeFrame frame = new TicTacToeFrame(send);
		frame.setTitle("Tic Tac Toe Game");
		frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		frame.setVisible(true);

		// Start listening for server responses
		TicTacToeRecieveThread thread = new TicTacToeRecieveThread(socket,
				frame, frame, frame);
		thread.start();
	}
}
