package com.service;

public class verifytest {

}


һ������web services ����(XFire)����ƽʱ��һ����
�������������֤����
1�����ȱ�д�������֤�࣬�̳�AbstractHandler��
package test;
import org.codehaus.xfire.MessageContext;
import org.codehaus.xfire.handler.AbstractHandler;
import org.jdom.Element;

public class AuthenticationHandler extends AbstractHandler {

public void invoke(MessageContext cfx) throws Exception {
   if (cfx.getInMessage().getHeader() == null) {
    throw new org.codehaus.xfire.fault.XFireFault("������������֤��Ϣ",
      org.codehaus.xfire.fault.XFireFault.SENDER);
   }
   Element token = cfx.getInMessage().getHeader().getChild(
     "AuthenticationToken");
   if (token == null) {
    throw new org.codehaus.xfire.fault.XFireFault("���������������֤��Ϣ",
      org.codehaus.xfire.fault.XFireFault.SENDER);
   }

   String username = token.getChild("Username").getValue();
   String password = token.getChild("Password").getValue();
   try {
    // ���������֤ ��ֻ��abcd@1234���û�Ϊ��Ȩ�û�
    if (username.equals("abcd") && password.equals("1234"))
     // ����䲻��ʾ
     System.out.println("�����֤ͨ��");
    else
     throw new Exception();
   } catch (Exception e) {
    throw new org.codehaus.xfire.fault.XFireFault("�Ƿ����û���������",
      org.codehaus.xfire.fault.XFireFault.SENDER);
   }
}
}
2��Client������Ȩ��Ϣ
package test;
import org.codehaus.xfire.MessageContext;
import org.codehaus.xfire.handler.AbstractHandler;
import org.jdom.Element;

public class ClientAuthenticationHandler extends AbstractHandler {

    private String username = null;

    private String password = null;

    public ClientAuthenticationHandler() { 
    }

    public ClientAuthenticationHandler(String username,String password) { 
    this.username = username; 
        this.password = password;
    }

    public void setUsername(String username) { 
        this.username = username; 
    }

    public void setPassword(String password) { 
        this.password = password; 
    }

    public void invoke(MessageContext context) throws Exception {

        //ΪSOAP Header������֤��Ϣ
        Element el = new Element("header"); 
        context.getOutMessage().setHeader(el); 
        Element auth = new Element("AuthenticationToken"); 
        Element username_el = new Element("Username"); 
        username_el.addContent(username); 
        Element password_el = new Element("Password"); 
        password_el.addContent(password); 
        auth.addContent(username_el); 
        auth.addContent(password_el); 
        el.addContent(auth); 
    } 
}
3���޸�services.xmlΪweb services��Handler
<?xml version="1.0" encoding="UTF-8"?>
<beans xmlns="http://xfire.codehaus.org/config/1.0">
<service xmlns="http://xfire.codehaus.org/config/1.0">
   <name>Hello</name>
   <namespace>http://test/HelloService</namespace>
   <serviceClass>test.IHello</serviceClass>
   <implementationClass>test.HelloImpl</implementationClass>
   <inHandlers> 
   <handler handlerClass ="test.AuthenticationHandler" ></handler > 
   </inHandlers>
   <style>wrapped</style>
   <use>literal</use>
   <scope>application</scope>
</service>
</beans>
4���½�һ����ClientTest����������
package test;
import java.lang.reflect.Proxy;
import java.net.MalformedURLException;
import org.codehaus.xfire.client.*;
import org.codehaus.xfire.service.Service;
import org.codehaus.xfire.service.binding.ObjectServiceFactory;

public class ClientTest {

/**
* @param args
*/
public static void main(String[] args) {
   // TODO Auto-generated method stub
   try {
    Service serviceModel = new ObjectServiceFactory().create(IHello.class);
    IHello service = (IHello) new XFireProxyFactory().create(serviceModel,
        "http://dracom-d1514b82:8080/web_services3/services/Hello");   
    XFireProxy proxy = (XFireProxy)Proxy.getInvocationHandler(service);
    Client client = proxy.getClient();
    //������Ȩ��Ϣ
    client.addOutHandler(new ClientAuthenticationHandler("abcd","1234"));
    //�������web services�����ķ�����Ϣ
    System.out.println(service.getMessage("���aaa"));
   } catch (MalformedURLException e) {
    // TODO Auto-generated catch block
    e.printStackTrace();
   } catch (IllegalArgumentException e) {
    // TODO Auto-generated catch block
    e.printStackTrace();
   }
}
}
�����������Ǿ�����˱��룬��������web services�����пͻ��˴��룬����Ϊabcd@1234λ��Ȩ�û���
ʹ��abcd@1234,������������web services,����ô����ʺţ�����������쳣:
Exception in thread "main" org.codehaus.xfire.XFireRuntimeException: Could not invoke service.. Nested exception is org.codehaus.xfire.fault.XFireFault: �Ƿ����û���������
org.codehaus.xfire.fault.XFireFault: �Ƿ����û���������
at org.codehaus.xfire.fault.Soap11FaultSerializer.readMessage(Soap11FaultSerializer.java:31)
at org.codehaus.xfire.fault.SoapFaultSerializer.readMessage(SoapFaultSerializer.java:28)
at org.codehaus.xfire.soap.handler.ReadHeadersHandler.checkForFault(ReadHeadersHandler.java:111)
at org.codehaus.xfire.soap.handler.ReadHeadersHandler.invoke(ReadHeadersHandler.java:67)
at org.codehaus.xfire.handler.HandlerPipeline.invoke(HandlerPipeline.java:131)
at org.codehaus.xfire.client.Client.onReceive(Client.java:406)
at org.codehaus.xfire.transport.http.HttpChannel.sendViaClient(HttpChannel.java:139)
at org.codehaus.xfire.transport.http.HttpChannel.send(HttpChannel.java:48)
at org.codehaus.xfire.handler.OutMessageSender.invoke(OutMessageSender.java:26)
at org.codehaus.xfire.handler.HandlerPipeline.invoke(HandlerPipeline.java:131)
at org.codehaus.xfire.client.Invocation.invoke(Invocation.java:79)
at org.codehaus.xfire.client.Invocation.invoke(Invocation.java:114)
at org.codehaus.xfire.client.Client.invoke(Client.java:336)
at org.codehaus.xfire.client.XFireProxy.handleRequest(XFireProxy.java:77)
at org.codehaus.xfire.client.XFireProxy.invoke(XFireProxy.java:57)
at $Proxy0.getMessage(Unknown Source)
at test.ClientTest.main(ClientTest.java:24)

�������CientTest������Heade����������쳣:
    XFireProxy proxy = (XFireProxy)Proxy.getInvocationHandler(service);
    Client client = proxy.getClient();
    //������Ȩ��Ϣ
    client.addOutHandler(new ClientAuthenticationHandler("abcd1","1234"));

��Ϣ: Fault occurred!
org.codehaus.xfire.fault.XFireFault: ������������֤��Ϣ
at test.AuthenticationHandler.invoke(AuthenticationHandler.java:11)
at org.codehaus.xfire.handler.HandlerPipeline.invoke(HandlerPipeline.java:131)
at org.codehaus.xfire.transport.DefaultEndpoint.onReceive(DefaultEndpoint.java:64)
at org.codehaus.xfire.transport.AbstractChannel.receive(AbstractChannel.java:38)
at org.codehaus.xfire.transport.http.XFireServletController.invoke(XFireServletController.java:304)
at org.codehaus.xfire.transport.http.XFireServletController.doService(XFireServletController.java:129)
at org.codehaus.xfire.transport.http.XFireServlet.doPost(XFireServlet.java:116)
at javax.servlet.http.HttpServlet.service(HttpServlet.java:710)
at javax.servlet.http.HttpServlet.service(HttpServlet.java:803)
at org.apache.catalina.core.ApplicationFilterChain.internalDoFilter(ApplicationFilterChain.java:290)
at org.apache.catalina.core.ApplicationFilterChain.doFilter(ApplicationFilterChain.java:206)
at org.apache.catalina.core.StandardWrapperValve.invoke(StandardWrapperValve.java:230)
at org.apache.catalina.core.StandardContextValve.invoke(StandardContextValve.java:175)
at org.apache.catalina.core.StandardHostValve.invoke(StandardHostValve.java:128)
at org.apache.catalina.valves.ErrorReportValve.invoke(ErrorReportValve.java:104)
at org.apache.catalina.core.StandardEngineValve.invoke(StandardEngineValve.java:109)
at org.apache.catalina.connector.CoyoteAdapter.service(CoyoteAdapter.java:261)
at org.apache.coyote.http11.Http11Processor.process(Http11Processor.java:844)
at org.apache.coyote.http11.Http11Protocol$Http11ConnectionHandler.process(Http11Protocol.java:581)
at org.apache.tomcat.util.net.JIoEndpoint$Worker.run(JIoEndpoint.java:447)
at java.lang.Thread.run(Thread.java:619)