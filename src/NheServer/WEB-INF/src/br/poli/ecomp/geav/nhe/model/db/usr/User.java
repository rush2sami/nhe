package br.poli.ecomp.geav.nhe.model.db.usr;

import br.poli.ecomp.geav.nhe.model.db.Table;

public class User extends Table implements IUser
{
	
	private int _usr_identificador;
	private String _usr_name;
	private String _usr_login;
	private String _usr_password;
	private String _usr_email;
	
	public User() {
		// TODO Auto-generated constructor stub
	}

	@Override
	public String get_usr_email() {
		return this._usr_email;
	}

	@Override
	public int get_usr_identificador() {
		return this._usr_identificador;

	}

	@Override
	public String get_usr_login() {
		return this._usr_login;
	}

	@Override
	public String get_usr_name() {
		return this._usr_name;
	}

	@Override
	public String get_usr_password() {
		return this._usr_password;
	}

	@Override
	public void set_usr_email(String value) {
		this._usr_email = value;

	}

	@Override
	public void set_usr_identificador(int value) {
		this._usr_identificador = value;

	}

	@Override
	public void set_usr_login(String value) {
		this._usr_login = value;

	}

	@Override
	public void set_usr_name(String value) {
		this._usr_name = value;

	}

	@Override
	public void set_usr_password(String value) {
		this._usr_password = value;

	}

	@Override
	public int get_identificador() {
		// TODO Auto-generated method stub
		return this._usr_identificador;
	}
}
