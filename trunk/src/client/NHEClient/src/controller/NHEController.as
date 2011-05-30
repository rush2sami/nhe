package controller
{
	import controller.server.ServerController;
	import controller.world.WorldController;
	
	import flash.display.Stage;
	
	import view.NHEView;

	public class NHEController
	{
		private var stage:Stage;
		private var _pro_nhe_view:NHEView;
		
		// Controllers
		private var pro_server_controller:ServerController;
		private var pro_world_controller:WorldController;
		
		/*------------------------------------
		* 			PUBLIC METHODS
		-----------------------------------*/
		public function NHEController(stage:Stage)
		{
			this.stage = stage;
		}
		
		
		public function start():void
		{
			create_controllers();
		}
		
		/*------------------------------------
		* 			  PROPERTIES
		-----------------------------------*/
		public function get pro_nhe_view():NHEView
		{
			return _pro_nhe_view;
		}
		
		/*------------------------------------
		* 		   PRIVATE METHODS
		-----------------------------------*/
		private function create_controllers():void
		{
			pro_server_controller = new ServerController();
			pro_world_controller = new WorldController(stage);
			pro_db_controller = new db
		}
		

	}
}