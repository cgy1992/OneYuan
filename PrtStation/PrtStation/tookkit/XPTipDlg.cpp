// XPTipDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PrtStation.h"
#include "XPTipDlg.h"
#include "afxdialogex.h"


// CXPTipDlg �Ի���

IMPLEMENT_DYNAMIC(CXPTipDlg, CDialog)

CXPTipDlg::CXPTipDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CXPTipDlg::IDD, pParent)
{
	m_bTracking = FALSE;
}

CXPTipDlg::~CXPTipDlg()
{
}

void CXPTipDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CXPTipDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()


// CXPTipDlg ��Ϣ�������


void CXPTipDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	//**�ػ汳��ɫ
	if (m_isShow)
	{
		CRect re;
		dc.SetBkColor(RGB(255,255,150));
		GetClientRect(&re);
		CBrush brush;
		brush.CreateSolidBrush(RGB(255,255,150));
		dc.SelectObject(&brush);
		//dc.Rectangle(re);

		dc.SetTextColor(RGB(0,0,0));
		dc.TextOut(0, 0, m_disText);
	}

	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
}

void CXPTipDlg::showtip(CPoint point1, CString strText)
{
	m_disText = strText;
	m_isShow = TRUE;

	DWORD pos=GetMessagePos();
	CPoint point = (LOWORD(pos),HIWORD(pos));//��ǰ����λ��

	point.x=LOWORD(pos);
	point.y=HIWORD(pos);

	CRect reDlg;
	reDlg.left=point.x;
	reDlg.top=point.y;
	reDlg.right=reDlg.left + 10 * strText.GetLength();
	reDlg.bottom=reDlg.top + 25;

	this->MoveWindow(reDlg); //���������Ļ��

	this->ShowWindow(TRUE);
	this->Invalidate();
}

void CXPTipDlg::hidetip()
{
	m_isShow = FALSE;

	this->ShowWindow(FALSE);
	this->Invalidate();
}




void CXPTipDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (!m_bTracking)
	{
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE;
		tme.dwHoverTime = 1;
		m_bTracking = _TrackMouseEvent(&tme);
	}

	CDialog::OnMouseMove(nFlags, point);
}


LRESULT CXPTipDlg::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_bTracking = FALSE;
	hidetip();

	return 0;
}
