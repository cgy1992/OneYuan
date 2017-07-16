package com.verify;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Timestamp;
import java.util.Calendar;
import java.util.Date;
import java.util.Properties;
import java.util.Scanner;

import org.codehaus.xfire.client.XFireProxyFactory;
import org.codehaus.xfire.service.Service;
import org.codehaus.xfire.service.binding.ObjectServiceFactory;

import com.service.CardPrint;
import com.service.CardPrintImpl;

import org.apache.log4j.FileAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.Level;
import org.apache.log4j.SimpleLayout;

import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

public class returnCall {
	

	/*�ն˴�ӡ�����ش�ӡ���*/
	public int returnResult(
			int isOK,    /*�Ƿ�ɹ�*/
			String SBYY,   /*ʧ��ԭ��*/
			String KSBM,		/*��ʶ����*/
			String ATR,	/*ATR*/
			String yhkh, /*���п���*/
			int taskSessionID) throws Exception		/*����sessionID*/
	{
		String taskID;  /*�Զ����ɵ�taskID,����ΪGY+���߼������ӣ�10λ���������Ӳ�����0���룩+10λ��ˮ��*/
	
		Connection sqlConn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		int status;
		Date today = new java.util.Date();  
        Timestamp time; 
				
		sqlConn=CardPrintImpl.getConn();
        try 
        {
			stmt = sqlConn.createStatement();
			if (isOK == 1)
			{
				sql = "update task_info set status = 2 where id = "+taskSessionID;   		
			    stmt.executeUpdate(sql);				
			}
			else if (isOK == 0)
			{
				sql = "update task_info set status = -1 where id = "+taskSessionID;   		
			    stmt.executeUpdate(sql);			    			    
			}
			
			
		        
		} catch (SQLException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			CardPrintImpl.closeConn(sqlConn);
			System.out.println("database error");
			return -1;
		}
		
		CardPrintImpl.closeConn(sqlConn);
		
		/*��ȡ�籣����webservice url�ͷ��ʵ��û��������룬Ϊ�����籣���Ļ�дwebservice�ӿ���׼��*/
		Properties configProperties = new Properties(); 
		String proFilePath = System.getProperty("user.dir") + "\\guanyuan_config.properties";  
		System.out.println(proFilePath);
		InputStream inputStream;
		try {
			inputStream = new BufferedInputStream(new FileInputStream(proFilePath));
			
			try {   
				configProperties.load(inputStream);   
			  } catch (IOException e1) {   
			   e1.printStackTrace();  
			   System.out.println("read config file,ioexception1");
			   return -1;
			  }   
			  
		} catch (FileNotFoundException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
			System.out.println("read config file,ioexception2");
			return -1;
		}  
		 
		  
	    URL wsUrl = new URL(configProperties.getProperty("yinhai_url"));
	    HttpURLConnection conn;
	    System.out.println(configProperties.getProperty("yinhai_url"));
        
        
        try {
        	conn = (HttpURLConnection) wsUrl.openConnection();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.out.println("returnPrintResult:can not connect to yinhai webservice");
			return -1;
		}
		
        String jyls;
        String jyyzm;
        String inputstr = "";
	        	        
        conn.setDoInput(true);
        conn.setDoOutput(true);
        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Type", "text/xml;charset=UTF-8");
        conn.setRequestProperty("SOAPAction", "");
        OutputStream os;
		
		
		try {
			os = conn.getOutputStream();
		} catch (IOException e3) {
			// TODO Auto-generated catch block
			e3.printStackTrace();
			System.out.println("returnPrintResult:webservice outputstream error");
			return -1;
		}
  
		System.out.println("username:"+configProperties.getProperty("username")+",password:"+configProperties.getProperty("password"));
		String username = configProperties.getProperty("username");
		String password = configProperties.getProperty("password");
		
        //�����壬��ȡ������ˮ�ź���֤��
		String soap="<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
		+"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
		+"<soap:Header>"
			+"<in:system xmlns:in=\"http://yinhai.com\">" +
			"<userName>"+username+"</userName>" +
			"<passWord>"+password+"</passWord>" 
			+"<jylsh>0</jylsh>" +
			"<jyyzm>0</jyyzm>" +
			"<jybh>MakeCardResult</jybh>" +
			"</in:system>"
		+"</soap:Header>" +
		"<soap:Body>" +
			"<ns2:getJylsh xmlns:ns2=\"http://webservice.common.yinhai.com/\">"+
			"</ns2:getJylsh>" +
		"</soap:Body>" +
		"</soap:Envelope>;";
		
		System.out.println("first  soap is"+soap);

        os.write(soap.getBytes());
        InputStream is = conn.getInputStream();
        byte[] b = new byte[1024];
        int len = 0;
        String s = "";
        while((len = is.read(b)) != -1){
            String ss = new String(b,0,len,"UTF-8");
            s += ss;
        }
        s=s.replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", "");
        System.out.println(s);
        is.close();
        os.close();
        conn.disconnect();
        
        if (s == "")
        {
        	System.out.println("returnPrintResult:get yinhai yhlsh webservice error");
        	return -1;
        }
   
        /*��ȡ���ص���ˮ���׺ź���֤��*/
        int startIndex = s.indexOf("<jylsh>")+"<jylsh>".length();
        int endIndex = s.indexOf("</jylsh>");
      
        jyls = s.substring(startIndex, endIndex);
        System.out.println(jyls);
        
        startIndex = s.indexOf("<jyyzm>")+"<jyyzm>".length();
        endIndex = s.indexOf("</jyyzm>");
      
        jyyzm = s.substring(startIndex, endIndex);
        System.out.println(jyyzm);
        
        /*��ȡ����ˮ�ź���֤����ٴε����籣����MakeCardResult�ӿ�����д���*/
        wsUrl = new URL(configProperties.getProperty("yinhai_url"));
        conn = (HttpURLConnection) wsUrl.openConnection();
        conn.setDoInput(true);
        conn.setDoOutput(true);
        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Type", "text/xml;charset=UTF-8");
        conn.setRequestProperty("SOAPAction", "");
	        
		os = conn.getOutputStream();
        
		/*��ȡtaskID*/
		sqlConn=CardPrintImpl.getConn();
		stmt = sqlConn.createStatement();
		sql = "select taskID from task_info where id = "+taskSessionID;
		System.out.println(sql);
        //rs = stmt.executeQuery(sql);
        
        try 
        {
        	rs = stmt.executeQuery(sql);
        }
        catch (SQLException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			CardPrintImpl.closeConn(sqlConn);
			System.out.println("mysql operate error");
			return -1;
		}
        
        if (rs.next()) 
        {                
            taskID = rs.getString(1);
            System.out.println("taskID");
            System.out.println(taskID);
        }
        else
        {
            System.out.println("cannot find taskID,taskSession:"+taskSessionID);
        	return -1;
        }
        
        CardPrintImpl.closeConn(sqlConn);
			
			
			
		if (isOK == 1)
		{
			inputstr ="<input>" +
			"<isok>1</isok>" +
			"<sbyy></sbyy>" +
			"<taskid>"+taskID+"</taskid>" +
			"<ksbm>"+KSBM+"</ksbm>" +
			"<atr>"+ATR+"</atr>" +
			"<yhkh>"+yhkh+"</yhkh>" +
			"</input>" ;	
		}
		else
		{	
			startIndex = 0;
			endIndex = SBYY.indexOf("|");	        
			if (endIndex > 0)
			{
				SBYY = SBYY.substring(startIndex, endIndex); 
			}
	        
			inputstr ="<input>" +
			"<isok>0</isok>" +
			"<sbyy>"+SBYY+"</sbyy>" +
			"<taskid></taskid>" +
			"<ksbm></ksbm>" +
			"<atr></atr>" +
			"<yhkh></yhkh>" +
			"</input>" ;
		}
		
		inputstr = inputstr.replace("<","&lt;").replace(">","&gt;");
		System.out.println("inputstr is "+inputstr);
			
		soap="<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
			+"<soap:Envelope xmlns:soap=\"http://schemas.xmlsoap.org/soap/envelope/\">"
			+"<soap:Header>"
				+"<in:system xmlns:in=\"http://yinhai.com\">" +
				"<userName>"+username+"</userName>" +
				"<passWord>"+password+"</passWord>" 
				+"<jylsh>"+jyls+"</jylsh>" +
				"<jyyzm>"+jyyzm+"</jyyzm>" +
				"<jybh>MakeCardResult</jybh>" +
				"</in:system>"
			+"</soap:Header>" +
			"<soap:Body>" +
				"<ns2:callBusiness xmlns:ns2=\"http://webservice.common.yinhai.com/\">"+"<inputxml>" +
				inputstr+
				"</inputxml>" +
				
				"</ns2:callBusiness>" +
			"</soap:Body>" +
			"</soap:Envelope>;";
  
		
		
		//soap.getBytes("utf-8");
		System.out.println("second soap is"+soap);

        os.write(soap.getBytes("utf-8"));
        is = conn.getInputStream();
        b = new byte[1024];
        len = 0;
        s = "";
        while((len = is.read(b)) != -1){
            String ss = new String(b,0,len,"UTF-8");
            s += ss;
        }
        //System.out.println(s);
        //System.out.println(s.replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", ""));
        s=s.replace("&lt;", "<").replace("&gt;", ">").replace("&quot;", "");
        System.out.println(s);
        
        if (s == "")
        {
        	System.out.println("returnPrintResult:get yinhai makecardresult webservice error");
        	return -1;
        }
        is.close();
        os.close();
        conn.disconnect();
		
     
		return 0;
	
	}
	
	

	/**
	 * @param args
	 * @throws Exception 
	 * @throws NumberFormatException 
	 */
	public static void main(String[] args) throws NumberFormatException, Exception {
		
		int status = 0;
		Scanner input;
		String s;
		String SBYY;  /*ʧ��ԭ��*/
		String KSBM;		/*��ʶ����*/
		String ATR;	/*ATR*/
		String yhkh;
		String taskID;
		
		returnCall result = new returnCall();
		
		
		while(true)
		{
			input = new Scanner(System.in);
			
			System.out.println("��������̽������ʧ��Ϊ0���򿨳ɹ�Ϊ1,����ֵ�˳�");
			s=input.next();
			
			if(s.indexOf("0")!= -1)		
			{
				System.out.println("������Ļ��̽��Ϊ��ʧ�ܣ����������");	
				System.out.println("�����������ID��");	
				taskID =input.next();
				System.out.println("�����ID:"+taskID);
				
				System.out.println("��ʼ��������webservice���л���...");
				
				status = result.returnResult(0, "���¶�ʧ�ܵĽ�������ֶ�����|", "", "", "",Integer.parseInt(taskID));
				if (status == 0)
				{
					System.out.println("���̳ɹ�");
				}
				else
				{
					System.out.println("����ʧ��");
					
				}
				continue;
				
			}
			else if(s.indexOf("1")!= -1)	
			{
				input = new Scanner(System.in);
				System.out.println("������Ļ��̽��Ϊ�򿨳ɹ������������");	
				
				System.out.println("�����������ID��");	
				taskID =input.next();
				System.out.println("�����ID:"+taskID);
				
				System.out.println("�����뿨ʶ���룺");	
				KSBM =input.next();
				System.out.println("��ʶ����:"+KSBM);
				
				input = new Scanner(System.in);
				System.out.println("������ATR��");	
				ATR =input.next();
				System.out.println("ATR:"+ATR);
				
				input = new Scanner(System.in);
				System.out.println("���������п��ţ�");	
				yhkh =input.next();
				System.out.println("���п���:"+yhkh);
				
				System.out.println("��ʼ��������webservice���л���...");
				
				status = result.returnResult(1,"",KSBM, ATR, yhkh,Integer.parseInt(taskID));
				
				if (status == 0)
				{
					System.out.println("���̳ɹ�");
				}
				else
				{
					System.out.println("����ʧ��");					
				}
				continue;
				
			}
			else
			{
				System.out.println("�˳�...");
				return;
			}

		}

		
		
		
          
	}

}
