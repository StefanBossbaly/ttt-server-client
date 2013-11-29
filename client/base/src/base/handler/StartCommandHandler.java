package base.handler;

/**
 * The handler that will receive the callback when the start command is received
 * 
 * @author Stefan Bossbaly
 * @author Hardik Patel
 */
public interface StartCommandHandler {
	/**
	 * The start command tells the client that the game has started
	 * 
	 * START
	 */
	public void handleStartCommand();
}
