package tttclient.serverinterface;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.nio.charset.Charset;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import tttclient.handler.EndCommandHandler;
import tttclient.handler.ErrorCommandHandler;
import tttclient.handler.MoveCommandHandler;

public class TicTacToeRecieveInterface {
	public static final Pattern MOVE_COMMAND = Pattern
			.compile("MOVE[\\s]+(?<x>\\d+)[\\s]+(?<y>\\d+)[\\s]+(?<player>\\d+)");
	public static final Pattern ERROR_COMMAND = Pattern
			.compile("ERROR[\\s]+(?<error>[\\d]+)");
	public static final Pattern END_COMMAND = Pattern
			.compile("END[\\s]+(?<status>[\\d]+)[\\s]+(?<player>[\\d]+)");

	private Socket socket;
	private InputStreamReader reader;
	private MoveCommandHandler moveHandler;
	private ErrorCommandHandler errorHandler;
	private EndCommandHandler endHandler;

	public TicTacToeRecieveInterface(Socket socket) throws IOException {
		this.socket = socket;
		this.reader = new InputStreamReader(this.socket.getInputStream(),
				Charset.forName("US-ASCII"));
		this.moveHandler = null;
		this.errorHandler = null;
		this.endHandler = null;
	}

	public void registerMoveCommandHandler(MoveCommandHandler handler) {
		this.moveHandler = handler;
	}

	public void registerErrorCommandHandler(ErrorCommandHandler handler) {
		this.errorHandler = handler;
	}

	public void registerEndCommandHandler(EndCommandHandler handler) {
		this.endHandler = handler;
	}

	public void acceptInput() throws IOException {
		if (reader.ready()) {
			char[] buffer = new char[1024];

			if (reader.read(buffer, 0, buffer.length) == -1) {
				return;
			}
			
			String command = "";
			
			for (int i = 0; buffer[i] != '\0'; i++)
			{
				command += buffer[i];
			}
			
			System.out.println("Buffer read: " + command);
			Matcher matcher = null;

			matcher = MOVE_COMMAND.matcher(command);
			if (matcher.matches()) {
				System.out.println("Move Command Recieved");
				
				int x = Integer.parseInt(matcher.group("x"));
				int y = Integer.parseInt(matcher.group("y"));
				int player = Integer.parseInt(matcher.group("player"));

				this.moveHandler.handleMoveCommand(x, y, player);

				return;
			}

			matcher = ERROR_COMMAND.matcher(command);
			if (matcher.matches()) {
				System.out.println("Error Command Recieved");
				
				int error = Integer.parseInt(matcher.group("error"));

				this.errorHandler.handleErrorCommand(error);
				return;
			}

			matcher = END_COMMAND.matcher(command);
			if (matcher.matches()) {
				System.out.println("End Command Recieved");
				int status = Integer.parseInt(matcher.group("status"));
				int player = Integer.parseInt(matcher.group("player"));

				this.endHandler.handleEndCommand(status, player);
				return;
			}
			
			System.out.println("Command not reconized");
		}
	}
}
