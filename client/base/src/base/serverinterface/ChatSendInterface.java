package base.serverinterface;

import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.nio.charset.Charset;

public class ChatSendInterface {
	private Socket socket;
	private OutputStreamWriter writer;

	public ChatSendInterface(Socket socket) throws IOException {
		this.socket = socket;
		this.writer = new OutputStreamWriter(this.socket.getOutputStream(),
				Charset.forName("US-ASCII"));
	}

	public void sendBroadcastCommand(String message) {
		try {
			String broadcastCommand = "BROADCAST " + message + "\0";
			writer.write(broadcastCommand);
			writer.flush();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
