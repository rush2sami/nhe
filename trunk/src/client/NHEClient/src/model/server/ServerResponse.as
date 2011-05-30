package model.server
{
	public class ServerResponse
	{
		private var prn_response_id:Number;
		private var prn_function_id:Number;
		private var prn_parameters:Vector.<String>;
		
		public function ServerResponse(data:String)
		{
			fill_response(data);
		}
		
		/**
		 * Fill the data of the response
		 * Fill the response id
		 * Fill the function id
		 * Fill the parameters 
		 * @param data
		 * 
		 */
		private function fill_response(data:String):void
		{
			
			var fields:Array = data.split(ServerDefaults.SEPARATOR_FIELDS);
			
			
			prn_response_id = Number(fields[0]);
			prn_function_id = Number(fields[i]);
			
			prn_parameters = new Vector.<String>();
			for(var i:uint = 0; i < fields.length - 2; i++)
			{
				prn_parameters.push(fields[i+2]);
			}
		}
	}
}