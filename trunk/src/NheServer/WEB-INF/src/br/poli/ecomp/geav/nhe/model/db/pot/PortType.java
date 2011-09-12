package br.poli.ecomp.geav.nhe.model.db.pot;

import br.poli.ecomp.geav.nhe.model.db.Table;
import br.poli.ecomp.geav.nhe.model.db.atp.IActionType;

public class PortType extends Table implements IPortType
{
	private int _pot_identificador;
	private IActionType _att_identificador;
	private String _pot_description;
	private int _pot_in;
	
	public PortType()
	{
		
	}
	
	@Override
	public IActionType get_att_identificador() {
		return _att_identificador;
	}

	@Override
	public String get_pot_description() {
		return _pot_description;
	}

	@Override
	public int get_pot_identificador() {
		return _pot_identificador;
	}

	@Override
	public int get_pot_in() {
		return _pot_in;
	}

	@Override
	public void set_att_identificador(IActionType value) {
		_att_identificador = value;
	}

	@Override
	public void set_pot_description(String value) {
		_pot_description = value;
	}

	@Override
	public void set_pot_identificador(int value) {
		_pot_identificador = value;
	}

	@Override
	public void set_pot_in(int value) {
		_pot_in = value;
	}

	@Override
	public int get_identificador() {
		return this._pot_identificador;
	}

}
