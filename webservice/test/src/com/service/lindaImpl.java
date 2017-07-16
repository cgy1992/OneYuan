package com.service;




import java.io.*;
import java.net.*;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.sql.Connection;
import java.sql.DatabaseMetaData;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.Properties;

import javax.servlet.http.HttpServletRequest;
import javax.sql.DataSource;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;
import org.apache.commons.dbcp2.BasicDataSourceFactory;
import org.codehaus.xfire.transport.http.XFireServletController;



public class lindaImpl implements Ilinda {
	
	public static final String IP_ADDR = "localhost";//��������ַ 
	public static final int PORT = 6832;//�������˿ں�  
	
	public static final int REGISTER_OK = 0;
	public static final int REGISTER_FAIL = -1;
	
	public static int ID = 0;
	
	private static final Log log = LogFactory.getLog(lindaImpl.class);
	 private static final String configFile = "dbcp.properties";
	 private static DataSource dataSource;
	 static {
	  Properties dbProperties = new Properties();
	  try {
	   dbProperties.load(lindaImpl.class.getClassLoader()
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
		 /**
		  * �ر�����
		  * 
		  * @param conn
		  *            ��Ҫ�رյ�����
		  */
		 public static void closeConn(Connection conn) {
		  try {
		   if (conn != null && !conn.isClosed()) {
		    conn.setAutoCommit(true);
		    conn.close();
		   }
		  } catch (SQLException e) {
		   log.error("�ر����ݿ�����ʧ�ܣ�" + e);
		  }
		 }
	
	public String example(String message) {
		return message+"hipengwujie,this is your first";
	}
	
	public String getversion1() {
		return "version is";
	}
	
	public int cardDeviceRegister(
			String workStationIP,    /*�ն�IP*/
			int workStationPort,   /*�ն�Port*/
			String encryptIP,		/*���ܻ�IP*/
			int encryptPort,		/*���ܻ�Port*/
			String cardDeviceName,	/*��ӡ������*/
			String encryptIndex,	/*��������*/
			boolean isDecrypt)		/*�Ƿ���ܻ��ƿ�*/
	{
		Connection conn = null;
        Statement stmt = null;
        ResultSet rs = null;
        String sql = null;
        String ip=null;
		
		if (true)
		{
			System.out.println(workStationIP);
			System.out.println(workStationPort);
			System.out.println(encryptIP);
			System.out.println(encryptPort);
			System.out.println(cardDeviceName);
			System.out.println(encryptIndex);
			System.out.println(isDecrypt);
			
			
			try {
				HttpServletRequest request = XFireServletController.getRequest();
			    ip = request.getRemoteAddr();
				System.out.println("���ʵĿͻ��� IP ��:"+ ip);
				
	            
	            System.out.println("Results:");
			} catch (Exception e) {

					System.out.println("�޷���ȡHttpServletRequest");
					e.printStackTrace();

			}
			
			try 
			{
				conn=lindaImpl.getConn();
				stmt = conn.createStatement();
				sql = "select * from deviceInfo";
	            rs = stmt.executeQuery(sql);
	            System.out.println("Results:");
	            int numcols = rs.getMetaData().getColumnCount();
	            System.out.println("numcols " + numcols);
	            while (rs.next()) 
	            {
	                
	                System.out.println(" " + rs.getInt(1));
	                System.out.println(" " + rs.getString(2));
	                System.out.println(" " + rs.getInt(3));
	            }
	            ID = ID +6;
	            conn=lindaImpl.getConn();
	            sql = "insert into deviceInfo values("+ID+","+workStationIP+",workStationPort,"+encryptIP+",encryptPort,"+cardDeviceName+","+encryptIndex+",0)";
	            stmt.execute(sql);
	            System.out.println("Results:");
	            
	            ip="202.115.0.1";
	            sql = "insert into actionLog values("+ip+")";
	            stmt.execute(sql);
			} 
            
            catch (SQLException e) {
            	System.out.println("connect wrong!");
				e.printStackTrace();
			}
			
            lindaImpl.closeConn(conn);
			return REGISTER_OK;
		}
		else
		{
			return REGISTER_FAIL;
		}

	}
	
	public class msgHdr
	{
		public  int type;
		public  int len;
	}
	
	public static void main(String[] args) {
		
		int len =0;
		// TODO Auto-generated method stub
		lindaImpl register = new lindaImpl();
		
		register.cardDeviceRegister("1",2,"3",4,"5","6",true);
		
		System.out.println("�ͻ�������...");  
        System.out.println("�����յ����������ַ�Ϊ \"OK\" ��ʱ��, �ͻ��˽���ֹ\n"); 
        while (true) {  
        	Socket socket = null;
        	try {
        		//����һ�����׽��ֲ��������ӵ�ָ�������ϵ�ָ���˿ں�
	        	socket = new Socket(IP_ADDR, PORT);  
	              
	            //��ȡ������������  
	            DataInputStream input = new DataInputStream(socket.getInputStream());  
	            //��������˷�������  
	            ObjectOutputStream  out = new ObjectOutputStream(socket.getOutputStream());  
	           
	            /*lindaImpl.msgHdr inner = register.new msgHdr();
	            inner.type = 1;
	            inner.len = 512;
	            out.writeObject(inner);*/
	            
	            Integer len1 = 512;
	            
	            DataOutputStream out1 = new DataOutputStream(socket.getOutputStream());  
	            //out1.write(len1);
	            /*
	            for (int i =0 ;i< 4;i++)
	            {
	            	String str = "abcde|bcdef|cde";
	            	len = len + str.length();
	            	
		            out.writeUTF(str); 
		            System.out.println("�ѷ��ͣ�+len+");
	            }*/
	            
	              
	            String ret = input.readUTF();   
	            System.out.println("�������˷��ع�������: " + ret);  
	            // ����յ� "OK" ��Ͽ�����  
	            if ("OK".equals(ret)) {  
	                System.out.println("�ͻ��˽��ر�����");  
	                Thread.sleep(500);  
	                break;  
	            }  
	            
	            out.close();
	            input.close();
        	} catch (Exception e) {
        		System.out.println("�ͻ����쳣:" + e.getMessage()); 
        	} finally {
        		if (socket != null) {
        			try {
						socket.close();
					} catch (IOException e) {
						socket = null; 
						System.out.println("�ͻ��� finally �쳣:" + e.getMessage()); 
					}
        		}
        	}
        }
	}
	
}