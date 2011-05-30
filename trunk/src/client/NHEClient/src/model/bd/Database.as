package model.bd
{
	import marcozero.marzsdk.bd.MZDatabase;
	import model.bd.act.Action;
	import model.bd.cmp.Composition;
	import model.bd.pfl.Profile;
	import model.bd.pro.Project;
	import model.bd.ptc.Participation;
	import model.bd.atp.ActionType;
	import model.bd.usr.User;
	public class Database extends MZDatabase
	{
		public static const ACT:String = "acr";
		public static const CMP:String = "cmp";
		public static const PFL:String = "pfl";
		public static const PRO:String = "pro";
		public static const PTC:String = "ptc";
		public static const ATP:String = "atp";
		public static const USR:String = "usr";
		public static const ACTION_STR:String = "Action";
		public static const COMPOSITION_STR:String = "Composition";
		public static const PROFILE_STR:String = "Profile";
		public static const PROJECT_STR:String = "Project";
		public static const PARTICIPATION_STR:String = "Participation";
		public static const ACTIONTYPE_STR:String = "ActionType";
		public static const USER_STR:String = "User";
		public static const ACTION:Class = Action;
		public static const COMPOSITION:Class = Composition;
		public static const PROFILE:Class = Profile;
		public static const PROJECT:Class = Project;
		public static const PARTICIPATION:Class = Participation;
		public static const ACTIONTYPE:Class = ActionType;
		public static const USER:Class = User;
		public function Database()
		{
			criarRepositorio(ACT,ACTION_STR,ACTION);
			criarRepositorio(CMP,COMPOSITION_STR,COMPOSITION);
			criarRepositorio(PFL,PROFILE_STR,PROFILE);
			criarRepositorio(PRO,PROJECT_STR,PROJECT);
			criarRepositorio(PTC,PARTICIPATION_STR,PARTICIPATION);
			criarRepositorio(ATP,ACTIONTYPE_STR,ACTIONTYPE);
			criarRepositorio(USR,USER_STR,USER);
		}
	}
}
