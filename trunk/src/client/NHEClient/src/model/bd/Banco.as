package model.bd
{
	import marcozero.marzsdk.bd.MZDatabase;
	import model.bd.aca.Acao;
	import model.bd.cmp.Composicao;
	import model.bd.per.Perfil;
	import model.bd.pro.Projeto;
	import model.bd.ptc.Participacao;
	import model.bd.tpa.TipoAcao;
	import model.bd.usu.Usuario;
	public class Banco extends MZDatabase
	{
		public static const ACA:String = "aca";
		public static const CMP:String = "cmp";
		public static const PER:String = "per";
		public static const PRO:String = "pro";
		public static const PTC:String = "ptc";
		public static const TPA:String = "tpa";
		public static const USU:String = "usu";
		public static const ACAO_STR:String = "Acao";
		public static const COMPOSICAO_STR:String = "Composicao";
		public static const PERFIL_STR:String = "Perfil";
		public static const PROJETO_STR:String = "Projeto";
		public static const PARTICIPACAO_STR:String = "Participacao";
		public static const TIPOACAO_STR:String = "TipoAcao";
		public static const USUARIO_STR:String = "Usuario";
		public static const ACAO:Class = Acao;
		public static const COMPOSICAO:Class = Composicao;
		public static const PERFIL:Class = Perfil;
		public static const PROJETO:Class = Projeto;
		public static const PARTICIPACAO:Class = Participacao;
		public static const TIPOACAO:Class = TipoAcao;
		public static const USUARIO:Class = Usuario;
		public function Banco()
		{
			criarRepositorio(ACA,ACAO_STR,ACAO);
			criarRepositorio(CMP,COMPOSICAO_STR,COMPOSICAO);
			criarRepositorio(PER,PERFIL_STR,PERFIL);
			criarRepositorio(PRO,PROJETO_STR,PROJETO);
			criarRepositorio(PTC,PARTICIPACAO_STR,PARTICIPACAO);
			criarRepositorio(TPA,TIPOACAO_STR,TIPOACAO);
			criarRepositorio(USU,USUARIO_STR,USUARIO);
		}
	}
}
