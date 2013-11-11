package base.handler;

/**
 * The handler that will receive the callback when the error command is received
 * 
 * @author Stefan Bossbaly
 * @author Hardik Patel
 * 
 */
public interface ErrorCommandHandler {

	/**
	 * The error command signals that there was an error with a request. Error
	 * error
	 * 
	 * Error 1 = Not player's turn 2 = Position is out of bounds 3 = Position is
	 * occupied
	 * 
	 * 
	 * @param error
	 *            the code of what error occurred
	 */
	public void handleErrorCommand(int error);
}