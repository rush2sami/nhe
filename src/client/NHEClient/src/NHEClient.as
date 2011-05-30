package
{
	import appkit.responders.NResponder;
	
	import controller.NHEController;
	
	import flash.display.Sprite;
	import flash.events.Event;
	
	import view.ARWorld;
	import view.NHEView;
	
	public class NHEClient extends Sprite
	{
		private var prn_nhe_controller:NHEController;
		
		//private var arworld:ARWorld;
		public function NHEClient()
		{
			if(stage) init();
			else addEventListener(Event.ADDED_TO_STAGE, init);
		}
		private function init(e:Event = null):void
		{
			prn_nhe_controller = new NHEController(this.stage);
			prn_nhe_controller.start();
		}
	}
}