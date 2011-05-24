package
{
	import flash.display.Sprite;
	
	import view.ARWorld;
	
	public class NHEClient extends Sprite
	{
		private var arworld:ARWorld;
		public function NHEClient()
		{
			arworld = new ARWorld();
			addChild(arworld);
		}
	}
}