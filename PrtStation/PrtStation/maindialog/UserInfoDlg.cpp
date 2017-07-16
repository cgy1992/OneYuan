// UserInfoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrtStation.h"
#include "UserInfoDlg.h"
#include "afxdialogex.h"

#include "prtGlobal.h"
#include "PrtTaskHdl.h"

// UserInfoDlg dialog

IMPLEMENT_DYNAMIC(UserInfoDlg, CDialogEx)

UserInfoDlg::UserInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(UserInfoDlg::IDD, pParent)
{
	memset(m_strTimeBuf, 0, sizeof(m_strTimeBuf));
	memset(m_strOwnerName, 0, sizeof(m_strOwnerName));
}

UserInfoDlg::~UserInfoDlg()
{
}

void UserInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PLACE2, m_edtPlace);
	DDX_Control(pDX, IDC_EDIT_NUM1, m_edtIder);
	DDX_Control(pDX, IDC_EDIT_ID1, m_edtCardNo);
	DDX_Control(pDX, IDC_EDIT_NAME1, m_edtName);
	DDX_Control(pDX, IDC_EDIT_ST_TASK, m_edtTaskStatus);
	DDX_Control(pDX, IDC_EDIT_ST_PRT, m_edtPrtStatus);
	DDX_Control(pDX, IDC_EDIT_ST_WR, m_edtWrStatus);
	DDX_Control(pDX, IDOK1, m_btnOk);
	DDX_Control(pDX, IDC_EDIT_TIME, m_edtTime);
}


BEGIN_MESSAGE_MAP(UserInfoDlg, CDialogEx)
	ON_BN_CLICKED(IDOK1, &UserInfoDlg::OnBnClickedOk1)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDCANCEL1, &UserInfoDlg::OnBnClickedCancel1)
END_MESSAGE_MAP()


// UserInfoDlg message handlers
void UserInfoDlg::setTaskInfo(char *strTime, char *strName)
{
	strncpy(m_strTimeBuf, strTime, 63);
	strncpy(m_strOwnerName, strName, OWNER_TEXT_MAX_LEN);
}


BOOL UserInfoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	::SetWindowPos(GetSafeHwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	/*��������չʾ*/
	PRT_TASK_T *taskInfo = NULL;

	MUTEX_LOCK(g_taskHdl->m_prtTasksLock);
	taskInfo = g_taskHdl->getTask(m_strTimeBuf, m_strOwnerName);
	if (NULL == taskInfo)
	{
		MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);
		MessageBox("�����Ѿ�������,�����Ƿ����!", "��ʾ", MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	m_edtName.SetWindowText(taskInfo->outPrtInfo.ownerName); 
	m_edtCardNo.SetWindowText(taskInfo->outPrtInfo.ownerCardNum);
	m_edtIder.SetWindowText(taskInfo->outPrtInfo.ownerIdentityNum);
	m_edtPlace.SetWindowText(taskInfo->outPrtInfo.cardLaunchPlace);
	m_edtTime.SetWindowText(taskInfo->timeBuf);

	if (g_taskHdl->isTaskHdlEnd(taskInfo))
	{
		if (taskInfo->endByErrors)
		{
			m_edtTaskStatus.SetWindowText("�쳣����");
		}
		else
		{
			m_edtTaskStatus.SetWindowText("��������");
		}
	}
	else
	{
		m_edtTaskStatus.SetWindowText("δ���");
	}

	if (taskInfo->printSt == ST_INIT)
	{
		m_edtPrtStatus.SetWindowText("δִ��");
	}
	else if (taskInfo->printSt == ST_SUCCSS)
	{
		m_edtPrtStatus.SetWindowText("��������");
	}
	else if (taskInfo->printSt == ST_FAILED)
	{
		m_edtPrtStatus.SetWindowText("�쳣����");
	}

	if (taskInfo->outIcSt == ST_INIT)
	{
		m_edtWrStatus.SetWindowText("δִ��");
	}
	else if (taskInfo->outIcSt == ST_SUCCSS)
	{
		m_edtWrStatus.SetWindowText("��������");
	}
	else if (taskInfo->outIcSt == ST_FAILED)
	{
		m_edtWrStatus.SetWindowText("����");
	}

	int operFlag = g_taskHdl->getTaskOperFlag(taskInfo);
	if (0 == operFlag)
	{
		m_btnOk.EnableWindow(FALSE);
	}
	else if (FLAG_PRINT == operFlag)
	{
		m_btnOk.SetWindowText("��");
	}
	else if (FLAG_WRITE_IC == operFlag)
	{
		m_btnOk.SetWindowText("д��");
	}

	if (OK != g_taskHdl->photoToFile(taskInfo->outPrtInfo.ownerPhoto, taskInfo->outPrtInfo.ownerPhotoLen, g_testUserDir))
	{
		MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);
		MessageBox("ͼƬ����ʧ��,�����Ƿ����!", "��ʾ", MB_OK | MB_ICONWARNING);
		return FALSE;
	}
	MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);

	//����·������ͼƬ
	HRESULT hResult = m_image.Load(_T(g_testUserDir));
	if (FAILED(hResult))
	{
		MessageBox(_T("���ز���ͼƬʧ�ܣ�"), _T("����ʧ��"));
		return FALSE;
	}

	return TRUE;
}



void UserInfoDlg::OnBnClickedOk1()
{
	// TODO: Add your control notification handler code here
	PRT_TASK_T *taskInfo = NULL;

	MUTEX_LOCK(g_taskHdl->m_prtTasksLock);
	taskInfo = g_taskHdl->getTask(m_strTimeBuf, m_strOwnerName);
	if (NULL == taskInfo)
	{
		MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);
		MessageBox("�����Ѿ�������,�����Ƿ����!", "��ʾ", MB_OK);
		return;
	}

	taskInfo->operFlag = g_taskHdl->getTaskOperFlag(taskInfo);
	if (0 == taskInfo->operFlag)
	{
		MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);
		MessageBox("�����Ѿ��������,�����ٴ���!", "��ʾ", MB_OK);
		return;
	}
	
	/*ִ������*/
	g_prtService->printTaskHdl(taskInfo);

	MUTEX_UNLOCK(g_taskHdl->m_prtTasksLock);

	/*�ر��Լ�������*/
	this->OnOK();
	return;
}


void UserInfoDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialogEx::OnPaint() for painting messages


	if (m_image.IsNull())
	{
		return;
	}


	//int cx, cy;
	//��ȡͼƬ�Ŀ� �߶�
	//cx	= m_image.GetWidth();
	//cy	= m_image.GetHeight();

	CWnd *pWnd = NULL;
	CRect	rect;
	pWnd = GetDlgItem(IDC_STATIC_PHOTO1);//��ȡ�ؼ����

	//��ȡPicture Control�ؼ��Ĵ�С
	//pWnd->GetWindowRect(&rect);
	//���ͻ���ѡ�е��ؼ���ʾ�ľ���������
	//ScreenToClient(&rect);

	//�����ƶ����ؼ���ʾ������
	//pWnd->MoveWindow(rect.left, rect.top, cx, cy, TRUE);

	pWnd->GetClientRect(&rect);

	CDC *pDc = NULL;
	pDc = pWnd->GetDC();//��ȡpicture��DC
	m_image.Draw(pDc->m_hDC, rect);//��ͼƬ���Ƶ�picture��ʾ��������
	ReleaseDC(pDc);
}


void UserInfoDlg::OnBnClickedCancel1()
{
	// TODO: Add your control notification handler code here
	//�����ø����OnCancel�� ����õ�ģʽ�Ի����EndDialog()��
	this->OnCancel();
}


void UserInfoDlg::PostNcDestroy()
{
	// TODO: Add your specialized code here and/or call the base class
	delete this;
	CDialogEx::PostNcDestroy();
}


void UserInfoDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogEx::OnCancel();
	DestroyWindow();
}


void UserInfoDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//CDialogEx::OnOK();
	DestroyWindow();
}
