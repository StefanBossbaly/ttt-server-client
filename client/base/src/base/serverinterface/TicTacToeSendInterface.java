package base.serverinterface;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.nio.charset.Charset;

/**
 * Class that handles the sending of information between the client and the
 * server via the protocol
 * 
 * @author Stefan Bossbaly
 * @author Hardik Patel
 * 
 */
public class TicTacToeSendInterface {
	private Socket socket;
	private OutputStreamWriter writer;

	/**
	 * Constructs a send interface over the socket
	 * 
	 * @param socket
	 * @throws IOException
	 */
	public TicTacToeSendInterface(Socket socket) throws IOException {
		this.socket = socket;
		this.writer = new OutputStreamWriter(this.socket.getOutputStream(),
				Charset.forName("US-ASCII"));
	}

	/**
	 * Sends a move command to the server
	 * 
	 * @param x
	 *            the x position of the location
	 * @param y
	 *            the y position of the location
	 */
	public void sendMoveCommand(int x, int y) {
		try {
			String moveCommand = "MOVE " + x + " " + y + "\0";
			writer.write(moveCommand);
			writer.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Sends the id command to the server
	 * 
	 * @param firstName
	 *            the first name of the player
	 * @param lastName
	 *            the last name of the player
	 */
	public void sendIdCommand(int playerId, String firstName, String lastName) {
		try {
			String idCommand = "ID " + playerId + " " + firstName + " "
					+ lastName + "\0";
			writer.write(idCommand);
			writer.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
