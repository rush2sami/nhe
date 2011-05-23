package model.connection
{
	/**
	 * Class for  
	 * @author Anderson Tavares
	 * 
	 */
	public class SocketRequest
	{
		private var _pas_func:String;
		private var _pal_parameters:Vector.<String>;
		
		public function SocketRequest()
		{
		}
		
		/*------------------------------------
		* PUBLIC METHODS
		*-----------------------------------*/
		
		
		
		/*------------------------------------
		 * PROPERTIES
		 *-----------------------------------*/
		public function get pas_func():String
		{
			return _pas_func;
		}

		public function set pas_func(value:String):void
		{
			_pas_func = value;
		}

		public function get pal_parameters():Vector.<String>
		{
			return _pal_parameters;
		}

		public function set pal_parameters(value:Vector.<String>):void
		{
			_pal_parameters = value;
		}
		
		/*------------------------------------
		* PRIVATE METHODS
		*-----------------------------------*/
		
		
		/*------------------------------------
		* EVENT HANDLERS
		*-----------------------------------*/

	}
}