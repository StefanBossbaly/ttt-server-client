package base.handler;

/**
 * The handler that will receive the callback when the end command is received
 * 
 * @author Stefan Bossbaly
 * @author Hardik Patel
 * 
 */
public interface EndCommandHandler {

	/**
	 * The end command signals that the game is over. END status player
	 * 
	 * Status 0 = Game ended regularly 1 = Game ended because a player
	 * disconnected
	 * 
	 * Player 0 = Neither player 1 = Player 1 2 = Player 2
	 * 
	 * @param status
	 *            the status of the end command
	 * @param player
	 *            the player of the end command
	 */
	public void handleEndCommand(int status, int player);
}