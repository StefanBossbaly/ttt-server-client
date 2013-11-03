import java.net.Socket;

public class TTTInterface {
	private Socket socket;

	public TTTInterface(Socket socket) {
		this.socket = socket;
	}

	public interface MoveCommandHandler {
		public void handleMoveCommand(int x, int y, int player);
	}
	
	public interface ErrorCommandHandler {
		public void handleErrorCommand(int error);
	}
	
	public interface EndErrorCommand {
		public void handleEndCommand(int player);
	}
}
