package controller.client;

import util.events.Event;

public class ClientEvent extends Event
{
	public static final String USER_DISCONNECTED = "USER_DISCONNECTED";
	public static final String REQUESTED_COMMAND_EXECUTION = "REQUESTED_COMMAND_EXECUTION";
	
	public ClientEvent(String type)
	{
		super(type);
		// TODO Auto-generated constructor stub
	}

	public ClientEvent(String type, Boolean bubbles, Boolean cancelable)
	{
		super(type, bubbles, cancelable);
		// TODO Auto-generated constructor stub
	}

}
