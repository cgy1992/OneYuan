// PrintParamDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "PrtStation.h"
#include "PrintParamDlg.h"
#include "afxdialogex.h"

#include "prtGlobal.h"

// CPrintParamDlg �Ի���

IMPLEMENT_DYNAMIC(CPrintParamDlg, CDialogEx)

CPrintParamDlg::CPrintParamDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPrintParamDlg::IDD, pParent)
{

}

CPrintParamDlg::~CPrintParamDlg()
{
}

void CPrintParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_IP_JIMIJI, m_JimijiIp);
	DDX_Control(pDX, IDC_COMB_SHEBAOSAM, m_listShebaokaSlot);
	DDX_Control(pDX, IDC_CHK_PIN, m_chkUsePin);

	DDX_Control(pDX, IDC_COM_PRINTER_RO, m_listPrtRO);
	DDX_Control(pDX, IDC_COMB_JIMIKA_RO, m_listJimikaRO);
	DDX_Control(pDX, IDC_COMB_CARDRDER, m_listCardReader);
	DDX_Control(pDX, IDC_CHK_IC_DEBUG, m_chkIcDebug);
	DDX_Control(pDX, IDC_COMB_JIAMI_TYPE, m_listJiamiType);
	DDX_Control(pDX, IDC_EDIT_STATION_NUM, m_edtStationNum);
	DDX_Control(pDX, IDC_EDIT_SERIAL_NUM, m_edtSerialNum);

	DDX_Control(pDX, IDC_CHK_MATCH_BANK, m_chkMatchBank);
	DDX_Control(pDX, IDC_CHK_MATCH_ATR, m_chkMatchAtr);
	DDX_Control(pDX, IDC_CHK_FORCE_MIYAO, m_chkForceMiyao);
	DDX_Control(pDX, IDC_COMB_PRT_JIMIKA_TYPE, m_listPrtJiamiType);
	DDX_Control(pDX, IDC_COMB_USERKA_TYPE, m_listUserKaType);
	DDX_Control(pDX, IDC_COMB_PRTTYPE, m_listDoType);
	DDX_Control(pDX, IDC_EDIT_TISHI_THRD, m_edtTishiThrd);
	DDX_Control(pDX, IDC_EDIT_STOP_THRD, m_edtStopThrd);
	DDX_Control(pDX, IDC_COM_DLLTYPE2, m_cmbDllType);
}


BEGIN_MESSAGE_MAP(CPrintParamDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CPrintParamDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_PIN, &CPrintParamDlg::OnBnClickedChkPin)
	ON_CBN_SELCHANGE(IDC_COMB_JIAMI_TYPE, &CPrintParamDlg::OnCbnSelchangeCombJiamiType)
	ON_BN_CLICKED(IDCANCEL, &CPrintParamDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CPrintParamDlg ��Ϣ�������


void CPrintParamDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strTmp;
	PRT_PRINTER_CFG_T printerParam;
	CEdit*  pEdit = NULL;

	if (g_prtService->isTaskHdling())
	{
		MessageBox(_T("��ǰ�д�ӡ���񣬲����ύ��"), _T("��ʾ"));
		return;
	}

	PRT_PRINTER_CFG_T* oldPrinterCfg = printerCfgGet();
	memcpy(&printerParam, oldPrinterCfg, sizeof(PRT_PRINTER_CFG_T));

	m_JimijiIp.GetWindowText(strTmp);
	strncpy(&printerParam.jimijiAddrDesc[0], strTmp.GetBuffer(0), DEV_IP_MAX_LEN);
	printerParam.jimijiAddr = inet_addr(printerParam.jimijiAddrDesc);
	printerParam.jimijiAddr = ntohl(printerParam.jimijiAddr );

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//��ȡ��Ӧ�ı༭��ID
	pEdit->GetWindowText(strTmp); //����Ĭ����ʾ������
	printerParam.jimijiPort = _tcstoul(strTmp, NULL, 10);

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//��ȡ��Ӧ�ı༭��ID
	pEdit->GetWindowText(strTmp); //����Ĭ����ʾ������
	strncpy(&printerParam.jimijiUserName[0], strTmp.GetBuffer(0), JIMIJI_USER_MAX_LEN);

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//��ȡ��Ӧ�ı༭��ID
	pEdit->GetWindowText(strTmp); //����Ĭ����ʾ������
	strncpy(&printerParam.jimijiPasswd[0], strTmp.GetBuffer(0), JIMIJI_PASSWD_MAX_LEN);

	printerParam.shebaoSamType = m_listJiamiType.GetCurSel() + 1;
	printerParam.shebaoSamSlot = m_listShebaokaSlot.GetCurSel() + 1;

	printerParam.jiamiSamType = m_listPrtJiamiType.GetCurSel() + 1;

	printerParam.userkaType = m_listUserKaType.GetCurSel() + 1;


	if (m_chkUsePin.GetCheck() == BST_CHECKED)
	{
		printerParam.isUsePin = 1;
	}
	else
	{
		printerParam.isUsePin = 0;
	}
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//��ȡ��Ӧ�ı༭��ID
	pEdit->GetWindowText(strTmp);
	strncpy(&printerParam.pinCode[0], strTmp.GetBuffer(0), PIN_CODE_MAX_LEN);

	/*������*/
	printerParam.cardReaderType = m_listCardReader.GetCurSel() + 1;
	/*ִ�з�ʽ*/
	printerParam.printDoType = m_listDoType.GetCurSel() + 1;

	/*�Ƿ�ģ��д��*/
	if (m_chkIcDebug.GetCheck() == BST_CHECKED)
	{
		printerParam.isNotWrIc = 1;
	}
	else
	{
		printerParam.isNotWrIc = 0;
	}

	if (m_chkMatchBank.GetCheck() == BST_CHECKED)
	{
		printerParam.isWrMatchBank = 1;
	}
	else
	{
		printerParam.isWrMatchBank = 0;
	}

	if (m_chkMatchAtr.GetCheck() == BST_CHECKED)
	{
		printerParam.isWrMatchAtr = 1;
	}
	else
	{
		printerParam.isWrMatchAtr = 0;
	}

	if (m_chkForceMiyao.GetCheck() == BST_CHECKED)
	{
		printerParam.isWrForceMiyao = 1;
	}
	else
	{
		printerParam.isWrForceMiyao = 0;
	}

	//CString strTmp;
	m_edtTishiThrd.GetWindowText(strTmp);
	printerParam.tishiThrd = atoi(strTmp);
	m_edtStopThrd.GetWindowText(strTmp);
	printerParam.stopThrd = atoi(strTmp);

	/*д����*/
	if (OK != printerCfgSave(&printerParam))
	{
		return;
	}
	
	/*���浱ǰ����*/
	int ii = m_cmbDllType.GetCurSel();
	/*���¼�������*/
	prtSetWrDllType(g_writeIcDlls[ii]);

	MessageBox("���óɹ�!", "��ʾ", MB_OK);
	CDialogEx::OnOK();
}


BOOL CPrintParamDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	CString str;
	PRT_PRINTER_CFG_T *printerParam = printerCfgGet();

	CEdit*  pEdit = NULL;
	
	for (int i = 0; i<g_dll_cnt; i++)
	{
		m_cmbDllType.InsertString(i, (LPCTSTR)(g_writeIcDlls[i])); //����һ��
		if (strncmp(g_writeIcDllTypes, g_writeIcDlls[i], sizeof(g_writeIcDllTypes)) == 0)
		{
			m_cmbDllType.SetCurSel(i);
		}
	}

	m_listJimikaRO.InsertString(0, _T("����һ"));
	m_listJimikaRO.InsertString(1, _T("���۶�"));
	m_listJimikaRO.InsertString(2, _T("������"));
	m_listJimikaRO.InsertString(3, _T("������"));
	if (printerParam->jiamiSamSlot > 0)
	{
		m_listJimikaRO.SetCurSel(printerParam->jiamiSamSlot - 1);
	}
	else
	{
		m_listJimikaRO.SetCurSel(2);
	}
	m_listJimikaRO.EnableWindow(FALSE);

	m_listPrtRO.InsertString(0, (LPCTSTR)(printerParam->printer));
	m_listPrtRO.SetCurSel(0);
	m_listPrtRO.EnableWindow(FALSE);



	m_listShebaokaSlot.InsertString(0, _T("����һ"));
	m_listShebaokaSlot.InsertString(1, _T("���۶�"));
	m_listShebaokaSlot.InsertString(2, _T("������"));
	m_listShebaokaSlot.InsertString(3, _T("������"));
	if (printerParam->shebaoSamSlot > 0)
	{
		m_listShebaokaSlot.SetCurSel(printerParam->shebaoSamSlot - 1);
	}
	else
	{
		m_listShebaokaSlot.SetCurSel(2);
	}

	str.Format(_T("%x"), prtGetStationNum());
	m_edtStationNum.SetWindowText(str);

	str.Format(_T("%x"), prtGetCurSerailNum());
	m_edtSerialNum.SetWindowText(str);

	m_JimijiIp.SetWindowText(printerParam->jimijiAddrDesc);
	str.Format(_T("%d"), printerParam->jimijiPort);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(str); //����Ĭ����ʾ������

	str.Format(_T("%s"), printerParam->jimijiUserName);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(str); //����Ĭ����ʾ������

	str.Format(_T("%s"), printerParam->jimijiPasswd);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(str); //����Ĭ����ʾ������

	m_listJiamiType.InsertString(0, _T("PSAM����ʽ"));
	m_listJiamiType.InsertString(1, _T("���ܻ���ʽ"));

	if (printerParam->shebaoSamType == SHEBAO_JIAMI_TYPE_PSAM
			|| printerParam->shebaoSamType == SHEBAO_JIAMI_TYPE_JIAMIJI)
	{
		m_listJiamiType.SetCurSel(printerParam->shebaoSamType - 1);
	}
	else
	{
		m_listJiamiType.SetCurSel(1);
	}

	if (printerParam->shebaoSamType == SHEBAO_JIAMI_TYPE_JIAMIJI)
	{
		m_listShebaokaSlot.EnableWindow(FALSE);
	}
	else
	{
		m_JimijiIp.EnableWindow(FALSE);

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(FALSE); //����Ĭ����ʾ������

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(FALSE); //����Ĭ����ʾ������

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(FALSE); //����Ĭ����ʾ������
	}


	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(printerParam->pinCode); //����Ĭ����ʾ������

	if (printerParam->isUsePin)
	{
		m_chkUsePin.SetCheck(1);
	}
	else
	{
		m_chkUsePin.SetCheck(0);

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(FALSE); //����Ĭ����ʾ������
	}

	m_listPrtJiamiType.InsertString(0, _T("�汾1(��)"));
	m_listPrtJiamiType.InsertString(1, _T("�汾2"));

	if (printerParam->jiamiSamType == PRT_JIAMI_TYPE_VER1
				|| printerParam->jiamiSamType == PRT_JIAMI_TYPE_VER2)
	{
		m_listPrtJiamiType.SetCurSel(printerParam->jiamiSamType - 1);
	}
	else
	{
		m_listPrtJiamiType.SetCurSel(1);
	}

	/*�û�������*/
	m_listUserKaType.InsertString(0, _T("����"));
	m_listUserKaType.InsertString(1, _T("����"));
	m_listUserKaType.InsertString(2, _T("����-�ܵ�"));
	m_listUserKaType.InsertString(3, _T("����"));

	if (printerParam->userkaType == USERKA_TYPE_DATANG
				|| printerParam->userkaType == USERKA_TYPE_HUADA
				|| printerParam->userkaType == USERKA_TYPE_HUAHONG)
	{
		m_listUserKaType.SetCurSel(printerParam->userkaType - 1);
	}
	else
	{
		m_listUserKaType.SetCurSel(0);
	}

	/*������*/
	m_listCardReader.InsertString(0, _T("HD100"));
	m_listCardReader.InsertString(1, _T("������д��"));
	m_listCardReader.InsertString(2, _T("���ܶ�д��"));
	m_listCardReader.InsertString(3, _T("ACR��д��"));
	if (printerParam->cardReaderType > 0)
	{
		m_listCardReader.SetCurSel(printerParam->cardReaderType - 1);
	}
	else
	{
		m_listCardReader.SetCurSel(0);
	}

	/*ִ�з�ʽ*/
	/*
	DO_PRT_AND_WR_IC = 1,
	DO_PRT_STEP1 = 2,
	DO_WR_IC_STEP1 = 3,
	*/
	m_listDoType.InsertString(0, _T("�Զ��򿨺�д��"));
	m_listDoType.InsertString(1, _T("�Զ����ֶ�д��"));
	m_listDoType.InsertString(2, _T("�ֶ�д���Զ���"));
	if (printerParam->printDoType > 0)
	{
		m_listDoType.SetCurSel(printerParam->printDoType - 1);
	}
	else
	{
		m_listDoType.SetCurSel(0);
	}

	/*�Ƿ�ģ��д��*/
	if (printerParam->isNotWrIc)
	{
		m_chkIcDebug.SetCheck(1);
	}
	else
	{
		m_chkIcDebug.SetCheck(0);
	}

	if (printerParam->isWrMatchBank)
	{
		m_chkMatchBank.SetCheck(1);
	}
	else
	{
		m_chkMatchBank.SetCheck(0);
	}

	if (printerParam->isWrMatchAtr)
	{
		m_chkMatchAtr.SetCheck(1);
	}
	else
	{
		m_chkMatchAtr.SetCheck(0);
	}

	if (printerParam->isWrForceMiyao)
	{
		m_chkForceMiyao.SetCheck(1);
	}
	else
	{
		m_chkForceMiyao.SetCheck(0);
	}

	CString strTmp;
	strTmp.Format(_T("%d"), printerParam->tishiThrd);
	m_edtTishiThrd.SetWindowText(strTmp);
	strTmp.Format(_T("%d"), printerParam->stopThrd);
	m_edtStopThrd.SetWindowText(strTmp);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CPrintParamDlg::OnBnClickedChkPin()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CEdit *pEdit = NULL;

	if (m_chkUsePin.GetCheck() == BST_CHECKED)
	{
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(TRUE); //����Ĭ����ʾ������
	}
	else
	{
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_PIN);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(FALSE); //����Ĭ����ʾ������
	}
}

void CPrintParamDlg::OnCbnSelchangeCombJiamiType()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int type =  m_listJiamiType.GetCurSel() + 1;
	CEdit*  pEdit = NULL;

	if (type == SHEBAO_JIAMI_TYPE_JIAMIJI)
	{
		m_JimijiIp.EnableWindow(TRUE);
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(TRUE); //����Ĭ����ʾ������

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(TRUE); //����Ĭ����ʾ������

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(TRUE); //����Ĭ����ʾ������

		m_listShebaokaSlot.EnableWindow(FALSE);
	}
	else
	{
		m_JimijiIp.EnableWindow(FALSE);
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_JIMIJI1);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(FALSE); //����Ĭ����ʾ������

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_USER);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(FALSE); //����Ĭ����ʾ������

		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_JIAMIJI_PASSWD);//��ȡ��Ӧ�ı༭��ID
		pEdit->EnableWindow(FALSE); //����Ĭ����ʾ������

		m_listShebaokaSlot.EnableWindow(TRUE);
	}
}




void CPrintParamDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}
