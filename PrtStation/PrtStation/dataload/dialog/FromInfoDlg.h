#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CFromInfoDlg �Ի���

class CFromInfoDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFromInfoDlg)

public:
	CFromInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFromInfoDlg();

// �Ի�������
	enum { IDD = IDD_DLG_FORM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_edtWebTitle;
	CEdit m_edtFormTitle;
	afx_msg void OnBnClickedOk();
	virtual INT_PTR DoModal();
	CListCtrl m_listFormFileds;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton1();
};
