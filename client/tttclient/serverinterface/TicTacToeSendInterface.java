package tttclient.serverinterface;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.nio.charset.Charset;

public class TicTacToeSendInterface {
	private Socket socket;
	private OutputStreamWriter writer;

	public TicTacToeSendInterface(Socket socket) throws IOException {
		this.socket = socket;
		this.writer = new OutputStreamWriter(this.socket.getOutputStream(),
				Charset.forName("US-ASCII"));
	}

	public void sendMoveCommand(int x, int y) {
		try {
			String moveCommand = "MOVE " + x + " " + y + "\0";
			writer.write(moveCommand);
			writer.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
