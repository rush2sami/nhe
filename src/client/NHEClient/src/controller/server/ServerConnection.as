package controller.server
{
	import flash.net.Socket;
	
	import model.server.ServerRequest;
	import model.server.ServerResponse;

	public class ServerConnection
	{
		private var socket:Socket;
		
		private var socketRequest:ServerRequest;
		private var socketResponse:ServerResponse;
		
		public function ServerConnection()
		{
		}
		
		public function connect():void
		{
			socket.connect(
		}
		
		public function disconnect():void
		{
			
		}
		
		public function message_send(str:String):void
		{
			socket.writeUTFBytes(str);
		}
		
		public function request_send():void
		{
			socketRequest.
		}
	}
}