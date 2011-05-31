package controller.action
{
	import appkit.responders.NResponder;
	
	import controller.db.DBController;
	import controller.server.ServerController;
	
	import flash.events.Event;
	
	import model.server.ServerConnection;
	import model.server.ServerDefaults;
	import model.server.ServerRequest;
	import model.server.ServerResponse;

	public class ActionController
	{
		private var pro_db_controller:DBController;
		private var pro_server_controller:ServerController;
		
		public function ActionController()
		{
			
		}
		
		public function setup():void
		{
			NResponder.add(ServerConnection.SERVER_CLOSED, server_closed_event);
			NResponder.add(ServerConnection.SERVER_CONNECTED, server_connected_event);
			NResponder.add(ServerConnection.SERVER_IO_ERROR, server_io_error_event);
			NResponder.add(ServerConnection.SERVER_RECEIVED, server_received_event);
			
			pro_db_controller = new DBController();
			pro_server_controller = new ServerController();
		}
		
		private function server_closed_event():void
		{
			
		}
		
		private function server_connected_event():void
		{
			
		}
		
		private function server_io_error_event():void
		{
			
		}
		
		private function server_received_event():void
		{
			while(pro_server_controller.hasResponse)
			{
				var response:ServerResponse = pro_server_controller.nextResponse();
				switch(response.prn_function_id)
				{
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
					case ServerDefaults.RESPONSE_CHAT_RECEIVED:
						break;
				}
			}
		}
	}
}