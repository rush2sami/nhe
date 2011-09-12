package br.poli.ecomp.geav.nhe.model.db.acd;

import br.poli.ecomp.geav.nhe.model.db.Table;
import br.poli.ecomp.geav.nhe.model.db.act.IAction;
import br.poli.ecomp.geav.nhe.model.db.por.IPort;

public class ActionDisconnect extends Table implements IActionDisconnect
{
	private int _acd_identificador;
	private IAction _act_identificador;
	private IPort _por_destiny;
	private IPort _por_origin;
	
	
	public ActionDisconnect()
	{
		
	}
	
	

	@Override
	public int get_acd_identificador() {
		// TODO Auto-generated method stub
		return _acd_identificador;
	}

	@Override
	public IAction get_act_identificador() {
		// TODO Auto-generated method stub
		return _act_identificador;
	}

	@Override
	public IPort get_por_destiny() {
		// TODO Auto-generated method stub
		return _por_destiny;
	}

	@Override
	public IPort get_por_origin() {
		// TODO Auto-generated method stub
		return _por_origin;
	}

	@Override
	public void set_acd_identificador(int value) {
		_acd_identificador = value;
		
	}

	@Override
	public void set_act_identificador(IAction value) {
		_act_identificador = value;
	}

	@Override
	public void set_por_destiny(IPort value) {
		_por_destiny = value;
	}

	@Override
	public void set_por_origin(IPort value) {
		_por_origin = value;
	}



	@Override
	public int get_identificador() {
		// TODO Auto-generated method stub
		return this._acd_identificador;
	}

}
