package controller.state
{
	import flash.events.EventDispatcher;
	
	import view.state.StateTreeWidget;

	public class StateTreeController extends EventDispatcher
	{
		private var _prl_state_controllers:Vector.<StateController>;
		
		private var _pao_state_tree_widget:StateTreeWidget;
		
		public function StateTreeController()
		{
			this._prl_state_controllers = new Vector.<StateController>();
		}

		public function get pao_state_tree_widget():StateTreeWidget
		{
			return _pao_state_tree_widget;
		}

		public function set pao_state_tree_widget(value:StateTreeWidget):void
		{
			_pao_state_tree_widget = value;
		}

		public function get prl_state_controllers():Vector.<StateController>
		{
			return _prl_state_controllers;
		}
	}
}