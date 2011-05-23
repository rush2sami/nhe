package model.bd.ptc{	import model.bd.pro.IProjeto;	import model.bd.usu.IUsuario;	import model.bd.per.IPerfil;	import marcozero.marzsdk.bd.MZOperators;	import marcozero.marzsdk.bd.IMZTable;	import flash.utils.getQualifiedClassName;	public class Participacao implements IParticipacao{		private var _ptc_identificador:Number;		private var _pro_identificador:IProjeto;		private var _usu_identificador:IUsuario;		private var _per_identificador:IPerfil;		public function Participacao(){}		public function get ptc_identificador():Number		{			return _ptc_identificador;		}		public function get pro_identificador():IProjeto		{			return _pro_identificador;		}		public function get usu_identificador():IUsuario		{			return _usu_identificador;		}		public function get per_identificador():IPerfil		{			return _per_identificador;		}		public function set ptc_identificador(value:Number):void		{			this._ptc_identificador=value;		}		public function set pro_identificador(value:IProjeto):void		{			this._pro_identificador=value;		}		public function set usu_identificador(value:IUsuario):void		{			this._usu_identificador=value;		}		public function set per_identificador(value:IPerfil):void		{			this._per_identificador=value;		}		public function get identificador():Number		{			return ptc_identificador;		}		public function matches(elemento:IMZTable):Boolean
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
