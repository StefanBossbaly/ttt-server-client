package base.serverinterface;

import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.nio.charset.Charset;

import base.handler.EndCommandHandler;
import base.handler.ErrorCommandHandler;
import base.handler.MoveCommandHandler;

import com.google.code.regexp.Matcher;
import com.google.code.regexp.Pattern;

/**
 * Class that is the link between the server and the client using the designed
 * protocol
 * 
 * @author Stefan Bossbaly
 * @author Hardik Patel
 * 
 */
public class TicTacToeRecieveInterface {

	/**
	 * The regex pattern for the move command
	 */
	public static final Pattern MOVE_COMMAND = Pattern
			.compile("MOVE[\\s]+(?<x>\\d+)[\\s]+(?<y>\\d+)[\\s]+(?<player>\\d+)");

	/**
	 * The regex pattern for the error command
	 */
	public static final Pattern ERROR_COMMAND = Pattern
			.compile("ERROR[\\s]+(?<error>[\\d]+)");

	/**
	 * The regex pattern for the end command
	 */
	public static final Pattern END_COMMAND = Pattern
			.compile("END[\\s]+(?<status>[\\d]+)[\\s]+(?<player>[\\d]+)");

	private Socket socket;
	private InputStreamReader reader;
	private MoveCommandHandler moveHandler;
	private ErrorCommandHandler errorHandler;
	private EndCommandHandler endHandler;

	/**
	 * 
	 * @param socket
	 * @throws IOException
	 *             if the socket's input stream is not valid
	 */
	public TicTacToeRecieveInterface(Socket socket) throws IOException {
		this.socket = socket;
		this.reader = new InputStreamReader(this.socket.getInputStream(),
				Charset.forName("US-ASCII"));
		this.moveHandler = null;
		this.errorHandler = null;
		this.endHandler = null;
	}

	/**
	 * Registers a move command handler to receive call backs when a move
	 * command is received
	 * 
	 * @param handler
	 *            the class that implements the interface
	 */
	public void registerMoveCommandHandler(MoveCommandHandler handler) {
		this.moveHandler = handler;
	}

	/**
	 * Registers a error command handler to receive call backs when a error
	 * command is received
	 * 
	 * @param handler
	 *            the class that implements the interface
	 */
	public void registerErrorCommandHandler(ErrorCommandHandler handler) {
		this.errorHandler = handler;
	}

	/**
	 * Registers a end command handler to receive call backs when a end command
	 * is received
	 * 
	 * @param handler
	 *            the class that implements the interface
	 */
	public void registerEndCommandHandler(EndCommandHandler handler) {
		this.endHandler = handler;
	}

	/**
	 * Checks to see if the input stream has something to be read
	 * 
	 * @return true if the buffer of the stream is not empty; false otherwise
	 */
	public boolean isReady() {
		try {
			return reader.ready();
		} catch (IOException e) {
			return false;
		}
	}

	/**
	 * Tries to process string received as a command
	 * 
	 * @throws IOException
	 */
	public void acceptInput() throws IOException {

		// Make sure that we are ready for input
		if (reader.ready()) {

			// protocol defines that messages can't exceed protocol 1024
			char[] buffer = new char[1024];

			// Read the message into the buffer
			if (reader.read(buffer, 0, buffer.length) == -1) {
				System.out.println("End of stream reached");
				return;
			}

			// Convert char[] into Java String
			String command = "";
			for (int i = 0; buffer[i] != '\0'; i++) {
				command += buffer[i];
			}

			System.out.println("Buffer read: " + command);

			Matcher matcher = null;

			// See if the command matches our move command
			matcher = MOVE_COMMAND.matcher(command);
			if (matcher.matches()) {
				System.out.println("Move Command Recieved");

				int x = Integer.parseInt(matcher.group("x"));
				int y = Integer.parseInt(matcher.group("y"));
				int player = Integer.parseInt(matcher.group("player"));

				this.moveHandler.handleMoveCommand(x, y, player);

				return;
			}

			// See if the command matches our error command
			matcher = ERROR_COMMAND.matcher(command);
			if (matcher.matches()) {
				System.out.println("Error Command Recieved");

				int error = Integer.parseInt(matcher.group("error"));

				this.errorHandler.handleErrorCommand(error);
				return;
			}

			// See if the command matches our end command
			matcher = END_COMMAND.matcher(command);
			if (matcher.matches()) {
				System.out.println("End Command Recieved");
				int status = Integer.parseInt(matcher.group("status"));
				int player = Integer.parseInt(matcher.group("player"));

				this.endHandler.handleEndCommand(status, player);
				return;
			}

			// Command was not recognized
			throw new RuntimeException("Command not recognized");
		}
	}
}
