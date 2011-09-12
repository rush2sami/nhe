package br.poli.ecomp.geav.nhe.model.db.mdl;

import br.poli.ecomp.geav.nhe.model.db.Table;

public class Model extends Table implements IModel
{
	private int _mdl_identificador;
	
	public Model()
	{
		
	}
	
	@Override
	public int get_mdl_identificador() {
		return this._mdl_identificador;
	}
	
	@Override
	public void set_mdl_identificador(int value) {
		this._mdl_identificador = value;
	}

	@Override
	public int get_identificador() {
		return this._mdl_identificador;
	}
}
