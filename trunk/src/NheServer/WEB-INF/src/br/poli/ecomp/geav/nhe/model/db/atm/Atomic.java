package br.poli.ecomp.geav.nhe.model.db.atm;

import br.poli.ecomp.geav.nhe.model.db.Table;
import br.poli.ecomp.geav.nhe.model.db.atp.IActionType;
import br.poli.ecomp.geav.nhe.model.db.cpl.ICoupled;
import br.poli.ecomp.geav.nhe.model.db.mdl.IModel;

public class Atomic extends Table implements IAtomic
{
	private int _atm_identificador;
	private String _atm_description;
	private IActionType _att_identificador;
	private ICoupled _cpl_identificador;
	private IModel _mdl_identificador;
	
	public Atomic()
	{
		
	}
	
	@Override
	public String get_atm_description() {
		return _atm_description;
	}

	@Override
	public int get_atm_identificador() {
		return _atm_identificador;
	}

	@Override
	public IActionType get_att_identificador() {
		return _att_identificador;
	}

	@Override
	public ICoupled get_cpl_identificador() {
		return _cpl_identificador;
	}

	@Override
	public IModel get_mdl_identificador() {
		return _mdl_identificador;
	}

	@Override
	public void set_atm_description(String value) {
		_atm_description = value;
		
	}

	@Override
	public void set_atm_identificador(int value) {
		_atm_identificador = value;
		
	}

	@Override
	public void set_att_identificador(IActionType value) {
		_att_identificador = value;
	}

	@Override
	public void set_cpl_identificador(ICoupled value) {
		_cpl_identificador = value;
	}

	@Override
	public void set_mdl_identificador(IModel value) {
		_mdl_identificador = value;
	}

	@Override
	public int get_identificador() {
		return this._atm_identificador;
	}

}
