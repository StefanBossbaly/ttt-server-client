package tttclient.thread;

import java.io.IOException;
import java.net.Socket;

import tttclient.handler.EndCommandHandler;
import tttclient.handler.ErrorCommandHandler;
import tttclient.handler.MoveCommandHandler;
import tttclient.serverinterface.TicTacToeRecieveInterface;

public class TicTacToeRecieveThread extends Thread {
	private Socket socket;
	private MoveCommandHandler moveHandler;
	private ErrorCommandHandler errorHandler;
	private EndCommandHandler endHandler;

	public TicTacToeRecieveThread(Socket socket,
			MoveCommandHandler moveHandler, ErrorCommandHandler errorHandler,
			EndCommandHandler endHandler) {

		this.socket = socket;
		this.moveHandler = moveHandler;
		this.errorHandler = errorHandler;
		this.endHandler = endHandler;
	}

	public void run() {
		try {
			TicTacToeRecieveInterface recieveInterface = new TicTacToeRecieveInterface(
					this.socket);

			// Register for callbacks
			recieveInterface.registerMoveCommandHandler(this.moveHandler);
			recieveInterface.registerErrorCommandHandler(this.errorHandler);
			recieveInterface.registerEndCommandHandler(this.endHandler);

			while (true) {
				//See if we are ready for input
				if (recieveInterface.isReady()) {
					recieveInterface.acceptInput();
				} 
				//If not then just yield and wait for another turn
				else {
					Thread.yield();
				}
			}
		} catch (IOException e) {
			throw new RuntimeException("Should never happen");
		}
	}
}
