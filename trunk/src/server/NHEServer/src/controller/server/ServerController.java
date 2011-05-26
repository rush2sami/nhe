package controller.server;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.net.ServerSocket;
import java.util.Vector;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import util.events.EventDispatcher;
import view.server.ServerWidget;
import controller.client.ClientController;
import controller.client.ClientEvent;
import controller.client.ClientThread;

/**
 * Singleton which controls the server
 * @author atavares
 * 
 */
public class ServerController extends EventDispatcher
{
	// constants
	public static final int PORT = 5555;
	
	// Properties
	private ServerWidget pro_server_widget;
	private Vector<ClientController> prl_client_controllers;
	private ServerSocket pro_server_socket;
	private ServerThread pro_server_thread;
	private ExecutorService pro_executor_service;
	
	// Singleton instance
	private static ServerController serverController;
	
	private Boolean prb_running;
	
	
	/**
	 * Creates the server controller, the view, the server socket
	 * @throws IOException
	 * @throws SecurityException
	 * @throws NoSuchMethodException
	 */
	public ServerController() throws IOException, SecurityException, NoSuchMethodException
	{
		super();
		this.pro_server_socket = new ServerSocket(PORT);
		this.pro_server_widget = new ServerWidget();
		this.prl_client_controllers = new Vector<ClientController>();
		this.pro_server_thread = new ServerThread(this.pro_server_socket);
		this.pro_server_thread.addEventListener(ServerEvent.USER_CONNECTED, this, getClass().getMethod("server_user_connected_event", ServerEvent.class));
		this.pro_executor_service = Executors.newFixedThreadPool(5000);
		this.prb_running = false;
	}
	
	/**
	 * Get the Singleton instance of the server
	 * @return
	 * @throws SecurityException
	 * @throws IOException
	 * @throws NoSuchMethodException
	 */
	public static ServerController getInstance() throws SecurityException, IOException, NoSuchMethodException
	{
		if(serverController == null) serverController = new ServerController();
		return serverController;
	}
	
	public void server_start() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException
	{
		prb_running = true;
		dispatchEvent(new ServerEvent(ServerEvent.SERVER_STARTED));
	}
	
	public void server_stop() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException
	{
		prb_running = false;
		dispatchEvent(new ServerEvent(ServerEvent.SERVER_STOPPED));
	}
	
	public void server_user_connected_event(ServerEvent e) throws SecurityException, NoSuchMethodException
	{
		
		ClientController client_controller = new ClientController();
		ClientThread client_thread = new ClientThread(e.socket);
		
		client_controller.setThread(client_thread);
		
		client_controller.addEventListener(ClientEvent.USER_DISCONNECTED, this, getClass().getMethod("server_user_disconnected_event"));
		
		this.prl_client_controllers.add(client_controller);
		pro_executor_service.execute(client_thread);
	}
	
	
	public void server_user_disconnected_event(ServerEvent e)
	{
		this.prl_client_controllers.remove(e.getTarget());
	}

	public Boolean getPrb_running() {
		return prb_running;
	}

	public static void trace(String string)
	{
		// TODO Auto-generated method stub
		try {
			getInstance().pro_server_widget.trace(string);
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
