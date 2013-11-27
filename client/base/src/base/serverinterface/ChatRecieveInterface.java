package base.serverinterface;

import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.nio.charset.Charset;

import base.handler.MessageCommandHandler;
import base.handler.MoveCommandHandler;

import com.google.code.regexp.Matcher;
import com.google.code.regexp.Pattern;

public class ChatRecieveInterface {
	public static final Pattern MESSAGE_COMMAND = Pattern
			.compile("MESSAGE[\\s]+(?<name>[^,]+)[, ](?<message>\\p{ASCII}+)");
	
	private Socket socket;
	private InputStreamReader reader;
	private MessageCommandHandler messageHandler;
	
	public ChatRecieveInterface(Socket socket) throws IOException {
		this.socket = socket;
		this.reader = new InputStreamReader(this.socket.getInputStream(),
				Charset.forName("US-ASCII"));
		this.messageHandler = null;
	}
	
	public void registerMessageCommandHandler(MessageCommandHandler handler) {
		this.messageHandler = handler;
	}
	
	public boolean isReady() {
		try {
			return reader.ready();
		} catch (IOException e) {
			return false;
		}
	}
	
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
			matcher = MESSAGE_COMMAND.matcher(command);
			
			if (matcher.matches()){
				String name = matcher.group("name");
				String message = matcher.group("message");
				
				this.messageHandler.handMessageCommand(name, message);
				return;
			}
			
			// Command was not recognized
			throw new RuntimeException("Command not recognized");
		}
	}
}
