package model.events
{
	import flash.events.Event;
	
	public class SocketEvent extends Event
	{
		public static function REQUESTED:String = "REQUESTED";
		public static function RECEIVED:String = "RECEIVED";
		public static function CLOSED:String = "CLOSED";
		public static function CONNECTED:String = "CONNECTED";
		
		public function SocketEvent(type:String, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
		}
	}
}