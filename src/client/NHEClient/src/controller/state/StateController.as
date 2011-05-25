package controller.state
{
	import flash.events.EventDispatcher;
	import flash.events.IEventDispatcher;
	
	public class StateController extends EventDispatcher
	{
		public function StateController(target:IEventDispatcher=null)
		{
			super(target);
		}
	}
}