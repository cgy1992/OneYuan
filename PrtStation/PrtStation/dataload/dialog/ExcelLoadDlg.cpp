// ExcelLoadDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PrtStation.h"
#include "ExcelLoadDlg.h"
#include "afxdialogex.h"

#include "FromInfoDlg.h"
#include "prtGlobal.h"

// CExcelLoadDlg �Ի���

IMPLEMENT_DYNAMIC(CExcelLoadDlg, CDialogEx)

CExcelLoadDlg::CExcelLoadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CExcelLoadDlg::IDD, pParent)
{

}

CExcelLoadDlg::~CExcelLoadDlg()
{
}

void CExcelLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_edtExcelFile);
	DDX_Control(pDX, IDC_LIST_INFO, m_listElment);
	DDX_Control(pDX, ID_BTN_LAST, m_btnPrev);
	DDX_Control(pDX, ID_BTN_NEXT, m_btnNext);
	DDX_Control(pDX, IDC_STC_TOTAL, m_stcTotal);
	DDX_Control(pDX, IDC_STC_CUR, m_stcCur);
}


BEGIN_MESSAGE_MAP(CExcelLoadDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_FILESEL, &CExcelLoadDlg::OnBnClickedBtnFilesel)
	ON_BN_CLICKED(ID_BTN_LAST, &CExcelLoadDlg::OnBnClickedBtnLast)
	ON_BN_CLICKED(ID_BTN_NEXT, &CExcelLoadDlg::OnBnClickedBtnNext)
	ON_BN_CLICKED(ID_BTN_LOAD, &CExcelLoadDlg::OnBnClickedBtnLoad)
	ON_BN_CLICKED(IDC_BTN_FILELOAD, &CExcelLoadDlg::OnBnClickedBtnFileload)
	ON_BN_CLICKED(ID_BTN_SCANFORM, &CExcelLoadDlg::OnBnClickedBtnScanform)
END_MESSAGE_MAP()


// CExcelLoadDlg ��Ϣ�������


void CExcelLoadDlg::OnBnClickedBtnFilesel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	
	 // ���ù�����   
    TCHAR szFilter[] = _T("excel�ļ�(*.xls)|*.xls");   
    // ������ļ��Ի���   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // ��ʾ���ļ��Ի���   
    if (IDOK == fileDlg.DoModal())   
    {   
        // ���������ļ��Ի����ϵġ��򿪡���ť����ѡ����ļ�·����ʾ���༭����   
        strFilePath = fileDlg.GetPathName(); 
		m_edtExcelFile.SetWindowText(strFilePath);

		/*�Զ�����*/
		this->OnBnClickedBtnFileload();
    }   
}

void CExcelLoadDlg::setListColumnText()
{
	int nRow = 0;

	m_listElment.DeleteAllItems();
	for (int i = g_browserOper.m_fieldCnt - 1; i >= 0; i--)
	{
		nRow = m_listElment.InsertItem(0, g_browserOper.m_fields[i].name);//������
		m_listElment.SetItemText(nRow, 1, g_excelOper->m_curRowData[i]);			
	}
}

void CExcelLoadDlg::setListColumnCaptain()
{
	int nRow = 0;

	m_listElment.DeleteAllItems();
	for (int i = g_browserOper.m_fieldCnt - 1; i >= 0; i--)
	{
		nRow = m_listElment.InsertItem(0, g_browserOper.m_fields[i].name);//������
		m_listElment.SetItemText(nRow, 1, "");			
	}
}


void CExcelLoadDlg::OnBnClickedBtnFileload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strFilePath; 
	m_edtExcelFile.GetWindowText(strFilePath);

	/*������֮ǰ��*/
	if (g_excelOper != NULL)
	{
		delete g_excelOper;
		g_excelOper = NULL;
	}

	g_excelOper = new CExcelOper;
	if(1 != g_excelOper->init(strFilePath))
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("��ʼ��Excel�ļ�ʧ��!"), _T("����"));
		return;
	}

	int colNum = g_excelOper->getColNum();
	int rowNum = g_excelOper->getRowNum();

	/*�����ж�*/
	if (rowNum == 0)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("Excel�ļ��ޱ���!"), _T("����"));
		return;
	}

	/*�ֶθ����Ƚ�*/
	if (colNum != g_browserOper.m_fieldCnt)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		CString tmpStr;
		tmpStr.Format(_T("Excel�ļ�����(%d)�������ļ����ֶθ���(%d)�����!"), colNum, g_browserOper.m_fieldCnt);
		MessageBox(tmpStr, _T("����"));
		return;
	}

	/*��ȡ����*/
	if (1 != g_excelOper->getFirstRow())
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("Excel�ļ���ȡ����ʧ��!"), _T("����"));
		return;
	}

	/*�Ƚ������ļ�*/
	for (int i = 0; i < colNum; i++)
	{
		if (0 != strcmp(((LPSTR)(LPCTSTR)g_excelOper->m_curRowData[i]), g_browserOper.m_fields[i].name))
		{
			CString tmpStr;
			tmpStr.Format(_T("Excel�ļ���%d��(%s)�������ļ����ֶ�����(%s)�����!"), i+1, 
				(LPSTR)(LPCTSTR)(g_excelOper->m_curRowData[i]),
				g_browserOper.m_fields[i].name);
			MessageBox(tmpStr, _T("����"));

			delete g_excelOper;
			g_excelOper = NULL;
			return;
		}
	}

	if (rowNum == 1)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("Excel�ļ�������!"), _T("��ʾ"));
		return;
	}

	/*��ȡ��һ������*/
	int ret = g_excelOper->getNextRow();
	if (GET_ROW_BOTTOM == ret)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("�ѵ�Excel�ļ�β!"), _T("��ʾ"));
		return;
	}
	else if (1 != ret)
	{
		delete g_excelOper;
		g_excelOper = NULL;

		MessageBox(_T("Excel�ļ���ȡ����ʧ��!"), _T("����"));
		return;
	}

	CString tmpStr1;
	tmpStr1.Format("�ܹ� %u ��", rowNum-1);
	this->m_stcTotal.SetWindowText(tmpStr1);

	tmpStr1.Format("��ǰ�� 1 ��");
	this->m_stcCur.SetWindowText(tmpStr1);
	this->m_btnPrev.EnableWindow(false);
	if (rowNum == 2)
	{
		/*ֻ��һ������*/
		this->m_btnNext.EnableWindow(false);
	}
	else
	{
		this->m_btnNext.EnableWindow(true);
	}

	this->setListColumnText();
}

void CExcelLoadDlg::OnBnClickedBtnLast()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (NULL == g_excelOper)
	{
		MessageBox(_T("û��ָ��Excel�ļ�!"), _T("��ʾ"));
		return;
	}

	int ret = g_excelOper->getPrevRow();
	if (GET_ROW_TOP == ret)
	{
		MessageBox(_T("�ѵ�Excel�ļ�ͷ!"), _T("��ʾ"));
		return;
	}
	else if (1 != ret)
	{
		MessageBox(_T("Excel�ļ���ȡ����ʧ��!"), _T("����"));
		return;
	}

	this->setListColumnText();

	int curRowNum = g_excelOper->getCurRowNum();
	CString tmpStr1;
	tmpStr1.Format("��ǰ�� %u ��", curRowNum-1);
	this->m_stcCur.SetWindowText(tmpStr1);

	if (curRowNum == 2)
	{
		this->m_btnPrev.EnableWindow(false);
	}
	this->m_btnNext.EnableWindow(true);
}

void CExcelLoadDlg::OnBnClickedBtnNext()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (NULL == g_excelOper)
	{
		MessageBox(_T("û��ָ��Excel�ļ�!"), _T("��ʾ"));
		return;
	}

	int ret = g_excelOper->getNextRow();
	if (GET_ROW_BOTTOM == ret)
	{
		MessageBox(_T("�ѵ�Excel�ļ�β!"), _T("��ʾ"));
		return;
	}
	else if (1 != ret)
	{
		MessageBox(_T("Excel�ļ���ȡ����ʧ��!"), _T("����"));
		return;
	}

	this->setListColumnText();

	int rowNum = g_excelOper->getRowNum();
	int curRowNum = g_excelOper->getCurRowNum();

	CString tmpStr1;
	tmpStr1.Format("��ǰ�� %u ��", curRowNum-1);
	this->m_stcCur.SetWindowText(tmpStr1);

	if (rowNum == curRowNum)
	{
		this->m_btnNext.EnableWindow(false);
	}
	this->m_btnPrev.EnableWindow(true);
}

void CExcelLoadDlg::OnBnClickedBtnLoad()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (NULL == g_excelOper)
	{
		MessageBox(_T("û��ָ��Excel�ļ�!"), _T("��ʾ"));
		return;
	}

	int curRowNum = g_excelOper->getCurRowNum();
	int colNum = g_excelOper->getColNum();
	if (curRowNum > 1)
	{
		/*��ǰ������*/
		g_browserOper.formFieldFill(g_excelOper->m_curRowData, colNum);
		if (g_tmp_wr_fields_cnt < colNum)
		{
			CString tmpStr1;
			tmpStr1.Format(_T("��� %u ���ֶ�,��ȷ����ҳ���Ƿ���� %s ҳ��!"), 
					g_tmp_wr_fields_cnt, (LPCSTR)(LPCTSTR)g_tmp_webTitle);
			MessageBox(tmpStr1, _T("��ʾ"));
		}
		else
		{
			MessageBox(_T("������!"), _T("��ʾ"));
		}
	}
}

void CExcelLoadDlg::OnBnClickedBtnScanform()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFromInfoDlg  formDlg;
	formDlg.DoModal();
}

BOOL CExcelLoadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	
	LONG lStyle; 
	lStyle = GetWindowLong(m_listElment.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style 
	lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ 
	lStyle |= LVS_REPORT; //����style 
	SetWindowLong(m_listElment.m_hWnd, GWL_STYLE, lStyle);//����style 
	DWORD dwStyle = m_listElment.GetExtendedStyle(); 
	dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl�� 
	dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl�� 
	m_listElment.SetExtendedStyle(dwStyle); //������չ��� 

	CRect rectList;
	m_listElment.GetClientRect(rectList); //��õ�ǰ�ͻ�����Ϣ
	m_listElment.InsertColumn( 0, "�ֶ�", LVCFMT_LEFT, rectList.Width()/4);//������
	m_listElment.InsertColumn( 1, "ֵ", LVCFMT_LEFT, rectList.Width() * 3/4);
	
	if (g_excelOper != NULL)
	{
		/*֮ǰ�򿪹��ĵ���ֱ�ӵ�������*/
		m_edtExcelFile.SetWindowText(g_excelOper->m_filePath);
		this->setListColumnText();

		int curRowNum = g_excelOper->getCurRowNum();
		int rowNum = g_excelOper->getRowNum();

		CString tmpStr1;
		tmpStr1.Format("�ܹ� %u ��", rowNum-1);
		this->m_stcTotal.SetWindowText(tmpStr1);

		tmpStr1.Format("��ǰ�� %u ��", curRowNum-1);
		this->m_stcCur.SetWindowText(tmpStr1);

		if (curRowNum <= 2)
		{
			/*�Ѿ�������*/
			this->m_btnPrev.EnableWindow(false);
		}

		if (curRowNum == rowNum)
		{
			/*�Ѿ�����β*/
			this->m_btnNext.EnableWindow(false);
		}
	}
	else
	{
		/*û�д򿪣�չʾ�б�ͷ*/
		this->setListColumnCaptain();
		this->m_btnPrev.EnableWindow(false);
		this->m_btnNext.EnableWindow(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
