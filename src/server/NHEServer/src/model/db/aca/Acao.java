package model.bd.aca{	import model.bd.ptc.IParticipacao;	import model.bd.tpa.ITipoAcao;	import model.bd.cmp.IComposicao;	import marcozero.marzsdk.bd.MZOperators;	import marcozero.marzsdk.bd.IMZTable;	import flash.utils.getQualifiedClassName;		public class Acao implements IAcao	{		private var _aca_identificador:Number;		private var _ptc_identificador:IParticipacao;		private var _tpa_identificador:ITipoAcao;		private var _aca_pai:Number;		private var _cmp_identificador:IComposicao;		private var _aca_data_atual:String;				public function Acao()		{		}		public function get aca_identificador():Number		{			return _aca_identificador;		}		public function get ptc_identificador():IParticipacao		{			return _ptc_identificador;		}		public function get tpa_identificador():ITipoAcao		{			return _tpa_identificador;		}		public function get aca_pai():Number		{			return _aca_pai;		}		public function get cmp_identificador():IComposicao		{			return _cmp_identificador;		}		public function get aca_data_atual():String		{			return _aca_data_atual;		}		public function set aca_identificador(value:Number):void		{			this._aca_identificador=value;		}		public function set ptc_identificador(value:IParticipacao):void		{			this._ptc_identificador=value;		}		public function set tpa_identificador(value:ITipoAcao):void		{			this._tpa_identificador=value;		}		public function set aca_pai(value:Number):void		{			this._aca_pai=value;		}		public function set cmp_identificador(value:IComposicao):void		{			this._cmp_identificador=value;		}		public function set aca_data_atual(value:String):void		{			this._aca_data_atual=value;		}		public function get identificador():Number		{			return aca_identificador;		}		public function matches(elemento:IMZTable):Boolean
		{
			if(getQualifiedClassName(elemento) == getQualifiedClassName(this))
			{
				if(IMZTable(elemento).identificador == identificador)
					return true;
				else
					return false;
			}
			else
			{
				return false;
			}
		}
		public function matchesAll(elemento:IMZTable, operacao:Boolean, operadores:Object):Boolean
		{
			return MZOperators.matchesAll(this,elemento,operacao,operadores);
		}	}}
