import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.nio.charset.Charset;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class TTTInterface {
	public static final Pattern MOVE_COMMAND = Pattern.compile("^MOVE (?<x> \\d+) (?<y> \\d+) (?<player> \\d+)");
	public static final Pattern ERROR_COMMAND = Pattern.compile("^ERROR [0-9]");
	public static final Pattern END_COMMAND = Pattern.compile("^END [0-9]");
	
	private Socket socket;
	private MoveCommandHandler moveHandler;
	private ErrorCommandHandler errorHandler;
	private EndCommandHandler endHandler;

	public TTTInterface(Socket socket) {
		this.socket = socket;
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
		InputStreamReader reader = new InputStreamReader(
				this.socket.getInputStream(), Charset.forName("US-ASCII"));

		if (reader.ready()) {
			char[] buffer = new char[1024];
			
			if (reader.read(buffer, 0, buffer.length) == -1) {
				return;
			}
			
			String command = new String(buffer);
			
			Matcher matcher;
			
			matcher = MOVE_COMMAND.matcher(command);
			
			if (matcher.matches()){
				int x = Integer.parseInt(matcher.group("x"));
				int y = Integer.parseInt(matcher.group("y"));
				int player = Integer.parseInt(matcher.group("player"));
				
				this.moveHandler.handleMoveCommand(x, y, player);
				
				return;
			}
		}
	}

	public interface MoveCommandHandler {
		public void handleMoveCommand(int x, int y, int player);
	}

	public interface ErrorCommandHandler {
		public void handleErrorCommand(int error);
	}

	public interface EndCommandHandler {
		public void handleEndCommand(int player);
	}
}
