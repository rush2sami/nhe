package model.db.per;import model.db.Table;public class Perfil extends Table implements IPerfil{	private int per_identificador;	private String per_descricao;	public Perfil(){}		@Override	public String get_per_descricao() {		return this.per_descricao;	}	@Override	public int get_per_identificador() {		return this.per_identificador;	}	@Override	public void set_per_descricao(String value){		this.per_descricao = value;	}		@Override	public void set_per_identificador(int value) {		this.per_identificador = value;	}}
