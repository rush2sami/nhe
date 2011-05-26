package model.events
{
	import flash.events.Event;
	
	public class StateEvent extends Event
	{
		public static const REMOVED:String = "REMOVED";
		public static const ADDED:String = "ADDED";
		public static const ACTION_CHANGED:String = "ACTION_CHANGED";
		
		
		public function StateEvent(type:String, bubbles:Boolean=false, cancelable:Boolean=false)
		{
			super(type, bubbles, cancelable);
		}
	}
}