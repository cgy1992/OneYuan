// XPEdit.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PrtStation.h"
#include "XPEdit.h"


// XPEdit

IMPLEMENT_DYNAMIC(XPEdit, CEdit)

XPEdit::XPEdit()
{
	m_nMinValue = 0;
    m_nMaxValue = 5000;
    m_nDelta = 1;
}

XPEdit::~XPEdit()
{
}


BEGIN_MESSAGE_MAP(XPEdit, CEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()



// XPEdit ��Ϣ�������
void XPEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	BOOL bRet = FALSE;
    CString str;
    GetWindowText(str);

    switch(nChar)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case VK_BACK:
        bRet = TRUE;
        break;
    case '.':
        if(str.Find('.')<0)
            bRet = TRUE;
        break;
    default:
        break;
    }
    if(bRet)
        CEdit::OnChar(nChar, nRepCnt, nFlags);    

	//CEdit::OnChar(nChar, nRepCnt, nFlags);
}


//��ȡ����ķ�Χ
void XPEdit::GetRange(int & min, int & max) const
{
    min = m_nMinValue;
    max = m_nMaxValue;
}
 
//��������ķ�Χ
void XPEdit::SetRange(int min, int max)
{
    m_nMinValue = min;
    m_nMaxValue = max;
}
 
//��ȡ����ֵ
int XPEdit::GetDelta()
{
    return m_nDelta;
}
 
//���ò���ֵ
void XPEdit::SetDelta(int nDelta)
{
    m_nDelta = nDelta;
}
 
 
//��ȡС��
int XPEdit::GetDecimal()const
{
    int n = 0;
    CString str;               
    GetWindowText(str);    
    n = int(atof(str)*10);  //ת��������
    return (n);  
}
 
//����С��
BOOL XPEdit::SetDecimal(int nDecimal)
{
    if (nDecimal > m_nMaxValue || nDecimal < m_nMinValue)
        return FALSE;
    CString str;
    str.Format("%d.%d",nDecimal/10,nDecimal%10);  
    SetWindowText(str);    
    return TRUE;
}
 
//�ı�С��ʱ
void XPEdit::ChangeDecimal(int step)
{
    int n = GetDecimal() + step * m_nDelta;
    if(n > m_nMaxValue)
        n = m_nMaxValue;
    else if(n < m_nMinValue)
        n = m_nMinValue;
    SetDecimal(n);
}
