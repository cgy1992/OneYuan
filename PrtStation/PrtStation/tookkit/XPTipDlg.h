#pragma once


// CXPTipDlg �Ի���

class CXPTipDlg : public CDialog
{
	DECLARE_DYNAMIC(CXPTipDlg)

public:
	CXPTipDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CXPTipDlg();

// �Ի�������
	enum { IDD = IDD_XPTIPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	void showtip(CPoint point, CString strText);
	void hidetip();

	afx_msg void OnPaint();

private:
	BOOL m_bTracking;   //����갴��û���ͷ�ʱ��ֵΪtrue
	BOOL m_isShow;
	CString m_disText;
	CPoint m_disPoint;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
};
