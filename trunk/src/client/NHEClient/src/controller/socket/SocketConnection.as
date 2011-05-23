package controller.socket
{
	import flash.net.Socket;
	
	import model.connection.SocketRequest;
	import model.connection.SocketResponse;

	public class SocketConnection
	{
		private var socket:Socket;
		
		private var socketRequest:SocketRequest;
		private var socketResponse:SocketResponse;
		
		public function SocketConnection()
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