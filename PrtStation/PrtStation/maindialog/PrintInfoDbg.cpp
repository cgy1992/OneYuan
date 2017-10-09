// PrintInfoDbg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PrtStation.h"
#include "PrintInfoDbg.h"
#include "afxdialogex.h"

#include "etpLib.h"
#include "prtGlobal.h"
#include "serverComm.h"
#include "printComm.h"
#include "photoConvert.h"

// CPrintInfoDbg �Ի���

IMPLEMENT_DYNAMIC(CPrintInfoDbg, CDialogEx)

CPrintInfoDbg::CPrintInfoDbg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPrintInfoDbg::IDD, pParent)
{

}

CPrintInfoDbg::~CPrintInfoDbg()
{
}

void CPrintInfoDbg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SEX, m_cmb_sex);
}


BEGIN_MESSAGE_MAP(CPrintInfoDbg, CDialogEx)
	ON_BN_CLICKED(ID_PRINT_OK, &CPrintInfoDbg::OnBnClickedPrintOk)
	ON_BN_CLICKED(ID_PRINTCANCEL, &CPrintInfoDbg::OnBnClickedPrintcancel)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_BTN_PHOTO_SEL, &CPrintInfoDbg::OnBnClickedBtnPhotoSel)
	ON_BN_CLICKED(IDC_BUT_PRT_IC_CHK, &CPrintInfoDbg::OnBnClickedButPrtIcChk)
	ON_BN_CLICKED(IDC_BUT_PRT_RESET, &CPrintInfoDbg::OnBnClickedButPrtReset)
	ON_BN_CLICKED(IDC_BUT_PRT_CLEAR, &CPrintInfoDbg::OnBnClickedButPrtClear)
END_MESSAGE_MAP()


// CPrintInfoDbg ��Ϣ�������

int CPrintInfoDbg::getTestPrintInfo(PRT_INFO_T *cardInfo)
{
	CEdit*  pEdit = NULL;
	CString name;
	CString cardID;
	CString cardNum;
	CString cardLaunchTime;
	CString cardLaunchPlace;
	CString sex;

	memset(cardInfo, 0, sizeof(PRT_INFO_T));

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_NAME);
	pEdit->GetWindowText(name);
	if (name.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("��������̫��������"), _T("�Ƿ�����"));
		return ET_ERROR;
	}

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_ID);
	pEdit->GetWindowText(cardID);
	if (cardID.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("�籣��ID����̫��������"), _T("�Ƿ�����"));
		return ET_ERROR;
	}

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_NUM);
	pEdit->GetWindowText(cardNum);
	if (cardNum.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("�籣���볤��̫��������"), _T("�Ƿ�����"));
		return ET_ERROR;
	}
	
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_LAUNCHPLACE);
	pEdit->GetWindowText(cardLaunchTime);
	if (cardLaunchTime.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("�������ڳ���̫��������"), _T("�Ƿ�����"));
		return ET_ERROR;
	}

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LAUNCHPLACE2);
	pEdit->GetWindowText(cardLaunchPlace);
	if (cardLaunchPlace.GetLength() > OWNER_TEXT_MAX_LEN)
	{
		MessageBox(_T("�����س���̫��������"), _T("�Ƿ�����"));
		return ET_ERROR;
	}

	m_cmb_sex.GetWindowTextA(sex);
	
	CFile ImageFile;
	char * szImageBin = NULL;
	if (0 == ImageFile.Open(g_testPhotoDir, CFile::modeRead))
	{
		free(cardInfo);
		MessageBox(_T("�򿪲����ļ�ʧ�ܣ�����"), _T("����"));
		return ET_ERROR;
	}

	unsigned int iSrcLen = (int)ImageFile.GetLength();

	szImageBin = (char*)malloc(OWNER_PHOTO_MAX_LEN);
	if (NULL == szImageBin)
	{
		free(cardInfo);
		ImageFile.Close();
		MessageBox(_T("�ڴ治�㣡����"), _T("����"));
		return ET_ERROR;
	}
	ImageFile.Read((void*)szImageBin,iSrcLen);
	ImageFile.Close();


	PHOTO_TYPE_E photoType = g_photoConvert.getPhotoType(szImageBin, iSrcLen);
	if (PHOTO_INVALID == photoType)
	{
		free(cardInfo);
		free(szImageBin);
		MessageBox(_T("photo type invalid������"), _T("����"));
		return ET_ERROR;
	}
	else if (PHOTO_JPG != photoType)
	{
		if(OK != g_photoConvert.convertToJpg(szImageBin, iSrcLen,
			szImageBin, OWNER_PHOTO_MAX_LEN, &iSrcLen))
		{
			free(cardInfo);
			free(szImageBin);
			MessageBox(_T("photo convert failed������"), _T("����"));
			return ET_ERROR;
		}
	}

	unsigned int baseLen = 0;
	char* imgBase64 = base64encode(szImageBin, iSrcLen, &baseLen);
	if (NULL == imgBase64)
	{
		free(cardInfo);
		free(szImageBin);
		MessageBox(_T("base64 ����ʧ�ܣ�����"), _T("����"));
		return ET_ERROR;
	}
	free(szImageBin);

	if (baseLen > OWNER_PHOTO_MAX_LEN)
	{
		free(imgBase64);

		CString strTmp;
		strTmp.Format("��Ƭ̫�󣬳��� %lu KB������", OWNER_PHOTO_MAX_LEN/1024);
		MessageBox(_T(strTmp), _T("����"));
		return ET_ERROR;
	}

	memcpy(cardInfo->ownerPhoto, imgBase64, baseLen);
	cardInfo->ownerPhotoLen = baseLen;

	free(imgBase64);


	int fmtCnt = 0;
	PRT_DATA_FMT_T *dataFmt = NULL;
	dataFmt = icDataFmtInfoGet(&fmtCnt);

	PRT_TEMPLATE_T *prtFormat = NULL;
	prtFormat = prtTemplateGet();

	int i = 0;
	for (i = 0; i<fmtCnt; i++)
	{
		if (dataFmt[i].isPrint)
		{
			if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerNameAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerNameAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->ownerName, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName, name);
				}
				else
				{
					strncpy(cardInfo->ownerName, name, OWNER_TEXT_MAX_LEN);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerIdentityNumAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerIdentityNumAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->ownerIdentityNum, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName, cardID);
				}
				else
				{
					strncpy(cardInfo->ownerIdentityNum, cardID, OWNER_TEXT_MAX_LEN);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerCardNumAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerCardNumAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->ownerCardNum, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName, cardNum);
				}
				else
				{
					strncpy(cardInfo->ownerCardNum, cardNum, OWNER_TEXT_MAX_LEN);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->ownerSexAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->ownerSexAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->ownerSex, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName,
															sex);
				}
				else
				{
					SNPRINTF(cardInfo->ownerSex, OWNER_TEXT_MAX_LEN,
															"%s",
															sex);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->cardLaunchTimeAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->cardLaunchTimeAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->cardLaunchTime, OWNER_TEXT_MAX_LEN,
															"%s %s",
															dataFmt[i].printName, cardLaunchTime);
				}
				else
				{
					SNPRINTF(cardInfo->cardLaunchTime, OWNER_TEXT_MAX_LEN,
						"%s", cardLaunchTime);
				}
			}
			else if (0 == strncmp(dataFmt[i].attrName, prtFormat->cardLaunchPlaceAttr.attrName, ATTR_NAME_MAX_LEN))
			{
				if (prtFormat->cardLaunchPlaceAttr.isPrintStatic)
				{
					SNPRINTF(cardInfo->cardLaunchPlace, OWNER_TEXT_MAX_LEN,
						"%s %s",
						dataFmt[i].printName, cardLaunchPlace);
				}
				else
				{
					SNPRINTF(cardInfo->cardLaunchPlace, OWNER_TEXT_MAX_LEN,
						"%s", cardLaunchPlace);
				}
			}
		}
	}
	return OK;
}

void CPrintInfoDbg::OnBnClickedPrintOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g_taskHdl->isTaskingFull())
	{
		MessageBox(_T("��ǰ��ӡ�������������ύ��"), _T("��ʾ"));
		return;
	}

	PRT_TASK_T *taskInfo = (PRT_TASK_T*)malloc(sizeof(PRT_TASK_T));
	if (NULL == taskInfo)
	{
		MessageBox(_T("�ڴ治��������"), _T("����"));
		return;
	}

	memset(taskInfo, 0, sizeof(PRT_TASK_T));

	time_t nowTime = time(NULL); //��ȡĿǰ��ʱ��
	tm* local; //����ʱ��
	local = localtime(&nowTime); //תΪ����ʱ��
	strftime(taskInfo->timeBuf, sizeof(taskInfo->timeBuf) - 1, "%Y-%m-%d %H:%M:%S", local);

	CButton *pBut = NULL;
	pBut = (CButton*)GetDlgItem(IDC_CHECK_PRT);
	if (pBut->GetCheck() == BST_CHECKED)
	{
		taskInfo->flag |= FLAG_PRINT;
	}

	pBut = (CButton*)GetDlgItem(IDC_CHECK_WR);
	if (pBut->GetCheck() == BST_CHECKED)
	{
		taskInfo->flag |= FLAG_WRITE_IC;

		extern int gzzTaskDataBuild(char *outIcStr);
		if (OK != gzzTaskDataBuild(taskInfo->outIcStr))
		{
			free(taskInfo);
			MessageBox(_T("����IC����ʧ�ܣ�����"), _T("����"));
			return;
		}
	}

	if(OK != this->getTestPrintInfo(&taskInfo->outPrtInfo))
	{
		free(taskInfo);
		return;
	}

	/*�����ʼ״̬*/
	taskInfo->isTest = TRUE;
	g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_INIT);
	if(OK != g_prtService->printTaskHdl(taskInfo))
	{
		/*����Init��ʱ���Ѿ��������,�����Լ��ͷ�*/
		//free(taskInfo);
	}

	CDialog::OnOK();
}


void CPrintInfoDbg::OnBnClickedPrintcancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialog::OnCancel();
}



BOOL CPrintInfoDbg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_cmb_sex.InsertString(0, _T("��"));
	m_cmb_sex.InsertString(1, _T("Ů"));
	m_cmb_sex.SetCurSel(0);

	CEdit*  pEdit=(CEdit*)GetDlgItem(IDC_EDIT_NAME);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(_T("����")); //����Ĭ����ʾ������

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_ID);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(_T("123456789012345678")); //����Ĭ����ʾ������

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_NUM);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(_T("123456789")); //����Ĭ����ʾ������

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LAUNCHPLACE);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(_T("2017��7��")); //����Ĭ����ʾ������

	pEdit = (CEdit*)GetDlgItem(IDC_EDIT_LAUNCHPLACE2);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(_T("�Ĵ��ɶ�")); //����Ĭ����ʾ������

	//����·������ͼƬ
	HRESULT hResult = m_image.Load(_T(g_testPhotoDir));
	if (FAILED(hResult)) 
	{
		MessageBox(_T("���ز���ͼƬʧ�ܣ�"), _T("����ʧ��"));
		return FALSE;
	}

	CButton *pBut = NULL;
	pBut = (CButton*)GetDlgItem(IDC_CHECK_PRT);
	pBut->SetCheck(TRUE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CPrintInfoDbg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������

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
	pWnd = GetDlgItem(IDC_STATIC_PHOTO);//��ȡ�ؼ����

	//��ȡPicture Control�ؼ��Ĵ�С
	//pWnd->GetWindowRect(&rect);
	//���ͻ���ѡ�е��ؼ���ʾ�ľ���������
	//ScreenToClient(&rect);

	//�����ƶ����ؼ���ʾ������
	//pWnd->MoveWindow(rect.left, rect.top, cx, cy, TRUE);

	pWnd->GetClientRect(&rect);

	CDC *pDc = NULL;
	pDc	= pWnd->GetDC();//��ȡpicture��DC
	m_image.Draw(pDc->m_hDC, rect);//��ͼƬ���Ƶ�picture��ʾ��������
	ReleaseDC(pDc);
	// ��Ϊ��ͼ��Ϣ���� CDialogEx::OnPaint()
}


void CPrintInfoDbg::OnBnClickedBtnPhotoSel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	 // ���ù�����   
	TCHAR szFilter[] = _T("��Ƭ�ļ�(*.jpg;*.png;*.gif;*.bmp;*.pcx;*.tif;*.psd)|*.jpg;*.png;*.gif;*.bmp;*.pcx;*.tif;*.psd");
    // ������ļ��Ի���   
    CFileDialog fileDlg(TRUE, _T("txt"), NULL, 0, szFilter, this);   
    CString strFilePath;   
  
    // ��ʾ���ļ��Ի���   
    if (IDOK == fileDlg.DoModal())   
    {   
        // ���������ļ��Ի����ϵġ��򿪡���ť����ѡ����ļ�·����ʾ���༭����   
        strFilePath = fileDlg.GetPathName(); 

		strncpy(g_testPhotoDir, _T(strFilePath), MAX_PATH);

		//����·������ͼƬ
		HRESULT hResult = m_image.Load(_T(g_testPhotoDir));
		if (FAILED(hResult)) 
		{
			MessageBox(_T("������Ƭʧ�ܣ�"), _T("����ʧ��"));
			return;
		}

		/*����wm_paint��Ϣ*/
		this->Invalidate();
    }   
}


void CPrintInfoDbg::OnBnClickedButPrtIcChk()
{
	// TODO: Add your control notification handler code here
	if (g_taskHdl->getTaskingCnt() > 0)
	{
		MessageBox("����������ִ��,��ȴ������������ִ�д˲���", "��ʾ", MB_OK | MB_ICONWARNING);
		return;
	}

	PRT_PRINTER_CFG_T *prtParam = printerCfgGet();
	char errData[512] = {0};
	if (OK != XpsCardGoOut(prtParam->printer, prtParam->cardReaderType, errData))
	{
		CString str;
		str.Format(_T("����λʧ��: %s"), errData);
		MessageBox(str, "��ʾ", MB_OK | MB_ICONWARNING);
		return;
	}

	g_prtService->m_printerStopped = TRUE;

	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo; //This is an [in] parameter
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo; //Only compulsory field

	char exefile[256] = { 0 };
	SNPRINTF(exefile, 256, "%s������������⹤��\\Psam�����-������⹤��.exe", g_localModDir);
	if (CreateProcess(exefile, NULL,
		NULL, NULL, FALSE, 0, NULL,
		NULL, &StartupInfo, &ProcessInfo))
	{
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}
	else
	{
		MessageBox("The process could not be started...");
	}

	g_prtService->m_printerStopped = FALSE;

	XpsCardGoBack();
}


void CPrintInfoDbg::OnBnClickedButPrtReset()
{
	// TODO: Add your control notification handler code here
	if (g_taskHdl->getTaskingCnt() > 0)
	{
		MessageBox("����������ִ��,��ȴ������������ִ�д˲���", "��ʾ", MB_OK | MB_ICONWARNING);
		return;
	}

	g_prtService->m_printerStopped = TRUE;

	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo; //This is an [in] parameter
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof StartupInfo; //Only compulsory field

	char exefile[256] = { 0 };
	SNPRINTF(exefile, 256, "%s���ô�ӡ������\\��ӡ��.exe", g_localModDir);
	if (CreateProcess(exefile, NULL,
		NULL, NULL, FALSE, 0, NULL,
		NULL, &StartupInfo, &ProcessInfo))
	{
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}
	else
	{
		MessageBox("The process could not be started...");
	}

	g_prtService->m_printerStopped = FALSE;
}


void CPrintInfoDbg::OnBnClickedButPrtClear()
{
	// TODO: Add your control notification handler code here
	if (MessageBox("����豸��ɫ���ó������Ұ���࿨���ڻ����ϣ�Ȼ�������.\n�Ƿ������", "��ʾ", MB_OKCANCEL|MB_ICONWARNING) != IDOK)
	{
		return;
	}

	PRT_PRINTER_CFG_T *prtParam = printerCfgGet();
	int ret = XpsCleanPrinter(prtParam->printer);
	if (ret == 0)
	{
		MessageBox(_T("����ӡ���ɹ�"));
	}
	else
	{
		MessageBox(_T("����ӡ��ʧ��"));
	}
}
