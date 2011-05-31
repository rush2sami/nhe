package controller.augmentedreality
{
	import appkit.responders.NResponder;
	
	import controller.IController;
	import controller.action.ActionController;
	import controller.action.Movement;
	
	import model.augmentedreality.ARMovement;
	
	import view.augmentedreality.ARDetector;
	import view.augmentedreality.ARView;
	
	public class ARController implements IController
	{
		private var pro_ar_detector:ARDetector;
		private var pro_ar_view:ARView;
		
		private var pro_action_controller:ActionController;
		
		public function ARController()
		{
			
		}
		
		public function setup()
		{
			NResponder.add(ARDetector.MOVEMENT_DETECTED, ardetector_movement_detected_event);
		}
		
		private function ardetector_movement_detected_event(movement:ARMovement):void
		{
			
		}
	}
}