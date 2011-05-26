package controller.client;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.lang.reflect.InvocationTargetException;
import java.net.Socket;
import java.nio.charset.Charset;

import controller.server.ServerController;

import util.events.EventDispatcher;
import util.events.IEventDispatcher;

public class ClientThread extends EventDispatcher implements Runnable {

	private BufferedReader pno_input;
	private PrintWriter pno_output;
	private Socket pao_socket;
	
	private String last_message;

	public ClientThread(Socket socket)
	{
		super();
		this.pao_socket = socket;
	}
	
	public ClientThread(IEventDispatcher target)
	{
		super(target);
	}

	@Override
	public void run()
	{
		try {
			io_setup();
			last_message = "";
			while((!last_message.equals("disconnect")) && ServerController.getInstance().getPrb_running())
			{
				last_message = pno_input.readLine();
				if(last_message == null)
					last_message = "";
				
				if(last_message.equals("") || last_message.equals("\u0000"))
				{
					disconnect();
					return;
				}
				else
				{
					if(last_message.substring(0, 1).equals("\u0000"))
                    {
						last_message = last_message.substring(1);
                    }
					
					// Run the Command
					command_execute();
				}
			}
			disconnect();
		} catch (IOException e) {
			try {
				disconnect();
			} catch (IllegalArgumentException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			} catch (IllegalAccessException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			} catch (InvocationTargetException e1) {
				// TODO Auto-generated catch block
				e1.printStackTrace();
			} catch (IOException e1) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
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
		} catch (SecurityException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (NoSuchMethodException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}
	
	private void command_execute() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException
	{
		dispatchEvent(new ClientEvent(ClientEvent.REQUESTED_COMMAND_EXECUTION));
	}

	private void disconnect() throws IllegalArgumentException, IllegalAccessException, InvocationTargetException, IOException
	{
		pno_input.close();
		pno_output.close();
		dispatchEvent(new ClientEvent(ClientEvent.USER_DISCONNECTED));
	}

	private void io_setup() throws IOException
    {
        pno_input = new BufferedReader(new InputStreamReader(pao_socket.getInputStream(),Charset.forName("UTF-8")));
        pno_output = new PrintWriter(pao_socket.getOutputStream(), true);
        pno_output.flush();
    }

	public Socket getPao_socket() {
		return pao_socket;
	}

	public void setPao_socket(Socket paoSocket) {
		pao_socket = paoSocket;
	}

}
