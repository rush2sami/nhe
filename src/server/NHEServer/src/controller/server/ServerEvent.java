package controller.server;

import java.net.Socket;

import util.events.Event;

public class ServerEvent extends Event
{
	public static final String USER_CONNECTED = "USER_CONNECTED";
	public static final String SERVER_STARTED = "SERVER_STARTED";
	public static final String SERVER_STOPPED = "SERVER_STOPPED";
	
	public Socket socket;
	
	public ServerEvent(String type)
	{
		super(type);
		// TODO Auto-generated constructor stub
	}

	public ServerEvent(String type, Boolean bubbles, Boolean cancelable)
	{
		super(type, bubbles, cancelable);
		// TODO Auto-generated constructor stub
	}

}
