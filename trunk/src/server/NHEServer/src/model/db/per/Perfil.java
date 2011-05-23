package model.bd.per{	import marcozero.marzsdk.bd.MZOperators;	import marcozero.marzsdk.bd.IMZTable;	import flash.utils.getQualifiedClassName;	public class Perfil implements IPerfil{		private var _per_identificador:Number;		private var _per_descricao:Number;		public function Perfil(){}		public function get per_identificador():Number		{			return _per_identificador;		}		public function get per_descricao():Number		{			return _per_descricao;		}		public function set per_identificador(value:Number):void		{			this._per_identificador=value;		}		public function set per_descricao(value:Number):void		{			this._per_descricao=value;		}		public function get identificador():Number		{			return per_identificador;		}		public function matches(elemento:IMZTable):Boolean
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
