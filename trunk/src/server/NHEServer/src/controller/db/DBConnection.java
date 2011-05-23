/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package bancodados;

import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;

/**
 *
 * @author atavares
 */
public class DBConnection {
    Connection conexao;
    Statement instrucao;
    ResultSet resultado, chaves;
    
    
    private static DBConnection conexaoBD;

    private DBConnection() throws ClassNotFoundException, SQLException
    {
        Class.forName("com.mysql.jdbc.Driver");
        criarConexao();
        
    }

    private void criarConexao() throws SQLException
    {
        String connectionUrl = "jdbc:mysql://127.0.0.1/nhe";
        conexao = (Connection) DriverManager.getConnection(connectionUrl,"root","");
        instrucao = conexao.createStatement();
    }
    public static DBConnection getConexao() throws ClassNotFoundException, SQLException
    {
        if (conexaoBD == null)
            conexaoBD = new DBConnection();
        return new DBConnection();
    }

    public ResultSet realizarInstrucao(String sql,boolean update) throws SQLException
    {
        try
        {
            if(conexao != null)
            {
                if(update)
                {
                    instrucao.executeUpdate(sql);

                    return null;
                }
                else
                    return instrucao.executeQuery(sql);
            }
            return null;
        }
        catch(SQLException e)
        {
            if(e.getMessage().contains("last packet"))
            {
                criarConexao();
                return realizarInstrucao(sql, update);
            }
            else
            {
                throw e;
            }
        }
    }
    public void rollback() throws SQLException
    {
        conexao.setAutoCommit(false);
        conexao.rollback();
        conexao.setAutoCommit(true);
    }
    public void commit() throws SQLException
    {
        conexao.setAutoCommit(false);
        conexao.commit();
        conexao.setAutoCommit(true);
    }
    public void begin() throws SQLException
    {
        realizarInstrucao("begin", true);
    }
}
