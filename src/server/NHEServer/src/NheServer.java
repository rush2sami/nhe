import java.io.IOException;

import controller.server.ServerController;


public class NheServer
{
	private static ServerController serverController;
	
	/**
	 * Main method of the application,
	 * creates the server and activates it.  
	 * @param args
	 */
	public static void main(String[] args)
	{
		try {
			serverController = ServerController.getInstance();
			serverController.server_start();
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
		} catch (Exception e) {
			ServerController.trace("Couldn't start server: " + e.toString());
			e.printStackTrace();
		}
		
		

	}

}
