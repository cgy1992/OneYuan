// FromInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PrtStation.h"
#include "FromInfoDlg.h"
#include "afxdialogex.h"

#include "prtGlobal.h"

// CFromInfoDlg �Ի���

IMPLEMENT_DYNAMIC(CFromInfoDlg, CDialogEx)

CFromInfoDlg::CFromInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFromInfoDlg::IDD, pParent)
{

}

CFromInfoDlg::~CFromInfoDlg()
{
}

void CFromInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_WEB_TITLE, m_edtWebTitle);
	DDX_Control(pDX, IDC_EDIT_WEB_TITLE1, m_edtFormTitle);
	DDX_Control(pDX, IDC_LIST_FORM, m_listFormFileds);
}


BEGIN_MESSAGE_MAP(CFromInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CFromInfoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CFromInfoDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CFromInfoDlg ��Ϣ�������


void CFromInfoDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString webTitle, formTitle;

	m_edtWebTitle.GetWindowText(webTitle);
	m_edtFormTitle.GetWindowText(formTitle);

	//if (webTitle.IsEmpty())
	//{
	//	MessageBox(_T("��ָ��web���⣡"), _T("��ʾ"));
	//	return;
	//}

	if( 1 != g_browserOper.formScan(webTitle, formTitle))
	{
		MessageBox(_T("��ȡ����Ϣʧ��,�����Ƿ���������ϴ򿪣�"), _T("��ʾ"));
		return;
	}

	if (0 == g_tmp_fields_cnt)
	{
		MessageBox(_T("��������,�����Ƿ���������ϴ򿪣�"), _T("��ʾ"));
		return;
	}

	int nRow = 0;
	m_listFormFileds.DeleteAllItems();
	for (int i = 0; i < g_tmp_fields_cnt; i++)
	{
		nRow = m_listFormFileds.InsertItem(0, g_tmp_fields[i].formName);//������
		m_listFormFileds.SetItemText(nRow, 1, g_form_type_desc[g_tmp_fields[i].formType]);
		m_listFormFileds.SetItemText(nRow, 2, g_tmp_fields[i].value);//��������				
	}
	
	//CString tmpStr;
	//tmpStr.Format("��ȡ��%d���ֶ���Ϣ", g_tmp_fields_cnt);
	//MessageBox(tmpStr, _T("��ʾ"));
	//CDialogEx::OnOK();
}


INT_PTR CFromInfoDlg::DoModal()
{
	// TODO: �ڴ����ר�ô����/����û���
	return CDialogEx::DoModal();
}


BOOL CFromInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	LONG lStyle; 
	lStyle = GetWindowLong(m_listFormFileds.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style 
	lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ 
	lStyle |= LVS_REPORT; //����style 
	SetWindowLong(m_listFormFileds.m_hWnd, GWL_STYLE, lStyle);//����style 
	DWORD dwStyle = m_listFormFileds.GetExtendedStyle(); 
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl�� 
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl�� 
	m_listFormFileds.SetExtendedStyle(dwStyle); //������չ��� 

	CRect rectList;
	m_listFormFileds.GetClientRect(rectList); //��õ�ǰ�ͻ�����Ϣ
	m_listFormFileds.InsertColumn( 0, "�ֶ���", LVCFMT_LEFT, rectList.Width()/4);//������
	m_listFormFileds.InsertColumn( 1, "����", LVCFMT_LEFT, rectList.Width()/4);
	m_listFormFileds.InsertColumn( 2, "ֵ", LVCFMT_LEFT, rectList.Width()/2);

	m_edtWebTitle.SetWindowText(g_browserOper.m_webTitle);
	m_edtFormTitle.SetWindowText(g_browserOper.m_formTitle);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CFromInfoDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}
