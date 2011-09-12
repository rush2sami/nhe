package br.poli.ecomp.geav.nhe.model.db.att;

import br.poli.ecomp.geav.nhe.model.db.Table;

public class AtomicType extends Table implements IAtomicType
{
	private int _att_identificador;
	private String _att_name;
	private String _att_description;
	
	public AtomicType()
	{
		
	}
	
	@Override
	public String get_att_description() {
		return _att_description;
	}

	@Override
	public int get_att_identificador() {
		return _att_identificador;
	}

	@Override
	public String get_att_name() {
		return _att_name;
	}

	@Override
	public void set_att_description(String value) {
		_att_description = value;
		
	}

	@Override
	public void set_att_identificador(int value) {
		_att_identificador = value;
		
	}

	@Override
	public void set_att_name(String value) {
		_att_name = value;
	}

	@Override
	public int get_identificador() {
		return this._att_identificador;
	}

}
