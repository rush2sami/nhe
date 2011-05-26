package controller.server;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.net.ServerSocket;
import java.net.Socket;

import util.events.EventDispatcher;

public class ServerThread extends EventDispatcher implements Runnable
{
	private ServerSocket pao_socket_server;
		
	public ServerThread(ServerSocket vro_socket_server)
	{
		this.pao_socket_server = vro_socket_server;
	}

	@Override
	public void run()
	{
		try {
			Socket socket = this.pao_socket_server.accept();
			ServerEvent event = new ServerEvent(ServerEvent.USER_CONNECTED);
			event.socket = socket;
			dispatchEvent(event);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalArgumentException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IllegalAccessException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InvocationTargetException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

}
