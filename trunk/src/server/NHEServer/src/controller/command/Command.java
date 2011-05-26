package controller.command;

public class Command implements ICommand
{
	public static final String CONNECT 			= "01";
	public static final String DISCONNECT 		= "02";
	public static final String LOGIN 			= "03";
	public static final String LOGOUT 			= "04";
	public static final String COMPONENT_ADD 	= "05";
	public static final String COMPONENT_REMOVE = "06";
	public static final String COMPONENT_MOVE	= "07";
	public static final String PROJECT_SAVE		= "08";
	public static final String PROJECT_OPEN		= "09";
	public static final String PROJECT_ENTER	= "10";
	public static final String PROJECT_EXIT		= "11";
	public static final String PROJECT_FINISH	= "12";
	public static final String MESSAGE_SEND		= "13";
	public static final String PROCEDURE_RUN	= "14";
	
	public Command()
	{
		// TODO Auto-generated constructor stub
	}

	@Override
	public void run(String command, String parameters)
	{
		if(command.equals(CONNECT))
		{
			
		}
		else if(command.equals(DISCONNECT))
		{
			
		}
		else if(command.equals(LOGIN))
		{
			
		}
		else if(command.equals(LOGOUT))
		{
			
		}
		else if(command.equals(COMPONENT_ADD))
		{
			
		}
		else if(command.equals(COMPONENT_REMOVE))
		{
			
		}
		else if(command.equals(COMPONENT_MOVE))
		{
			
		}
		else if(command.equals(PROJECT_SAVE))
		{
			
		}
		else if(command.equals(PROJECT_OPEN))
		{
			
		}
		else if(command.equals(PROJECT_ENTER))
		{
			
		}
		else if(command.equals(PROJECT_EXIT))
		{
			
		}
		else if(command.equals(PROJECT_FINISH))
		{
			
		}
		else if(command.equals(MESSAGE_SEND))
		{
			
		}
		else if(command.equals(PROCEDURE_RUN))
		{
			
		}
	}
}
