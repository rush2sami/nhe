package model.bd.tpa{	import marcozero.marzsdk.bd.MZOperators;	import marcozero.marzsdk.bd.IMZTable;	import flash.utils.getQualifiedClassName;	public class TipoAcao implements ITipoAcao{		private var _tpa_identificador:Number;		private var _tpa_nome:Number;		public function TipoAcao(){}		public function get tpa_identificador():Number		{			return _tpa_identificador;		}		public function get tpa_nome():Number		{			return _tpa_nome;		}		public function set tpa_identificador(value:Number):void		{			this._tpa_identificador=value;		}		public function set tpa_nome(value:Number):void		{			this._tpa_nome=value;		}		public function get identificador():Number		{			return tpa_identificador;		}		public function matches(elemento:IMZTable):Boolean
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
