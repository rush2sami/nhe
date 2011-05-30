package model.db.pfl{	import marcozero.marzsdk.bd.MZOperators;	import marcozero.marzsdk.bd.IMZTable;	import flash.utils.getQualifiedClassName;	public class Profile implements IProfile{		private var _pfl_identificador:Number;		private var _pfl_descricao:Number;		public function Profile(){}		public function get pfl_identificador():Number		{			return _pfl_identificador;		}		public function get pfl_descricao():Number		{			return _pfl_descricao;		}		public function set pfl_identificador(value:Number):void		{			this._pfl_identificador=value;		}		public function set pfl_descricao(value:Number):void		{			this._pfl_descricao=value;		}		public function get identificador():Number		{			return pfl_identificador;		}		public function matches(elemento:IMZTable):Boolean
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
