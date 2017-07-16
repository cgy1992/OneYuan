package com.service;

import java.io.*;
import java.net.*;
import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import java.net.Socket;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.sql.Timestamp;
import java.util.Calendar;
import java.util.Date;
import java.util.Properties;
import java.util.Random;

import javax.jws.WebParam;
import javax.servlet.http.HttpServletRequest;
import javax.sql.DataSource;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.commons.dbcp2.BasicDataSourceFactory;
import org.codehaus.xfire.transport.http.XFireServletController;

import org.codehaus.xfire.client.XFireProxyFactory;
import org.codehaus.xfire.service.Service;
import org.codehaus.xfire.service.binding.ObjectServiceFactory;


import org.apache.log4j.FileAppender;
import org.apache.log4j.Logger;
import org.apache.log4j.Level;
import org.apache.log4j.SimpleLayout;

class  Msg_Info
{
/* ���ն˿ͻ��˷�����ӡ��������Ϣ�����ȴ�����Ϣͷ����Ϣ���������ȵ���Ϣ
 * �������ն˿ͻ��˷���������Ϣ��Ӧ������id����Ϣ����
 * 
typedef struct _msg_info
{		
	int type;
	int len;
}msg_info;
*/
	public static int length = 8;
	private byte[] buf = new byte[4+4]; 
	
	
	public int type = 0;
	public int len = 0;
	
	/*����ת��Ϊ�ֽ�����*/
	private static byte[] toLH(int n) {
	    byte[] b = new byte[4];
	    b[0] = (byte) (n & 0xff);
	    b[1] = (byte) (n >> 8 & 0xff);
	    b[2] = (byte) (n >> 16 & 0xff);
	    b[3] = (byte) (n >> 24 & 0xff);
	    return b;
	}
	/*���첢ת��*/
	public Msg_Info(int type,int len) {
		
		this.type = type;
		this.len = len;
	 
		byte[] temp;
	
		temp = toLH(type);
		System.arraycopy(temp, 0, buf, 0, temp.length);
		
		temp = toLH(len);
		System.arraycopy(temp, 0, buf, 4, temp.length);
	}

	/* ����Ҫ���͵�����*/
	public byte[] getBuf() {
		return buf;
	}
};

public class CardPrintImpl implements CardPrint {
	
	public static  int WorkStationPort = 6833;//��ӡ�ͻ��˵ļ����˿� 
	public static String WorkStationIP; //��ӡ�ͻ��˵ļ���IP
	public static Socket client_socket = null; 
		
	public static final int RETURN_OK = 0;
	public static final int RETURN_FAIL = -1;
	
	public static long  swiftNumber = 1000000000;   /*����Ψһ��ʶ*/
	public static int  Locale = 0x10100001;   /*��ˮ��*/
	
	public static final Logger logger = Logger.getLogger(CardPrintImpl.class);
	
	static {   	
        SimpleLayout layout = new SimpleLayout();
	    /*FileAppender appender = null;
	    try {
	         appender = new FileAppender(layout, "myLog.log", false);
		    } catch (Exception e) {
		    }
		    logger.addAppender(appender);*/
		    logger.setLevel(Level.ERROR);
		    
	    }
	
	
	/*���ݿ����ӳ�����*/
	private static final Log log = LogFactory.getLog(CardPrintImpl.class);
	private static final String configFile = "dbcp.properties";
	private static DataSource dataSource;
	static {
	  Properties dbProperties = new Properties();
	  try {
	   dbProperties.load(CardPrintImpl.class.getClassLoader()
	     .getResourceAsStream(configFile));
	   dataSource = BasicDataSourceFactory.createDataSource(dbProperties);
	   Connection conn = getConn();
	   DatabaseMetaData mdm = conn.getMetaData();
	   log.info("Connected to " + mdm.getDatabaseProductName() + " "
	     + mdm.getDatabaseProductVersion());
	   if (conn != null) {
	    conn.close();
	   }
	  } catch (Exception e) {
	   log.error("��ʼ�����ӳ�ʧ�ܣ�" + e);
	   
	  }
	}
	
	
    
	 
	 public static final Connection getConn() {
		  Connection conn = null;
		  try {
		   conn = dataSource.getConnection();
		  } catch (SQLException e) {
		   log.error("��ȡ���ݿ�����ʧ�ܣ�" + e);
		  }
		  return conn;
		 }
		 
	 public static void closeConn(Connection conn) {
	  try 
	  {
		   if (conn != null && !conn.isClosed()) {
		    conn.setAutoCommit(true);
		    conn.close();
		   }
	  } 
	  catch (SQLException e) {
	   log.error("�ر����ݿ�����ʧ�ܣ�" + e);
	  }
	 }
	
	public String getversion() {
		return "CardPrint v1.0";
	}
	
	/*�ն˴�ӡ�豸ע��*/
	public int cardDeviceRegister(
			String workStationIP,    /*�ն�IP*/
			String workStationPort,   /*�ն�Port*/
			String cardDeviceName,	/*��ӡ������*/
			String encryptIndex) throws SQLException	/*��������*/
			
	{
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		
		Date today = new java.util.Date();  
        Timestamp time; 
		
		System.out.println(workStationIP);
		System.out.println(workStationPort);
		
		System.out.println(cardDeviceName);
		System.out.println(encryptIndex);
		
		if (encryptIndex == null || encryptIndex == "")
		{
			return RETURN_FAIL;			
		}
	
		
		
        
        conn=CardPrintImpl.getConn();
        sql = "select id from deviceinfo where encryptIndex = '" +encryptIndex+ "'";
        System.out.println(sql);
        stmt = conn.createStatement();
        rs = stmt.executeQuery(sql);
        if (rs.next())
        {
        	System.out.println("has register");
        	sql = "delete from deviceinfo where encryptIndex = ?";
            System.out.println(sql);
            prest = conn.prepareStatement(sql);
            prest.setString(1, encryptIndex);
            rst = prest.execute();
        }
        
			
		/*��ȡ���ʸ�webservice�Ŀͻ���*/			
		try {
			HttpServletRequest request = XFireServletController.getRequest();
		    ip = request.getRemoteAddr();			
		} catch (Exception e) {
				e.printStackTrace();
				ip = "";
		}
			
		try 
		{
			time = new Timestamp(today.getTime());
            insertTableSQL = "INSERT INTO deviceInfo"
            	+ "(actionIP,workStationIP,workStationPort,cardDeviceName,encryptIndex,regTime) VALUES"
            	+ "(?,?,?,?,?,?)";
            
            prest = conn.prepareStatement(insertTableSQL);
            prest.setString(1, ip);
            prest.setString(2, workStationIP);
            prest.setString(3,workStationPort);	            
            prest.setString(4, cardDeviceName);
            prest.setString(5,encryptIndex);
            prest.setTimestamp(6,time);
                      
            rst = prest.execute();
           
            CardPrintImpl.closeConn(conn);
		}        
        catch (SQLException e) {
			e.printStackTrace();
			CardPrintImpl.closeConn(conn);
			logger.error("card print register failed"+workStationIP+workStationPort);
			return RETURN_FAIL;
		}			
            
        CardPrintImpl.closeConn(conn);
		return RETURN_OK;
		

	}
	
	/*�ն˴�ӡ�����ش�ӡ���*/
	public int returnPrintResult(
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
			    
			    /*��ӡ������Ϊʧ��ʱ��¼ʧ����Ϣ*/
			    time = new Timestamp(today.getTime());
                insertTableSQL = "INSERT INTO ws_error_log"
                	+ "(taskInfoID,wsType,isOK,SBYY,errorTime) VALUES"
                	+ "(?,?,?,?,?)";
                
                prest = sqlConn.prepareStatement(insertTableSQL);
                prest.setInt(1, taskSessionID);
                prest.setString(2, "returnPrintResult");
                prest.setInt(3, isOK);
                prest.setString(4, SBYY);
                prest.setTimestamp(5,time);
                          
                rst = prest.execute();
				
			}
			else if (isOK == 2) /*��ӡ��æ*/
			{
				sql = "update task_info set status = 100 where id = "+taskSessionID;   		
			    stmt.executeUpdate(sql);	
			    CardPrintImpl.closeConn(sqlConn);
			    logger.error("printer busy taskSessionID:"+taskSessionID);
				return 0;
			}
			else if (isOK == 3) /*�յ��ͻ�*/
			{
				sql = "update task_info set status = 1 where  id = "+taskSessionID;   		
			    stmt.executeUpdate(sql);	
			    CardPrintImpl.closeConn(sqlConn);
			    
				return 0;
			}
		        
		} catch (SQLException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			CardPrintImpl.closeConn(sqlConn);
			logger.error("returnPrintResult:database error");
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
			   logger.error("returnPrintResult:read config file,ioexception");
			   return -1;
			  }   
			  
		} catch (FileNotFoundException e2) {
			// TODO Auto-generated catch block
			e2.printStackTrace();
			logger.error("returnPrintResult:read config file,ioexception");
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
			logger.error("returnPrintResult:can not connect to yinhai webservice");
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
			logger.error("returnPrintResult:webservice outputstream error");
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
        	logger.error("returnPrintResult:get yinhai yhlsh webservice error");
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
			logger.error("mysql operate error");
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
        	/*�Ҳ���taskIDʱ��¼ʧ����Ϣ*/
		    time = new Timestamp(today.getTime());
            insertTableSQL = "INSERT INTO ws_error_log"
            	+ "(taskInfoID,wsType,isOK,SBYY,errorTime) VALUES"
            	+ "(?,?,?,?,?)";
            
            prest = sqlConn.prepareStatement(insertTableSQL);
            prest.setInt(1, taskSessionID);
            prest.setString(2, "returnPrintResult");
            prest.setInt(3, isOK);
            prest.setString(4, SBYY);
            prest.setTimestamp(5,time);
                      
            rst = prest.execute();
            
            CardPrintImpl.closeConn(sqlConn);
            
            logger.error("returnPrintResult:cannot find taskID,taskSession:"+taskSessionID);
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
        	logger.error("returnPrintResult:get yinhai makecardresult webservice error");
        	return -1;
        }
        is.close();
        os.close();
        conn.disconnect();
		
     
		return 0;
	
	}
	
	public static int getLengthString(String str){
		  try{
		   byte[] b = str.getBytes("GBK");
		   return b.length;
		  }catch(Exception ex){
			  logger.error("get LengthString failed");
			  
		   return 0;
		  }
		 }
	
	public String AddTask(			
			String strClientName,   /*�ͻ��˱�ʶ*/
			String strQueueName,		/*��������*/
			String strTaskName,		/*��������*/
			String strTaskData,	/*��������*/
			String intPriority) throws Exception	/*���ȼ�*/
	{
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		int status = 0;
		Date today = new java.util.Date();  
        Timestamp time; 
		StringBuffer sb = new StringBuffer();
		String taskID;
		int taskinfoID = 0;

		conn=CardPrintImpl.getConn();
		stmt = conn.createStatement();
		
		sql = "select global_value from globalValue limit 1";
        rs = stmt.executeQuery(sql);
        if (rs.next()) 
        { 
        	swiftNumber = rs.getInt(1);
        	System.out.println(swiftNumber);
        	swiftNumber = swiftNumber + 1;
        	stmt = conn.createStatement();
			sql = "update globalValue set global_value = "+swiftNumber+ " where global_value = "+rs.getInt(1);
			System.out.println(sql);
            stmt.executeUpdate(sql);
        }
        else
        {
        	swiftNumber = swiftNumber + 1;
        }
        
        String locale = "";
        String lsh = "";
        int lsh_hex = 0x100001;
        
        /*�Զ�������ˮ��*/
        sql = "select locale,lsh from identity_number limit 1";
        rs = stmt.executeQuery(sql);
        if (rs.next()) 
        { 
        	locale = rs.getString(1);  //������
        	lsh = rs.getString(2);   //6λ2����ȫ�ֺ�
        	System.out.println(lsh);  
        	lsh_hex = Integer.valueOf(lsh,16);  //תΪ���ͽ��м���
        	lsh_hex = lsh_hex + 1;
            System.out.format( "%x ",lsh_hex );
        	lsh = Integer.toHexString(lsh_hex); //������Ϻ�תΪ�ַ���
        	System.out.println("lsh-str:"+lsh);
        	locale = locale + lsh;  //�����ź�6λ��������
        	System.out.println(locale);
        	stmt = conn.createStatement();
			sql = "update identity_number set lsh = '" +lsh+ "' where locale = '" +rs.getString(1)+ "' ";
			System.out.println(sql);
            stmt.executeUpdate(sql);
        }
        else
        {
        	Locale = Locale + 1;
        	lsh = Integer.toHexString(Locale);
        	insertTableSQL = "INSERT INTO identity_number"
            	+ "(locale,lsh) VALUES"
            	+ "(?,?)";            
           
            prest = conn.prepareStatement(insertTableSQL);           
            prest.setString(1, "10");         
            prest.setString(2,lsh);
           
            rst = prest.execute();
        	
        }
        
		sql = "select id from task_info where (status = 0 or status = 1 or status = 100) and task_name = '" +strTaskName+ "' ";
        rs = stmt.executeQuery(sql);
 
        if (rs.next()) 
        { 
        	taskinfoID = rs.getInt(1);
        	
    		taskID = "GY"+strQueueName+swiftNumber;
    		
        	sb.append("<result>\r"); 
			sb.append("<code>0</code>\r"); 
		    sb.append("<strTaskID>");
		    sb.append(taskID);
		    sb.append("</strTaskID>\r"); 
		    sb.append("<message>��ӡ�����Ѿ��·��������ظ��·�</message>\r");   //���ݳɹ�ʱ����Ϊ��
		    sb.append("</result>\r"); 
		    
		    time = new Timestamp(today.getTime());
            insertTableSQL = "INSERT INTO ws_error_log"
            	+ "(taskInfoID,wsType,isOK,SBYY,errorTime) VALUES"
            	+ "(?,?,?,?,?)";
            
            /*�ظ��·���������浽���ݿ����־��*/
            prest = conn.prepareStatement(insertTableSQL);
            prest.setInt(1,taskinfoID );
            prest.setString(2, "addTask");
            prest.setInt(3, 0);
            prest.setString(4, "��ӡ�����Ѿ��·��������ظ��·�");
            prest.setTimestamp(5,time);
                      
            rst = prest.execute();
            
            logger.error("��ӡ�����ظ��·�������ID:"+taskinfoID);
		    
		    CardPrintImpl.closeConn(conn);
		    
		    System.out.println(sb.toString());
		    return sb.toString();
            
        }
		
		
		taskID = "GY"+strQueueName+swiftNumber;
		System.out.println("taskID:"+taskID);
		time = new Timestamp(today.getTime());
   
        insertTableSQL = "INSERT INTO task_info"
        	+ "(client_name,queue_name,task_name,task_data,intPriority,taskID,status,addTime,lsh) VALUES"
        	+ "(?,?,?,?,?,?,?,?,?)";
        
        try {
				prest = conn.prepareStatement(insertTableSQL);
				prest.setString(1, strClientName);
		        prest.setString(2,strQueueName);
		        prest.setString(3, strTaskName);
		        
		        prest.setString(4,strTaskData);
		        prest.setString(5,intPriority);
		        prest.setString(6,taskID);
		        prest.setInt(7, status);
		        prest.setTimestamp(8, time);
		        prest.setString(9, locale);
		        rst = prest.execute();
	        
		} catch (SQLException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			StringWriter sw = new StringWriter(); 

            e.printStackTrace(new PrintWriter(sw, true)); 

            String str = sw.toString(); 

            logger.error("���񱣴浽���ݿ�ʧ��,ʧ����ϢΪ��"+str+" ������ϢΪ��strClientName "+strClientName+
            			" strQueueName "+strQueueName+" strTaskName "+strTaskName+
            			" strTaskData "+strTaskData);
            			
			
			time = new Timestamp(today.getTime());
            insertTableSQL = "INSERT INTO ws_error_log"
            	+ "(taskInfoID,wsType,isOK,SBYY,errorTime) VALUES"
            	+ "(?,?,?,?,?)";
            
            /*�����������ݿ�ʧ�ܣ�д��־*/
            prest = conn.prepareStatement(insertTableSQL);
            prest.setInt(1, taskinfoID);
            prest.setString(2, "addTask");
            prest.setInt(3, 0);
            prest.setString(4, "���񱣴浽���ݿ�ʧ��:"+strTaskName);
            prest.setTimestamp(5,time);
                      
            rst = prest.execute();
            
			sb.append("<result>\r"); 
			sb.append("<code>0</code>\r"); 
		    sb.append("<strTaskID>");
		    sb.append(taskID);
		    sb.append("</strTaskID>\r"); 
		    sb.append("<message>���񱣴浽���ݿ�ʧ��</message>\r");   //���ݳɹ�ʱ����Ϊ��
		    sb.append("</result>\r"); 
		    
		    System.out.println(sb.toString());
		    CardPrintImpl.closeConn(conn);
		    return sb.toString();
		}
		
		CardPrintImpl.closeConn(conn);
        
		
		sb.append("<result>\r"); 
		sb.append("<code>1</code>\r"); 
	    sb.append("<strTaskID>");
	    sb.append(taskID);
	    sb.append("</strTaskID>\r"); 
	    sb.append("<message></message>\r");   //���ݳɹ�ʱ����Ϊ��
	    sb.append("</result>\r"); 
	    System.out.println(sb.toString());
	    return sb.toString();
	}
	
	public String getTask(String encryptIndex) throws Exception
	{
		int len =0;
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		String taskData = "init";
		int taskinfoID = 0;
		String lsh ="";
		int status;
		int num = 0;
		int msgLen = 0;
		Date today = new java.util.Date();  
        Timestamp time;
        String s = "no task";
		StringBuffer sb = new StringBuffer();
		
		CardPrintImpl register = new CardPrintImpl();
		
		/*��ѯ���ݿ����Ƿ���ڴ���ӡ������
		 * ����status=0��ʾ��ʼ״̬��status=1��ʾ�Ѿ��������·����ͻ��ˣ�status=2��ʾ�ͻ��˷��سɹ��Ĵ�ӡ���
		 * status=-1��ʾ�ͻ��˷���ʧ�ܵĴ�ӡ���
		 */
		conn=CardPrintImpl.getConn();
		stmt = conn.createStatement();
		sql = "select task_data,id,lsh from task_info where (status = 0 or status = 1) and queue_name = '" +encryptIndex+ "' order by intPriority limit 1";
        rs = stmt.executeQuery(sql);
        
        if (rs.next()) 
        { 
            taskData = rs.getString(1);
            taskinfoID = rs.getInt(2); 
            lsh = rs.getString(3);
            
        }
        else
        {
        	CardPrintImpl.closeConn(conn);
        	return s;
        	//return sb.toString();
        }
        
        stmt = conn.createStatement();
		sql = "update task_info set status = 1 where status = 0 and id = "+taskinfoID;
		
        stmt.executeUpdate(sql);
        CardPrintImpl.closeConn(conn);
        
        int taskLen = register.getLengthString(taskData);
        
        sb.append("<?xml version='1.0' encoding='gbk'?>\r");
		sb.append("<taskinfo>\r"); 
	    sb.append("<strTaskID>");
	    sb.append(taskinfoID);
	    sb.append("</strTaskID>\r"); 
	    sb.append("<strlsh>");
	    sb.append(lsh);
	    sb.append("</strlsh>\r"); 
	    sb.append("<strTaskData>");
	    sb.append(taskData);
	    sb.append("</strTaskData>\r");
	    sb.append("<strTaskLen>");
	    sb.append(String.valueOf(taskLen));
	    sb.append("</strTaskLen>\r");
	    
	    sb.append("</taskinfo>\r"); 
	    
	    
	    String taskString = sb.toString();
	    
	    byte[] bytes=taskString.getBytes("gbk");
	    s = new String(bytes,"gbk");
	    
	    /*byte[] buf = new byte[taskLen]; 
		
		byte[] temp = taskString.getBytes();
	    System.arraycopy(temp, 0, buf, 0, taskLen);
	    
	    taskString = new String(taskString.getBytes("utf-8"));  
	    
	    String s1 = new String(buf, "GBK");*/
	    
	    return s;

	}
	
	public static Socket Client_socket_init()
	{
		Socket socket_client = null;
		InetAddress addr = null;
		/*����socket�Ļ�������*/
		
		try {
			/*����socket ��ַ �˿���Ϣ*/
			addr = InetAddress.getByName(WorkStationIP);
			socket_client = new Socket(addr, WorkStationPort);
			System.out.println("Server:socket = " + socket_client);
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
		
		
		return socket_client;
	}
	
	/* send message !*/
	public static void send_messageLen(Socket socket,int type,int len)
	{
		/*�жϲ�������*/
		if(socket==null || len <0)
		{
			System.out.println("Server : input  para  error !");
			return ;
		}
		//byte[] receive = new byte[1056];
		int available=0;
		//Msger_Info mmInfo=null;
		Msg_Info sendmsg = new Msg_Info(type,len);
		try {
			if((available=socket.getInputStream().available())>=0)
			{
				//socket.getInputStream().read(new byte[available]);
				socket.getInputStream().skip(available);
			}
			socket.getOutputStream().write(sendmsg.getBuf());
			socket.getOutputStream().flush();
			//socket.getInputStream().read(receive);
			//mmInfo =Msger_Info .getmsger_info(receive);
			//System.out.println("received:name is :"+mmInfo.name+",msg is :"+mmInfo.msg+",len is :"+mmInfo.len);
			
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			logger.error("socket msg header send failed");
		}
		return ;
		//return mmInfo.msg;
		
	}
	
	
	
	
	public static void main(String[] args) throws SQLException {
		
		int len =0;
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        PreparedStatement prest = null;
        String ip=null;
		boolean rst = false;
		String insertTableSQL;
		String taskData = "init";
		int taskinfoID = 0;
		int status;
		int num = 0;
		int msgLen = 0;
		Date today = new java.util.Date();  
        Timestamp time;
        String encryptIndex;
        int lsh = 0x00000001;
        int count = 0;
        String str = "100abcf";
		
		
        System.out.println("�籣���ݴ����У��벻Ҫ�ر�");
        
        lsh = Integer.valueOf(str,16);
        System.out.format( "%x ",lsh );
		
		// TODO Auto-generated method stub
		CardPrintImpl register = new CardPrintImpl();
		
		
		//ÿ����������ȡ��һ��δ��������ȼ���ߵļ�¼
		while (true)
		{
			try 
			{	
				Thread.currentThread().sleep(50000);            
	        } 
			catch (InterruptedException e) {
	            e.printStackTrace(); 
	        }
	
        	Calendar calendar = Calendar.getInstance();
            //�ѵ�ǰʱ���ȥ3���£����Ϊ�������������ǰ�ƣ�����2010��1��-3����2009��10��
            calendar.add(Calendar.MINUTE,-5);
        	Timestamp curTime = new Timestamp(calendar.getTimeInMillis());
        	System.out.println(curTime);
        	Timestamp addTime;
        	int tmpTaskSessionID = 0;
        	
        	conn=CardPrintImpl.getConn();
    		stmt = conn.createStatement();
        	sql = "select id,addTime from task_info where status = 1";
            rs = stmt.executeQuery(sql);
            while (rs.next())
            {
            	tmpTaskSessionID = rs.getInt(1);
            	addTime = rs.getTimestamp(2);
            	System.out.println(addTime);
            	if (curTime.after(addTime))
            	{
            		System.out.println("������¼�Ѿ���ʱ");
            		logger.error("this record has been timeout,taskSessionID:"+tmpTaskSessionID);
            		 //���û�д�ӿ�
            		String serviceUrl = "http://localhost:8080/CardPrintService/services/CardPrint";
            		Service serviceModel = new ObjectServiceFactory().create(CardPrint.class, null, "http://localhost:8080/CardPrintService/services/CardPrint?wsdl", null);
            		XFireProxyFactory serviceFactory = new XFireProxyFactory();
            		try
            		{
            			CardPrint service = (CardPrint)serviceFactory.create(serviceModel,serviceUrl);
            			status = service.returnPrintResult(0, "�ô�ӡ��¼�ڳ�ʱʱ����δ��ӡ|", "", "", "", tmpTaskSessionID);
            		    System.out.println(status);
            		}
            		catch(Exception e)
            		{
            			logger.error("print timeout,get yinhai webservice error");
            		    e.printStackTrace();
            		}
            		stmt = conn.createStatement();
            		sql = "update task_info set status = -1 where id = "+tmpTaskSessionID;
            		
                    stmt.executeUpdate(sql);
                    
            		
            	}
            	
            }
        	CardPrintImpl.closeConn(conn);
        	continue;
           
		}
		
	
	}
	
}