package controller.server
{
	import appkit.responders.NResponder;
	
	import flash.events.DataEvent;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.net.Socket;
	
	import model.server.ServerConnection;
	import model.server.ServerDefaults;
	import model.server.ServerRequest;
	import model.server.ServerResponse;

	public class ServerController
	{
		private var _prn_server_connection:ServerConnection;
		
		public function ServerController()
		{
			
		}
		
		public function get prn_server_connection():ServerConnection
		{
			return _prn_server_connection;
		}

		public function connect():void
		{
			if(_prn_server_connection == null)
			{
				_prn_server_connection = new ServerConnection(ServerDefaults.HOST, ServerDefaults.PORT);
				
				NResponder.addNative(_prn_server_connection, Event.CONNECT			, server_connection_connect_event);
				NResponder.addNative(_prn_server_connection, IOErrorEvent.IO_ERROR	, server_connection_ioerror_event);
				NResponder.addNative(_prn_server_connection, DataEvent.DATA			, server_connection_data_event);
				NResponder.addNative(_prn_server_connection, Event.CLOSE			, server_connection_close_event);
			}
			_prn_server_connection.connect();
		}
		
		public function disconnect():void
		{
			
		}
		
		public function request_send(request:ServerRequest):void
		{
			prn_server_connection.send(request);
		}
		
		private function server_connection_connect_event(e:Event):void
		{
			
		}
		
		private function server_connection_ioerror_event(e:IOErrorEvent):void
		{
			
		}
		
		private function server_connection_data_event(e:DataEvent):void
		{
			
		}
		
		private function server_connection_close_event(e:Event):void
		{
			
		}
		
	}
}