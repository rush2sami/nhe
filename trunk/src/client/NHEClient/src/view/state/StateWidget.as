package view.state
{
	import controller.state.StateController;
	
	import flash.display.Sprite;
	
	import model.state.State;
	
	import view.ITooltipavel;
	
	public class StateWidget extends Sprite implements ITooltipavel;
	{
		private var sprite:Sprite;
		private var controller:StateController;
		
		
		public function StateWidget()
		{
			super();
		}
		
		/**
		 *  
		 * 
		 */
		public function enable():void
		{
			
		}
		
		/**
		 *  
		 * 
		 */		
		public function disable():void
		{
			
		}
		
		/**
		 *  
		 * 
		 */		
		public function select():void
		{
			
		}
		
		/**
		 *  
		 * 
		 */
		public function unselect():void
		{
			
		}
		
		public function tooltip_enable():void
		{
			
		}
		
		public function tooltip_disable():void
		{
			
		}
		
		public function tooltip_get():Sprite
		{
			
		}
	}
}