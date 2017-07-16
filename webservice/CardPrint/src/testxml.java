




import java.io.FileOutputStream;

import org.jdom2.Attribute;
import org.jdom2.Comment;
import org.jdom2.Document;
import org.jdom2.Element;
import org.jdom2.output.Format;
import org.jdom2.output.XMLOutputter;

/**
 * ʹ��JDOM����һ��XML�ĵ��������
 * 
 */
public class testxml
{

    public static void main(String[] args) throws Exception
    {
        // �ĵ�
        Document document = new Document();

        // ����һ��Ԫ��
        Element root = new Element("inputxml");

        // �����ɵ�Ԫ�ؼ����ĵ�����Ԫ��
        document.addContent(root);

        // ��������ע��
        Comment comment = new Comment("This is my comments");
        root.addContent(comment);

        // ������Ԫ��1
        Element e = new Element("isOK");
        // ������
       
        e.setText("isok");
        root.addContent(e);

        // ����Ԫ��2
        Element e2 = new Element("world");
        Attribute attribute = new Attribute("testAttr", "attr Value");
        e2.setAttribute(attribute);// set�����᷵��Ԫ�ر���������method chain style��
        root.addContent(e2);

        e2.addContent(new Element("subElement").setAttribute("a", "aValue")
                .setAttribute("x", "xValue").setAttribute("y", "yValue")
                .setText("textContent"));

        // ��ʽ��
        Format format = Format.getPrettyFormat();
        // Format.getRawFormat()������ͨ������XML���ݵ����紫�䣬��Ϊ���ָ�ʽ��ȥ�����в���Ҫ�Ŀհף�����ܹ�����������

        // �����Լ��趨һЩformat������
        format.setIndent("    ");// ��������Ϊ�ĸ��ո�Ĭ��Ϊ�����ո�

        // ���
        XMLOutputter out = new XMLOutputter(format);
        out.output(document, new FileOutputStream("jdom.xml"));// ���ڵ�ǰ��Ŀ·�����ҵ�

    }
}