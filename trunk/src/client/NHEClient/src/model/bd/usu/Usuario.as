package model.bd.usu{import marcozero.marzsdk.bd.MZOperators;import marcozero.marzsdk.bd.IMZTable;import flash.utils.getQualifiedClassName;public class Usuario implements IUsuario{private var _usu_identificador:Number;private var _usu_nome:Number;private var _usu_login:String;private var _usu_senha:String;private var _usu_email:String;public function Usuario(){}public function get usu_identificador():Number{return _usu_identificador;}public function get usu_nome():Number{return _usu_nome;}public function get usu_login():String{return _usu_login;}public function get usu_senha():String{return _usu_senha;}public function get usu_email():String{return _usu_email;}public function set usu_identificador(value:Number):void{this._usu_identificador=value;}public function set usu_nome(value:Number):void{this._usu_nome=value;}public function set usu_login(value:String):void{this._usu_login=value;}public function set usu_senha(value:String):void{this._usu_senha=value;}public function set usu_email(value:String):void{this._usu_email=value;}public function get identificador():Number{return usu_identificador;}public function matches(elemento:IMZTable):Boolean
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
  }}}
