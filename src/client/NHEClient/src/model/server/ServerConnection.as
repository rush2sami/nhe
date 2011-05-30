package model.server
{
	import appkit.responders.NResponder;
	
	import flash.events.DataEvent;
	import flash.events.Event;
	import flash.events.IOErrorEvent;
	import flash.net.XMLSocket;
	import flash.system.Security;

	[Event(name="connect", type="flash.events.Event")]
	[Event(name="io_error", type="flash.events.IOErrorEvent")]
	[Event(name="data", type="flash.events.DataEvent")]
	[Event(name="close", type="flash.events.Event")]
	public class ServerConnection
	{
		private var socket:XMLSocket;
		private var port:Number;
		private var address:String;
		private var prl_requests:Vector.<ServerRequest>;
		private var prl_responses:Vector.<ServerResponse>;
		
		/*------------------------------------
		* 		   PUBLIC METHODS
		-----------------------------------*/
		public function ServerConnection(address:String, port:Number)
		{
			this.port = port;
			this.address = address;
			this.prl_requests = new Vector.<ServerRequest>();
			this.prl_responses = new Vector.<ServerResponse>();
		}
		
		public function setup():void
		{
			Security.loadPolicyFile(this.address);
			socket = new XMLSocket();
			NResponder.addNative(socket, Event.CONNECT, socket_connect_event);
			NResponder.addNative(socket, IOErrorEvent.IO_ERROR, socket_ioerror_event);
			NResponder.addNative(socket, DataEvent.DATA, socket_data_event);
			NResponder.addNative(socket, Event.CLOSE, socket_close_event);
			prl_requests = new Vector.<ServerRequest>();
			prl_responses = new Vector.<ServerResponse>();
		}
		
		public function connect():void
		{
			socket.connect(address,port);
		}
		
		public function send(request:ServerRequest):void
		{
			socket.send(request.text + "\r");
		}
		
		public function disconnect():void
		{
			socket.close();
		}
		
		/*------------------------------------
		* 		   	 PROPERTIES
		-----------------------------------*/
		public function get connected():Boolean
		{
			return socket.connected;
		}
		
		/*------------------------------------
		* 		   PRIVATE METHODS
		-----------------------------------*/
		private function socket_ioerror_event(e:IOErrorEvent):void
		{
			NResponder.dispatch(e.type);
		}
		
		private function socket_connect_event(e:Event):void
		{
			NResponder.dispatch(e.type);
		}
		
		private function socket_data_event(e:DataEvent):void
		{
			append_responses(e.data);
			NResponder.dispatch(e.type);
		}
		
		private function socket_close_event(e:Event):void
		{
			NResponder.dispatch(e.type);
		}
		
		private function append_responses(data:String):void
		{
			var responses:Array = data.split(ServerDefaults.SEPARATOR_REQUESTS);
			for(var i:uint = 0; i < responses.length; i++)
			{
				prl_requests.push(new ServerResponse(responses[i]));
			}
		}
	}
}