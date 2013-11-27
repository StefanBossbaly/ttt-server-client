package base.thread;

import java.io.IOException;
import java.net.Socket;

import base.handler.MessageCommandHandler;
import base.serverinterface.ChatRecieveInterface;

public class ChatRecieveThread extends Thread {

	private Socket socket;
	private MessageCommandHandler handler;

	public ChatRecieveThread(Socket socket, MessageCommandHandler handler) {
		this.socket = socket;
		this.handler = handler;
	}

	@Override
	public void run() {
		try {
			ChatRecieveInterface chatInterface = new ChatRecieveInterface(
					socket);
			chatInterface.registerMessageCommandHandler(this.handler);

			while (true) {
				if (chatInterface.isReady()) {
					chatInterface.acceptInput();
				} else {
					Thread.yield();
				}
			}
		} catch (IOException e) {
			throw new RuntimeException("Should never happen");
		}
	}
}
