package br.poli.ecomp.geav.nhe.model.db;

import br.poli.ecomp.geav.nhe.model.db.acd.ActionDisconnect;
import br.poli.ecomp.geav.nhe.model.db.aco.ActionConnect;
import br.poli.ecomp.geav.nhe.model.db.act.Action;
import br.poli.ecomp.geav.nhe.model.db.atm.Atomic;
import br.poli.ecomp.geav.nhe.model.db.atp.ActionType;
import br.poli.ecomp.geav.nhe.model.db.att.AtomicType;
import br.poli.ecomp.geav.nhe.model.db.cpl.Coupled;
import br.poli.ecomp.geav.nhe.model.db.mdl.Model;
import br.poli.ecomp.geav.nhe.model.db.pfl.Profile;
import br.poli.ecomp.geav.nhe.model.db.por.Port;
import br.poli.ecomp.geav.nhe.model.db.pot.PortType;
import br.poli.ecomp.geav.nhe.model.db.pro.Project;
import br.poli.ecomp.geav.nhe.model.db.ptc.Participation;
import br.poli.ecomp.geav.nhe.model.db.usr.User;

public class Banco extends Database
{
	private static Banco banco;
	
	public static final String ACD = "acd";
	public static final String ACO = "aco";
	public static final String ACT = "act";
	public static final String ATM = "atm";
	public static final String ATP = "atp";
	public static final String ATT = "att";
	public static final String CPL = "cpl";
	public static final String MDL = "mdl";
	public static final String PFL = "pfl";
	public static final String POR = "por";
	public static final String POT = "pot";
	public static final String PRO = "pro";
	public static final String PTC = "ptc";
	public static final String USR = "usr";
	public static final String ACTIONDISCONNECT_STR = "ActionDisconnect";
	public static final String ACTIONCONNECT_STR = "ActionConnect";
	public static final String ACTION_STR = "Action";
	public static final String ATOMIC_STR = "Atomic";
	public static final String ACTIONTYPE_STR = "ActionType";
	public static final String ATOMICTYPE_STR = "AtomicType";
	public static final String COUPLED_STR = "Coupled";
	public static final String MODEL_STR = "Model";
	public static final String PROFILE_STR = "Profile";
	public static final String PORT_STR = "Port";
	public static final String PORTTYPE_STR = "PortType";
	public static final String PROJECT_STR = "Project";
	public static final String PARTICIPATION_STR = "Participation";
	public static final String USER_STR = "User";
	
	public static final Class<? extends Table> ACTIONDISCONNECT = ActionDisconnect.class;
	public static final Class<? extends Table> ACTIONCONNECT = ActionConnect.class;
	public static final Class<? extends Table> ACTION = Action.class;
	public static final Class<? extends Table> ATOMIC = Atomic.class;
	public static final Class<? extends Table> ATOMICTYPE = AtomicType.class;
	public static final Class<? extends Table> ACTIONTYPE = ActionType.class;
	public static final Class<? extends Table> COUPLED = Coupled.class;
	public static final Class<? extends Table> MODEL = Model.class;
	public static final Class<? extends Table> PROFILE = Profile.class;
	public static final Class<? extends Table> PORT = Port.class;
	public static final Class<? extends Table> PORTTYPE = PortType.class;
	public static final Class<? extends Table> PROJECT = Project.class;
	public static final Class<? extends Table> PARTICIPATION = Participation.class;
	public static final Class<? extends Table> USER = User.class;
	private Banco()
	{
		criarRepositorio(ACD,ACTIONDISCONNECT_STR,ACTION);
		criarRepositorio(ACO,ACTIONCONNECT_STR,ACTIONCONNECT);
		criarRepositorio(ACT,ACTION_STR,ACTION);
		criarRepositorio(ATM,ATOMIC_STR,ATOMIC);
		criarRepositorio(ATP,ACTIONTYPE_STR,ACTIONTYPE);
		criarRepositorio(ATT,ATOMICTYPE_STR,ATOMICTYPE);
		criarRepositorio(CPL,COUPLED_STR,COUPLED);
		criarRepositorio(MDL,MODEL_STR,MODEL);
		criarRepositorio(PFL,PROFILE_STR,PROFILE);
		criarRepositorio(POR,PORT_STR,PORT);
		criarRepositorio(POT,PORTTYPE_STR,PORT);
		criarRepositorio(PRO,PROJECT_STR,PROJECT);
		criarRepositorio(PTC,PARTICIPATION_STR,PARTICIPATION);
		criarRepositorio(USR,USER_STR,USER);
	}
	public static Banco getInstance()
	{
		if(banco == null)
			banco = new Banco();
		return banco;
	}
}
