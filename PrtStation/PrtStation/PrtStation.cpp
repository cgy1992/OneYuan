
// PrtStation.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "PrtStation.h"
#include "PrtStationDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPrtStationApp

BEGIN_MESSAGE_MAP(CPrtStationApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPrtStationApp ����

CPrtStationApp::CPrtStationApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CPrtStationApp ����

CPrtStationApp theApp;


// CPrtStationApp ��ʼ��

BOOL CPrtStationApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	
	m_hMutex = CreateMutex(NULL, true, APP_NAME);
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{		
		MessageBox(NULL, _T("�ͻ�������������,�����ظ��򿪣�"), _T("��ᱣ�Ͽ���Է�ƿ�����վ"), MB_OK);
		//AfxMessageBox(_T("�ͻ�������������,�����ظ���"), MB_OK | MB_APPLMODAL | MB_ICONSTOP);
		return (FALSE);
	}

	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CPrtStationDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}



int CPrtStationApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class
	if (m_hMutex != NULL)
	{
		CloseHandle(m_hMutex);	//�رվ��
	}

	return CWinApp::ExitInstance();
}
