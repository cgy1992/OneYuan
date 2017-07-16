#include "stdafx.h"
#include "SplashCtrl.h"
#include "PrtStation.h"

BEGIN_MESSAGE_MAP(CSplashCtrl, CWnd)
	//{{AFX_MSG_MAP(CSplashCtrl)
	ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CSplashCtrl::CSplashCtrl()
{
}

CSplashCtrl::~CSplashCtrl()
{
}

void CSplashCtrl::ShowSplashWindow()
{
#if 0
	/*���·�����xp��win7����ʾ����*/
#define XPS_USER_PHOTO_FILE   "test\\splash.jpg"
	char splashDir[MAX_PATH + 1] = { 0 };
	char localModDir[MAX_PATH + 1] = { 0 };

	HMODULE module = GetModuleHandle(0);
	GetModuleFileName(module, localModDir, MAX_PATH);
	char *ch = _tcsrchr(localModDir, _T('\\')); //�������һ��\���ֵ�λ�ã�������\������ַ�������\��
	ch[1] = 0;//NULL  ͨ������������szFilePath = ��szFilePath�ضϣ��ض����һ��\������ַ���������\��
	_snprintf_s(splashDir, MAX_PATH, _TRUNCATE, _T("%s%s"), localModDir, XPS_USER_PHOTO_FILE);

	CImage img;
	img.Load(splashDir);
	HBITMAP hbmp = (HBITMAP)img.operator HBITMAP();
	m_bitmap.DeleteObject();
	m_bitmap.Attach(hbmp);
#else
	m_bitmap.LoadBitmapA(IDB_BITMAP6);
#endif

	BITMAP bitmap;
	m_bitmap.GetBitmap(&bitmap);

	/*��������, ͬʱ�����wm_create*/
	CreateEx(0, AfxRegisterWndClass(0), NULL, WS_POPUP|WS_VISIBLE,
		0, 0, bitmap.bmWidth, bitmap.bmHeight, NULL, NULL);
}

int CSplashCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CenterWindow();
	return 0;
}

void CSplashCtrl::OnPaint()
{
	CPaintDC dc(this);

	CDC bitdc;
	bitdc.CreateCompatibleDC(&dc);
	bitdc.SelectObject(&m_bitmap);
	BITMAP bitmap;
	m_bitmap.GetBitmap(&bitmap);
	dc.BitBlt(0,0,bitmap.bmWidth, bitmap.bmHeight, &bitdc, 0, 0,SRCCOPY);
}

void CSplashCtrl::HideSplashWindow()
{
	this->ShowWindow(SW_HIDE);
}