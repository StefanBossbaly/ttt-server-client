package base.handler;

/**
 * The handler that will receive the callback when the move command is received
 * 
 * @author Stefan Bossbaly
 * @author Hardik Patel
 * 
 */
public interface MoveCommandHandler {

	/**
	 * The move command moves a player to a location.
	 * 
	 * MOVE x y player
	 * 
	 * @param x
	 *            the x position of the location
	 * @param y
	 *            the y position of the location
	 * @param player
	 *            the player that moved to the location
	 */
	public void handleMoveCommand(int x, int y, int player);
}
