package br.poli.ecomp.geav.nhe.controller.server;

import java.lang.reflect.InvocationTargetException;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;

import org.red5.server.adapter.ApplicationAdapter;
import org.red5.server.api.IConnection;
import org.red5.server.api.Red5;

import br.poli.ecomp.geav.nhe.controller.db.DBConnection;
import br.poli.ecomp.geav.nhe.model.db.Banco;
import br.poli.ecomp.geav.nhe.model.db.Procedures;
import br.poli.ecomp.geav.nhe.model.db.RepositoryException;
import br.poli.ecomp.geav.nhe.model.db.pro.Project;
import br.poli.ecomp.geav.nhe.model.db.usr.User; 
import br.poli.ecomp.geav.nhe.model.server.ConnectedUser;
import br.poli.ecomp.geav.nhe.model.server.LoginStatus;
import br.poli.ecomp.geav.nhe.model.server.ProcedureResponse;
import br.poli.ecomp.geav.nhe.model.server.Room;

public class NHEApplication extends ApplicationAdapter
{
	private List<Room> rooms_opened = new ArrayList<Room>();
	private List<ConnectedUser> users_connected = new ArrayList<ConnectedUser>();
	
	
	public static void getInstance()
	{
		 
	}
	
	public LoginStatus login(String user, String password)
	{
		
		LoginStatus status = new LoginStatus();
		
		try {
			DBConnection.get_connection().begin();
			Vector<String> parameters = new Vector<String>();
			parameters.add("'" + user + "'");
			parameters.add("'" + password + "'");
			ProcedureResponse response = DBConnection.get_connection().run_procedure(Procedures.USR_LOGIN, parameters);
			status.pao_user = response;
			if(!response.data.isEmpty())
			{
				Boolean hasUser = false;
				for(int i = 0; i < users_connected.size(); i++)
				{
					if(users_connected.get(i).user.get_usr_login().equals(user))
					{
						hasUser = true;
						disconnect(users_connected.get(i).connection,this.scope);
						users_connected.get(i).connection = Red5.getConnectionLocal();
					}
				}
				
				if(!hasUser)
				{
					ConnectedUser connectedUser = new ConnectedUser();
					connectedUser.connection = Red5.getConnectionLocal();
					connectedUser.current_room = null;
					connectedUser.user = new User();
					
					connectedUser.user.set_usr_login(user);
					
					int indexName = response.fields.indexOf("usr_name");
					connectedUser.user.set_usr_name(String.valueOf((response.data.get(0).get(indexName))));
					
					int indexEmail = response.fields.indexOf("usr_email");
					connectedUser.user.set_usr_email(String.valueOf((response.data.get(0).get(indexEmail))));
					
					Integer id = (Integer) response.data.get(0).get(response.fields.indexOf("usr_identificador"));
					connectedUser.user.set_usr_identificador(id.intValue());
					
					try {
						connectedUser.user = (User) Banco.getInstance().repositorio(Banco.USER_STR).adicionarComCuidado(connectedUser.user);
					} catch (IllegalArgumentException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (RepositoryException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (IllegalAccessException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					} catch (InvocationTargetException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
					
					users_connected.add(connectedUser);
				}
				status.pab_logged = true;
			}
			else
				status.pab_logged = false;
		} catch (SQLException e) {
			status.pab_logged = false;
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			status.pab_logged = false;
			e.printStackTrace();
		}
		
		return status;
	}
	
	
	@Override
	public void appDisconnect(IConnection conn) {
		
		super.appDisconnect(conn);
		
		logout();
	}
	
	public LoginStatus logout()
	{
		ConnectedUser userRemoved = null;
		for(int i = 0; i < users_connected.size(); i++)
		{
			if(users_connected.get(i).connection.equals(Red5.getConnectionLocal()))
			{
				userRemoved = users_connected.remove(i);
			}
		}
		
		for(int i = 0; i < rooms_opened.size(); i++)
		{
			rooms_opened.get(i).connected_users.remove(userRemoved);
		}
		LoginStatus loginStatus = new LoginStatus();
		loginStatus.pab_logged = false;
		return loginStatus;
	}
	
	public int createProject(String title, String description, String model, List<String> participants, int usr_id)
	{
		Vector<String> parameters = new Vector<String>();
		parameters.add("'" + title + "'");
		parameters.add("'" + description + "'");
		parameters.add(Integer.toString(usr_id));
		parameters.add("'" + model + "'");
		ProcedureResponse response;
		try {
			DBConnection.get_connection().begin();
			response = DBConnection.get_connection().run_procedure(Procedures.PRO_CREATE, parameters);
			for(int i = 0; i < participants.size(); i++)
			{
				Vector<String> paramPtc = new Vector<String>();
				paramPtc.add("'" + participants.get(i) + "'");
				paramPtc.add("'" + response.data.get(0).get(0) + "'");
				DBConnection.get_connection().run_procedure(Procedures.PTC_CREATE, parameters);
			}
			DBConnection.get_connection().commit();
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return -1;
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return -1;
		}
		Long valor = (Long) response.data.get(0).get(0);
		return valor.intValue();
	}
	
	public ProcedureResponse listProjects()
	{
		Vector<String> parameters = new Vector<String>();
		Integer id = new Integer(getUser().user.get_usr_identificador());
		parameters.add(id.toString());
		
		ProcedureResponse response = new ProcedureResponse();
		try {
			response = DBConnection.get_connection().run_procedure(Procedures.PRO_SEARCH_USR, parameters);
		} catch (SQLException e) {
			
			e.printStackTrace();
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// TODO: fazer a remoção do módulo
		return response;
	}
	
	private ConnectedUser getUser()
	{
		for(int i = 0; i < users_connected.size(); i++)
		{
			if(users_connected.get(i).connection.equals(Red5.getConnectionLocal()))
			{
				return users_connected.get(i);
			}
		}
		return null;
	}
	
	public ProcedureResponse viewProject(int pro_identificador)
	{
		ProcedureResponse falseResponse = new ProcedureResponse();
		falseResponse.fields.add("success");
		List<Object> failure = new ArrayList<Object>();
		failure.add(false);
		falseResponse.data.add(failure);
		
		ProcedureResponse trueResponse = new ProcedureResponse();
		trueResponse.fields.add("success");
		List<Object> success = new ArrayList<Object>();
		success.add(true);
		trueResponse.data.add(success);
		
		// TODO: fazer a entrada da sala
		IConnection conn = Red5.getConnectionLocal();
		
		// Search for user
		ConnectedUser user = null;
		for(int i = 0; i < users_connected.size(); i++)
		{
			if(users_connected.get(i).connection.equals(conn))
			{
				user = users_connected.get(i);
				break;
			}
		}
		
		
		if(user != null)
		{
			int indice = -1;
		
			// Search room for selected project
			for(int i = 0; i < rooms_opened.size(); i++)
			{
				if(rooms_opened.get(i).current_project.get_pro_identificador() == pro_identificador)
				{
					indice = i;
					break;
				}
			}
			
			try 
			{
				
				DBConnection.get_connection().begin();
				Vector<String> paramPtc = new Vector<String>();
				paramPtc.add(Integer.toString(user.user.get_usr_identificador()));
				paramPtc.add(Integer.toString(pro_identificador));
				ProcedureResponse response = DBConnection.get_connection().run_procedure(Procedures.PTC_SEARCH, paramPtc);
			
				// If found room, just add user to the room (only if he is a viewer,
				// else create a room and add him (only if he is the owner) 
				if(indice != -1)
				{
					// If has participation, add him
					// else, send failure (he is forbidden to enter the room)
					if(response.data.size() > 0 && rooms_opened.get(indice).connected_users.indexOf(user) == -1)
						rooms_opened.get(indice).connected_users.add(user);
					else
						return falseResponse;
				}
				else // Create the room
				{
					
					if(response.data.size() != 0)
					{
						int coluna = response.fields.indexOf("pfl_identificador");
						
						// Se ele for o dono do projeto
						if((Integer)response.data.get(0).get(coluna) == 2)
						{
							Room newRoom = new Room();
							newRoom.connected_users.add(user);
							newRoom.current_project = new Project();
							newRoom.current_project.set_pro_identificador(pro_identificador);
							try {
								newRoom.current_project = (Project) Banco.getInstance().repositorio(Banco.PROJECT_STR).adicionarComCuidado(newRoom.current_project);
							} catch (IllegalArgumentException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} catch (RepositoryException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} catch (IllegalAccessException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} catch (InvocationTargetException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
							rooms_opened.add(newRoom);
							
							for(int i = 0; i < response.fields.size();i++)
							{
								trueResponse.fields.add(response.fields.get(i));
								trueResponse.data.get(0).add(response.data.get(0).get(i));
							}
							
						}
						else // Senão não pode abrir a sala
							return falseResponse;
					}
					else
						return falseResponse;
				}
			} catch (SQLException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return falseResponse;
			} catch (ClassNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return falseResponse;
			}
		}
		else
			return falseResponse;
		
		return trueResponse;
	}
	
	public Boolean exitProject(int pro_identificador)
	{
		
		return true;
	}
	
	
	/**
	 * List Participants of current Project
	 * @param pro_identificador int the id of the project
	 * @return a ProcedureResponse with users
	 * */
	public ArrayList<ProcedureResponse> listParticipants(int pro_identificador)
	{
		ProcedureResponse response = new ProcedureResponse();
		response.fields.add("usr_identificador");
		response.fields.add("usr_name");
		response.fields.add("usr_login");
		response.fields.add("usr_password");
		response.fields.add("usr_email");
		
		ArrayList<ProcedureResponse> retorno = new ArrayList<ProcedureResponse>();
		for(int i = 0; i < rooms_opened.size(); i++)
		{
			if(rooms_opened.get(i).current_project.get_pro_identificador() == pro_identificador)
			{
				try {
					DBConnection.get_connection().begin();
					ProcedureResponse responseDBTotal = new ProcedureResponse();
					retorno.add(response);
					retorno.add(responseDBTotal);
					for(int j = 0; j < rooms_opened.get(i).connected_users.size(); j++)
					{
						User usuario = rooms_opened.get(i).connected_users.get(j).user;
						List<Object> linha = new ArrayList<Object>();
						linha.add(usuario.get_usr_identificador());
						linha.add(usuario.get_usr_name());
						linha.add(usuario.get_usr_login());
						linha.add(usuario.get_usr_password());
						linha.add(usuario.get_usr_email());
						response.data.add(linha);
						
						Vector<String> paramPtc = new Vector<String>();
						paramPtc.add(Integer.toString(usuario.get_usr_identificador()));
						paramPtc.add(Integer.toString(pro_identificador));
						ProcedureResponse responseDB = DBConnection.get_connection().run_procedure(Procedures.PTC_SEARCH, paramPtc);
						
						responseDBTotal.data.add(responseDB.data.get(0));
						if(j == 0) responseDBTotal.fields = responseDB.fields;
					}
				} catch (SQLException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (ClassNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				break;
			}
		}
		return retorno;
	}
	
	public Boolean changeUserName()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
	public Boolean changeUserRole()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
	public Boolean loadModel()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
	public Boolean addModule()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
	public Boolean moveModule()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
	public Boolean removeModule()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
	public Boolean undoState()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
	public Boolean setCurrentState()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
	public Boolean removeState()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
	public Boolean simulateModel()
	{
		// TODO: fazer a remoção do módulo
		return true;
	}
	
}


