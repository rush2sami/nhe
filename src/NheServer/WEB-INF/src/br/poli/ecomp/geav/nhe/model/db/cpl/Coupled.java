package br.poli.ecomp.geav.nhe.model.db.cpl;

import br.poli.ecomp.geav.nhe.model.db.Table;
import br.poli.ecomp.geav.nhe.model.db.mdl.IModel;

public class Coupled extends Table implements ICoupled
{
	private int _cpl_identificador;
	private String _cpl_description;
	private IModel _mdl_identificador;
	public Coupled()
	{
		
	}
	
	

	@Override
	public String get_cpl_description() {
		return _cpl_description;
	}

	@Override
	public int get_cpl_identificador() {
		return _cpl_identificador;
	}

	@Override
	public IModel get_mdl_identificador() {
		return _mdl_identificador;
	}

	@Override
	public void set_cpl_description(String value) {
		_cpl_description = value;
	}

	@Override
	public void set_cpl_identificador(int value) {
		_cpl_identificador = value;
	}

	@Override
	public void set_mdl_identificador(IModel value) {
		_mdl_identificador = value;
	}



	@Override
	public int get_identificador() {
		return this._cpl_identificador;
	}

}
