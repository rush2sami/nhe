/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package model.bd;

import bancodados.Relacionamentos;
import bancodados.TiposCampos;
import constantes.Separadores;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;

/**
 *
 * @author Anderson
 */
public class Registro implements Serializable{
    /**
	 * 
	 */
	private static final long serialVersionUID = -3648255978711185447L;
	private String nomeTabela;
    private HashMap<String,Object> campos=new HashMap<String, Object>();
    private ArrayList<String> chavesPrimarias;
    private TiposCampos tiposCampos;


    /**
     * @return the nomeTabela
     */
    
    public Registro(){
    	super();
//    	this.nomeTabela = "";
        //this.campos = new HashMap<String, Object>();
//        this.chavesPrimarias = new Vector();
    }

    public Registro(String nomeDaTabela)
    {
        this.campos = new HashMap<String, Object>();
        if(nomeTabela == null)  nomeTabela  = nomeDaTabela;
        if(tiposCampos == null) tiposCampos = new TiposCampos(nomeTabela);
        if(chavesPrimarias == null) chavesPrimarias = tiposCampos.getChavesPrimarias();
    }
    public Registro(Registro registro)
    {
        this.setCampos(registro.getCampos());
    }

    public String getNomeTabela() {
        return nomeTabela;
    }

    public Boolean match(Registro criterio, Boolean operador, Boolean parcial) {
        Boolean opAnd = true, opOr = false;
        for(String campo: criterio.getCampos().keySet())
        {
            if(criterio.get(campo) instanceof String)
            {
                String valorCri = (String)criterio.get(campo);
                String valorReg = (String)get(campo);
                if(valorReg.indexOf(valorCri) >= 0 && parcial ||
                   valorReg.equals(valorCri) && !parcial)
                    opOr = true;
                else
                    opAnd = false;
            }
            else if(criterio.get(campo) instanceof Integer ||
                    criterio.get(campo) instanceof Double ||
                    criterio.get(campo) instanceof Float)
            {
                if(get(campo).toString().equals(criterio.get(campo).toString()))
                    opOr = true;
                else
                    opAnd = false;
            }
            else if(criterio.get(campo) instanceof java.sql.Date)
            {
                java.sql.Date dataCri = (java.sql.Date)criterio.get(campo);
                java.sql.Date dataReg = (java.sql.Date)get(campo);
                if(dataReg.toString().indexOf(dataCri.toString()) >= 0)
                {
                    opOr = true;
                }
                else
                {
                    opAnd = false;
                }
            }
        }
        return (operador && opAnd) || (!operador && opOr);

    }

    /**
     * @param nomeTabela the nomeTabela to set
     */
    public void setNomeTabela(String nomeDaTabela) {
        nomeTabela = nomeDaTabela;
    }

    /**
     * @return the campos
     */
    public HashMap<String, Object> getCampos() {
        return campos;
    }


    /*public String getNomesCampos(String separador, boolean comTabela)
    {
        return getNomesCampos(separador, comTabela, true, new boolean[]{true}, 0, new boolean[]{true},0);
    }

    public String getNomesOriginais(String separador)
    {
        String nomes = getNomesCampos(separador, true);
        String resultado = "";
        String[] campoArray = nomes.split(separador);
        for(int i = 0; i < campoArray.length; i++)
        {
            String tabela = campoArray[i].split("\\.")[0];
            String nomeCampo = campoArray[i].split("\\.")[1];
            if(!tabela.equalsIgnoreCase(getNomeTabela()))
            {
                nomeCampo = Relacionamentos.getRelacionamentos().getCampoEstrangeiro(getNomeTabela(), tabela, nomeCampo);
            }
            resultado += nomeCampo + separador;
        }
        resultado = resultado.substring(0,resultado.length() - separador.length());
        return resultado;
    }

    public String[] getValoresEstrangeiros(String campo)
    {
        String[] resultados = new String[2];



        return resultados;
    }

    public String getValoresOriginais(String separador)
    {

        String nomes = getNomesOriginais(separador);

        String resultado = "";
        String[] campoArray = nomes.split(separador);
        for(int i = 0; i < campoArray.length; i++)
        {
            String tabela = campoArray[i].split("\\.")[0];
            String nomeCampo = campoArray[i].split("\\.")[1];
            if(!tabela.equalsIgnoreCase(getNomeTabela()))
            {
                nomeCampo = Relacionamentos.getRelacionamentos().getCampoEstrangeiro(getNomeTabela(), tabela, nomeCampo);
            }
            resultado += nomeCampo + separador;
        }
        resultado = resultado.substring(0,resultado.length() - separador.length());
        return resultado;
    }

    public String getValorEstrangeiro(String campo, String separador)
    {
        String resultado = "";
        if(get(campo) instanceof Registro)
        {
            Registro subRegistro = (Registro)get(campo);
            for(int i = 0; i < subRegistro.getChavesPrimarias().size(); i++)
            {
                resultado += subRegistro.getValorEstrangeiro(subRegistro.getChavesPrimarias().get(i),separador) + separador;
            }
        }
        else
        {
            resultado += get(campo) + separador;
        }
        resultado = resultado.substring(0, resultado.length() - separador.length());
        return resultado;
    }

    public String getNomesCampos(String separador, boolean comTabela, boolean comChavesPrimarias, boolean[] comChavesEstrangeiras, int nivelAtual, boolean[] comCamposNormais, int nivelAtualNormal)
    {
        String nomes = "";
        String nomeTabelaAtual = (comTabela) ? nomeTabela+"." : "";
        Set<String> nomesCampos= campos.keySet();
        for(String nomeCampo : nomesCampos)
        {
            
            //Se for chave prim�ria
            if(chavesPrimarias.contains(nomeCampo))
            {
                if(nivelAtual == 0 && comChavesPrimarias)
                    nomes += nomeTabelaAtual + nomeCampo + separador;
                else if(comChavesEstrangeiras[nivelAtual-1])
                    nomes += nomeTabelaAtual + nomeCampo + separador;
            }
            //Se for chave estrangeira
            else if(campos.get(nomeCampo) instanceof Registro)
            {
                boolean testarChaveEstrangeira = (nivelAtual >= comChavesEstrangeiras.length) ? false : comChavesEstrangeiras[nivelAtual];
                if(testarChaveEstrangeira)
                {
                    Registro campoRegistro = (Registro)campos.get(nomeCampo);
                    nomes += campoRegistro.getNomesCampos(separador,comTabela, comChavesPrimarias, comChavesEstrangeiras, nivelAtual+1, comCamposNormais,nivelAtualNormal+1) + separador;
                }
            }
            //Se for campo normal
            else
            {
                boolean testeNivelAtual = (nivelAtualNormal >= comCamposNormais.length) ? false : comCamposNormais[nivelAtualNormal];
                if(testeNivelAtual)
                {
                    nomes += nomeTabelaAtual + nomeCampo + separador;
                }
            }

        }
        if(!nomes.equals(""))
            nomes = nomes.substring(0, nomes.length() - separador.length());
        return nomes;
    }
    
    public String getValoresCampos(String separador)
    {
        return getValoresCampos(separador, true, new boolean[]{true}, 0, new boolean[]{true},0);
    }
    
    public String getValoresCampos(String separador, boolean comChavesPrimarias, boolean[] comChavesEstrangeiras, int nivelAtual, boolean[] comCamposNormais, int nivelAtualNormal)
    {
        String valores = "";
        Set<String> nomesCampos= campos.keySet();
        for(String nomeCampo : nomesCampos)
        {

            //Se for chave prim�ria
            if(chavesPrimarias.contains(nomeCampo))
            {
                if(nivelAtual == 0 && comChavesPrimarias)
                    valores += toString(nomeCampo) + separador;
                else if(comChavesEstrangeiras[nivelAtual-1])
                    valores += toString(nomeCampo) + separador;
            }
            //Se for chave estrangeira
            else if(campos.get(nomeCampo) instanceof Registro)
            {
                boolean testarChaveEstrangeira = (nivelAtual >= comChavesEstrangeiras.length) ? false : comChavesEstrangeiras[nivelAtual];
                if(testarChaveEstrangeira)
                {
                    Registro campoRegistro = (Registro)campos.get(nomeCampo);
                    valores += campoRegistro.getValoresCampos(separador, comChavesPrimarias, comChavesEstrangeiras, nivelAtual+1, comCamposNormais,nivelAtualNormal+1) + separador;
                }
            }
            //Se for campo normal
            else
            {
                boolean testeNivelAtual = (nivelAtualNormal >= comCamposNormais.length) ? false : comCamposNormais[nivelAtualNormal];
                if(testeNivelAtual)
                {
                    valores += toString(nomeCampo) + separador;
                }
            }

        }
        if(!valores.equals(""))
            valores = valores.substring(0, valores.length() - separador.length());
        return valores;
    }*/

    public HashMap<String,String> getNomesValores(String separador, boolean comPK, boolean comFK, boolean comNormal)
    {
        HashMap<String,String> resultado = new HashMap<String, String>();
        String chaves = "";
        String valores = "";

        for(String campo : campos.keySet())
        {
            if(ehChavePrimaria(campo) && comPK ||
               ehChaveEstrangeira(campo) && comFK ||
               ehChaveNormal(campo) && comNormal)
            {
                chaves += campo + separador;
                valores += toString(campo) + separador;
            }
            

            
        }
        chaves = chaves.substring(0, chaves.length() - separador.length());
            valores = valores.substring(0, valores.length() - separador.length());
        resultado.put("chave", chaves);
        resultado.put("valor", valores);
        return resultado;
    }

    public String toString(String campo)
    {
        String aspas;
        if(get(campo) instanceof String ||
           get(campo) instanceof java.sql.Date ||
           get(campo) instanceof java.sql.Time)
            aspas = "'";
        else
            aspas = "";
        return aspas + get(campo) + aspas;
    }

    /**
     * @param campos the campos to set
     */
    public void setCampos(HashMap<String, Object> campos) {
        this.campos = campos;
    }

    public Object get(String campo)
    {
        return campos.get(campo);
    }
    public void set(String campo, Object valor)
    {
        campos.put(campo, valor);
    }

    /**
     * @return the chavesPrimarias
     */
    public ArrayList<String> getChavesPrimarias() {
        return chavesPrimarias;
    }

    /**
     * @param chavesPrimarias the chavesPrimarias to set
     */
    public void setChavesPrimarias(ArrayList chavesPrimariasTmp) {
        chavesPrimarias = chavesPrimariasTmp;
    }
    public void adicionarChavePrimaria(String chavePrimaria)
    {
        chavesPrimarias.add(chavePrimaria);
    }
    public void removerChavePrimaria(String chavePrimaria)
    {
        chavesPrimarias.remove(chavePrimaria);
    }
    public Object[] toObject()
    {
        return null;
        /*Vector vetor = new Vector();
        for(Object item : campos.keySet())
        {

        }*/
    }
    public static void main(String args[])
    {

        Registro registro = new Registro("participacao");

        registro.set("projeto_sala",new Registro("projeto_sala"));
        Registro subRegistro = (Registro)registro.get("projeto_sala");
        subRegistro.set("sala", new Registro());
        Registro subSubRegistro = (Registro)subRegistro.get("sala");
        subSubRegistro.set("id", 1);
        subRegistro.set("projeto", new Registro());
        subSubRegistro = (Registro)subRegistro.get("projeto");
        subSubRegistro.set("id", 1);
        registro.set("usuario", new Registro("usuario"));

        subRegistro = (Registro)registro.get("usuario");

        subRegistro.preencherRegistro("usuario.id,usuario.nome", "2,'Anderson'");
        subRegistro.set("bairro", new Registro("bairro"));
        subSubRegistro = (Registro)subRegistro.get("bairro");
        subSubRegistro.preencherRegistro("bairro.id,bairro.nome", "1,'Boa Vista'");
        subSubRegistro.set("cidade", new Registro("cidade"));
        Registro subSubSubRegistro = (Registro)subSubRegistro.get("cidade");
        subSubSubRegistro.preencherRegistro("cidade.id,cidade.nome", "1,'Recife'");
        //System.out.println(registro.getNomesCampos(",", true, true, new boolean[]{true}, 0, new boolean[]{true}, 0));
        //System.out.println(registro.getValoresCampos(",", true, new boolean[]{true}, 0, new boolean[]{true}, 0));
        System.out.println(registro.ehChavePrimaria("id"));
        System.out.println(subRegistro.getChavesPrimarias());
        //System.out.println(registro.getNomesOriginais(","));
    }

    //Preencher Registro com valores dentro de uma string
    public void preencherRegistro(String campos,String valores)
    {
        String[] campoArray = campos.split(",");
        String[] valorArray = valores.split(",");
        for(int i = 0; i < campoArray.length; i++)
        {
            String tabelaAtual = campoArray[i].split("\\.")[0];
            String campoAtual = campoArray[i].split("\\.")[1];

            if(tabelaAtual.equalsIgnoreCase(getNomeTabela()))
            {
                set(campoAtual, tiposCampos.parseValor(campoAtual, tiposCampos.tirarAspas(campoAtual, valorArray[i])));
            }
        }
    }

    //Retorna true se o campo for chave prim�ria da tabela do Registro
    public boolean ehChavePrimaria(String campo)
    {
        return tiposCampos.ehChavePrimaria(campo);
    }

    public boolean ehChaveEstrangeira(String campo)
    {
        return Relacionamentos.getRelacionamentos().ehChaveEstrangeira(getNomeTabela(), campo);

    }

    public boolean ehChaveNormal(String campo)
    {
        return !ehChaveEstrangeira(campo) && !ehChavePrimaria(campo);
    }
    //Converte um valor (em texto) de um campo no seu tipo adequado
    // varchar -> String
    // int, double, float -> Integer, Double, Float
    // date, time -> java.sql.Date, java.sql.Time
    public Object parseValor(String campo, String valor)
    {
        return tiposCampos.parseValor(campo, valor);
    }
    @Override
    public String toString()
    {
        String resultado = "";
        for(String campo:campos.keySet())
        {
            resultado += campo + ": " + campos.get(campo) + "\n";
        }
        return resultado;
    }

    /*public static ArrayList converterParaArray(String texto,String[] props,Object[] tipos,Class classe)
    {
        String sepObj = Separadores.OBJETOS;
        String sepDel = Separadores.DELIMITADORES;
        String sepCmp = Separadores.CAMPOS;

        String[] resultado = texto.split(sepObj);
        var lista:Array = new Array();
        var objetoTmp:Object;
        for(var i:int = 0; i < resultado.length ; i++)
        {
            objetoTmp = new classe();
            for(var j:int = 0; j < props.length; j++)
            {
                var campos:Array = resultado[i].split(sepCmp);
                if(campos[j] != "")
                {
                    objetoTmp[props[j]] = new tipos[j](campos[j]);
                }
            }
            lista.push(objetoTmp);
        }
        return lista;
    }*/
}
