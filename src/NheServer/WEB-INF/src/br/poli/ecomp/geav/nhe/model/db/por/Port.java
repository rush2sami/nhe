package br.poli.ecomp.geav.nhe.model.db.por;

import br.poli.ecomp.geav.nhe.model.db.Table;
import br.poli.ecomp.geav.nhe.model.db.mdl.IModel;

public class Port extends Table implements IPort
{
	private int _por_identificador;
	private IModel _mdl_identificador;
	private String _por_description;
	private int _por_in;
	private IPort _por_influencee;
	
	public Port()
	{
		
	}
	
	@Override
	public int get_identificador() {
		return this._por_identificador;
	}

	@Override
	public IModel get_mdl_identificador() {
		return _mdl_identificador;
	}

	@Override
	public String get_por_description() {
		// TODO Auto-generated method stub
		return _por_description;
	}

	@Override
	public int get_por_identificador() {
		// TODO Auto-generated method stub
		return _por_identificador;
	}

	@Override
	public int get_por_in() {
		// TODO Auto-generated method stub
		return _por_in;
	}

	@Override
	public IPort get_por_influencee() {
		// TODO Auto-generated method stub
		return _por_influencee;
	}

	@Override
	public void set_mdl_identificador(IModel value) {
		_mdl_identificador = value;
		
	}

	@Override
	public void set_por_description(String value) {
		_por_description = value;
	}

	@Override
	public void set_por_identificador(int value) {
		_por_identificador = value;
	}

	@Override
	public void set_por_in(int value) {
		_por_in = value;
	}

	@Override
	public void set_por_influencee(IPort value) {
		_por_influencee = value;
	}

}
