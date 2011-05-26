package controller.client;

import java.lang.reflect.InvocationTargetException;
import java.net.Socket;

import util.events.EventDispatcher;

import model.db.usu.Usuario;

public class ClientController extends EventDispatcher
{
	private Usuario user;
	private Socket socket;
	private ClientThread thread;
	
	/**
	 * Constructor without sending initial values
	 */
	public ClientController()
	{
		super();
	}
	
	public ClientController(Usuario user, Socket socket, ClientThread thread)
	{
		this.user = user;
		this.socket = socket;
		this.thread = thread;
	}

	public Usuario getUser()
	{
		return user;
	}

	public void setUser(Usuario user)
	{
		this.user = user;
	}

	public Socket getSocket()
	{
		return socket;
	}

	public void setSocket(Socket socket)
	{
		this.socket = socket;
	}

	public ClientThread getThread()
	{
		return thread;
	}

	public void setThread(ClientThread thread) throws SecurityException, NoSuchMethodException
	{
		if(this.thread != null)
		{
			this.thread.setPao_socket(null);
			this.thread.removeEventListener(ClientEvent.REQUESTED_COMMAND_EXECUTION, this, getClass().getMethod("client_requested_command_execution_event"));
			this.thread.removeEventListener(ClientEvent.USER_DISCONNECTED, this, getClass().getMethod("client_disconnected_event"));
		}
		this.thread = thread;
		this.thread.addEventListener(ClientEvent.REQUESTED_COMMAND_EXECUTION, this, getClass().getMethod("client_requested_command_execution_event", ClientEvent.class));
		this.thread.addEventListener(ClientEvent.USER_DISCONNECTED, this, getClass().getMethod("client_disconnected_event", ClientEvent.class));
	}
		
	@SuppressWarnings("unused")
	private void client_requested_command_execution_event(ClientEvent e)
	{
		
	}
		
	@SuppressWarnings("unused")
	private void client_disconnected_event(ClientEvent e) throws IllegalArgumentException, IllegalAccessException, InvocationTargetException
	{
		dispatchEvent(e);
	}
		
}
