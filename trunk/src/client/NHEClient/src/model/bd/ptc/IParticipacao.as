package model.bd.ptc{	import model.bd.pro.IProjeto;	import model.bd.usu.IUsuario;	import model.bd.per.IPerfil;	import marcozero.marzsdk.bd.MZOperators;	import marcozero.marzsdk.bd.IMZTable;	public interface IParticipacao extends IMZTable	{		function set ptc_identificador(value:Number):void;		function set pro_identificador(value:IProjeto):void;		function set usu_identificador(value:IUsuario):void;		function set per_identificador(value:IPerfil):void;		function get ptc_identificador():Number;		function get pro_identificador():IProjeto;		function get usu_identificador():IUsuario;		function get per_identificador():IPerfil;	}}
