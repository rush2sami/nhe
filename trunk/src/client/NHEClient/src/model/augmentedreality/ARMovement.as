package model.augmentedreality
{
	import controller.action.IMovement;
	
	public class ARMovement implements IMovement
	{
		public static const MOVING:Number = 1;
		public static const TOUCHING:Number = 2;
		
		private var _movement:Number;
		
		public function ARMovement(movement:Number)
		{
			this.movement = movement;
		}

		public function get movement():Number
		{
			return _movement;
		}

	}
}