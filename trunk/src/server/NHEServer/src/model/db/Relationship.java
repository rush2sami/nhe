/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package model.db;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author Anderson
 */
public class Relationship
{
    private static Relationship relacionamento;
    private ConexaoBD conexao;
    private ResultSet resultado;

    private Relationship()
    {
        try {
            conexao = ConexaoBD.getConexao();
            resultado = conexao.realizarInstrucao("call mysql.ListarEstrangeirasBanco('nhe');", false);
            /*for(int i = 1; i <= resultado.getMetaData().getColumnCount(); i++)
                System.out.printf("%20s",resultado.getMetaData().getColumnLabel(i));
            System.out.print("\n");
            while(resultado.next())
            {
                for(int i = 1; i <= resultado.getMetaData().getColumnCount(); i++)
                    System.out.printf("%20s",resultado.getString(i));
                System.out.print("\n");
            }*/
        } catch (ClassNotFoundException ex) {
            Logger.getLogger(Relationship.class.getName()).log(Level.SEVERE, null, ex);
        } catch (SQLException ex) {
            Logger.getLogger(Relationship.class.getName()).log(Level.SEVERE, null, ex);
        }

    }
    public static Relationship getRelationship()
    {
        if(relacionamento == null)
            relacionamento = new Relationship();
        return relacionamento;
    }
    public static void main(String args[])
    {
        Relationship rel = new Relationship();
        System.out.println(rel.getTabelaEstrangeira("bairro","cidade"));
        System.out.println(rel.getTabelasEstrangeiras("participacao"));
    }

    public boolean ehChaveEstrangeira(String nomeTabela, String campo) {
        return getTabelaEstrangeira(nomeTabela, campo).isEmpty();
    }
    public HashMap<String, ArrayList<String>> getTabelasEstrangeiras(String tabela)
    {
        HashMap<String, ArrayList<String>> estrangeiras = new HashMap<String, ArrayList<String>>();
        estrangeiras.put("tabela", new ArrayList());
        estrangeiras.put("campo", new ArrayList());
        ArrayList<String> tabelas = estrangeiras.get("tabela");
        ArrayList<String> campos = estrangeiras.get("campo");
        try {
            resultado.beforeFirst();
            while(resultado.next())
            {
                if(resultado.getString("Table").equalsIgnoreCase(tabela))
                {
                    tabelas.add(resultado.getString("Parent table"));
                    campos.add(resultado.getString("Parent Key"));
                }
            }
        } catch (SQLException ex) {
            Logger.getLogger(Relationship.class.getName()).log(Level.SEVERE, null, ex);
        }
        return estrangeiras;
    }

    public HashMap<String, String> getTabelaEstrangeira(String tabela, String campo)
    {
        HashMap<String, String> estrangeiras = new HashMap<String, String>();
        try {
            resultado.beforeFirst();
            while(resultado.next())
            {
                if(resultado.getString("Table").equalsIgnoreCase(tabela))
                {
                    if(resultado.getString("Key").equalsIgnoreCase(campo))
                    {
                        estrangeiras.put("tabela", resultado.getString("Parent table"));
                        estrangeiras.put("campo", resultado.getString("Parent Key"));
                        return estrangeiras;
                    }
                }
            }
        } catch (SQLException ex) {x
            Logger.getLogger(Relationship.class.getName()).log(Level.SEVERE, null, ex);
        }
        return estrangeiras;
    }

    public String getCampoEstrangeiro(String tabela, String tabelaEstrangeira, String chaveEstrangeira)
    {
        try {
            resultado.beforeFirst();
            while(resultado.next())
            {
                if(resultado.getString("Parent Table").equalsIgnoreCase(tabelaEstrangeira) &&
                   resultado.getString("Parent Key").equalsIgnoreCase(chaveEstrangeira) &&
                   resultado.getString("Table").equalsIgnoreCase(tabela))
                    return resultado.getString("Key");
            }
        } catch (SQLException ex) {
            Logger.getLogger(Relationship.class.getName()).log(Level.SEVERE, null, ex);
        }
        return null;
    }
}
