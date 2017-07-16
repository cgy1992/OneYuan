// PrinterRegDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "afxdialogex.h"

#include "etpLib.h"
#include "prtGlobal.h"
#include "printComm.h"

#include "PrtStation.h"
#include "PrinterRegDlg.h"

// PrinterRegDlg �Ի���

IMPLEMENT_DYNAMIC(PrinterRegDlg, CDialogEx)

PrinterRegDlg::PrinterRegDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(PrinterRegDlg::IDD, pParent)
{

}

PrinterRegDlg::~PrinterRegDlg()
{
}

void PrinterRegDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COM_PRINTER, m_listPrt);
	DDX_Control(pDX, IDC_COMB_JIMIKA, m_listJimika);
	DDX_Control(pDX, IDC_IP_SERVER, m_ServerIp);

	DDX_Control(pDX, IDC_COM_STATION, m_combStations);
	DDX_Control(pDX, IDC_COM_LOCALIP, m_combLocalIpaddr);
}


BEGIN_MESSAGE_MAP(PrinterRegDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &PrinterRegDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &PrinterRegDlg::OnBnClickedCancel)
	ON_CBN_SELCHANGE(IDC_COMB_JIMIKA, &PrinterRegDlg::OnCbnSelchangeCombJimika)
	ON_CBN_SELCHANGE(IDC_COM_STATION, &PrinterRegDlg::OnCbnSelchangeComStation)
END_MESSAGE_MAP()


void PrinterRegDlg::init_show(PRT_REG_CFG_T *regInfo, PRT_PRINTER_CFG_T *printerCfg)
{
	int count = 0;
	PRT_DEV_T *prtDev = NULL;
	prtDev = getWinPrinters(&count);
	if (NULL != prtDev)
	{
		m_listPrt.ResetContent();
		for(int i=0; i<count; i++)
		{
			m_listPrt.InsertString(i, (LPCTSTR)(prtDev[i].printerName)); //����һ��
		}
		m_listPrt.SetCurSel(0);
		
		for(int i=0; i<count; i++)
		{
			if (0 == strncmp(printerCfg->printer, (LPCTSTR)(prtDev[i].printerName), PRINTER_MAX_LEN))
			{
				m_listPrt.SetCurSel(i);
			}
		}

		free(prtDev);
	}
	else
	{
		MessageBox(_T("û�����Ӵ�ӡ����"), _T("��ʾ"));
	}

	m_listJimika.ResetContent();
	m_listJimika.InsertString(0, _T("����һ"));
	m_listJimika.InsertString(1, _T("���۶�"));
	m_listJimika.InsertString(2, _T("������"));
	m_listJimika.InsertString(3, _T("������"));
	if (printerCfg->jiamiSamSlot > 0)
	{
		m_listJimika.SetCurSel(printerCfg->jiamiSamSlot - 1);
	}
	else
	{
		m_listJimika.SetCurSel(2);
	}

	char jamiyinzi[PRINTER_JIMIKA_INFO_LEN + 1] = {0};
	char errData[RESULT_FAIL_STR_LEN + 1] = {0};

	CEdit*  pEdit = NULL;
	int ret = 0;
#if 1
	ret = XpsGetJamiYinzi(printerCfg->cardReaderType, printerCfg->jiamiSamSlot, jamiyinzi, errData);
	if (OK != ret)
	{
		MessageBox(_T(errData), _T("��ȡ��������ʧ��"));
	}
	else
	{
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//��ȡ��Ӧ�ı༭��ID
		pEdit->SetWindowText(jamiyinzi); //������ʾ������
	}
#else
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(regInfo->jimikaInfo); //����Ĭ����ʾ������
#endif

	CString str;

	m_ServerIp.SetWindowText(regInfo->serverAddrDesc);
	str.Format(_T("%d"), regInfo->serverPort);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_SERVER);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(str); //����Ĭ����ʾ������

	bool found = false;
	m_combLocalIpaddr.ResetContent();
	for (int ii = 0; ii < g_localip_cnt; ii++)
	{
		m_combLocalIpaddr.InsertString(ii, g_localip_desc[ii]);
		if (strncmp(regInfo->localAddrDesc, g_localip_desc[ii], 255) == 0)
		{
			m_combLocalIpaddr.SetCurSel(ii);
			found = true;
		}
	}
	if (found == false)
	{
		m_combLocalIpaddr.SetCurSel(0);
	}

	str.Format(_T("%d"), regInfo->localPort);
	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_LOCAL);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(str); //����Ĭ����ʾ������
}

// PrinterRegDlg ��Ϣ�������
BOOL PrinterRegDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	char *stationType = prtGetStationType();
	for(int i=0; i<g_station_cnt; i++)
	{
		m_combStations.InsertString(i, (LPCTSTR)(g_stations[i])); //����һ��
		if (strncmp(stationType, g_stations[i], MAX_STATION_NAME_LEN) == 0)
		{
			m_combStations.SetCurSel(i);
		}
	}
	

	PRT_REG_CFG_T *regInfo = registerCfgGet();
	PRT_PRINTER_CFG_T *printerCfg = printerCfgGet();

	this->init_show(regInfo, printerCfg);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void PrinterRegDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g_prtService->isRegistering())
	{
		MessageBox(_T("��ǰ����ע���У������ظ��ύ��"), _T("��ʾ"));
		return;
	}

	if (g_prtService->isTaskHdling())
	{
		MessageBox(_T("��ǰ�д�ӡ���񣬲����ύ��"), _T("��ʾ"));
		return;
	}

	/*���浱ǰ����*/
	int ii = m_combStations.GetCurSel();
	/*���¼�������*/
	if (sysConfigReload(g_stations[ii]) != OK)
	{
		MessageBox(_T("���¼�������ʧ��,������ѡվ������ã�"), _T("����"));
		return;
	}

	PRT_REG_CFG_T *oldRegisterCfg = registerCfgGet();
	PRT_PRINTER_CFG_T *oldPrinterCfg = printerCfgGet();

	PRT_REG_CFG_T regInfo;
	memcpy(&regInfo, oldRegisterCfg, sizeof(PRT_REG_CFG_T));
	PRT_PRINTER_CFG_T printerCfg;
	memcpy(&printerCfg, oldPrinterCfg, sizeof(PRT_PRINTER_CFG_T));
	char jiamiyinzi[PRINTER_JIMIKA_INFO_LEN + 1] = {0};

	CString strTmp;   
	int i = m_listPrt.GetCurSel();
	m_listPrt.GetLBText(i, strTmp);
	strncpy(&printerCfg.printer[0], strTmp.GetBuffer(0), PRINTER_MAX_LEN);

	if (printerCfg.printer[0] == 0)
	{
		MessageBox(_T("��ӡ��Ϊ�գ�����ע�ᣡ"), _T("��ʾ"));
		return;
	}

	printerCfg.jiamiSamSlot = m_listJimika.GetCurSel() + 1;

	CEdit*  pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//��ȡ��Ӧ�ı༭��ID
	pEdit->GetWindowText(strTmp); //����Ĭ����ʾ������
	strncpy(&jiamiyinzi[0], strTmp.GetBuffer(0), PRINTER_JIMIKA_INFO_LEN);

	m_ServerIp.GetWindowText(strTmp);
	strncpy(&regInfo.serverAddrDesc[0], strTmp.GetBuffer(0), DEV_IP_MAX_LEN);
	regInfo.serverAddr = inet_addr(regInfo.serverAddrDesc);
	regInfo.serverAddr = ntohl(regInfo.serverAddr );

	m_combLocalIpaddr.GetWindowText(strTmp);
	strncpy(&regInfo.localAddrDesc[0], strTmp.GetBuffer(0), DEV_IP_MAX_LEN);
	regInfo.localAddr = inet_addr(regInfo.localAddrDesc);
	regInfo.localAddr = ntohl(regInfo.localAddr );


	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_SERVER);//��ȡ��Ӧ�ı༭��ID
	pEdit->GetWindowText(strTmp); //����Ĭ����ʾ������
	regInfo.serverPort = _tcstoul(strTmp, NULL, 10);

	pEdit=(CEdit*)GetDlgItem(IDC_EDIT_PORT_LOCAL);//��ȡ��Ӧ�ı༭��ID
	pEdit->GetWindowText(strTmp); //����Ĭ����ʾ������
	regInfo.localPort = _tcstoul(strTmp, NULL, 10);

	/*��д����*/
	registerCfgSave(&regInfo);
	printerCfgSave(&printerCfg);
	saveJiamiyinzi(jiamiyinzi);

	/*��ע��*/	
	if (jiamiyinzi[0] == 0)
	{
		MessageBox(_T("��������Ϊ��,����ע��,��ȷ�ϴ�ӡ���Ƿ������ӣ�"), _T("��ʾ"));
	}
	else
	{
		g_prtService->registerTaskHdl();
	}
	CDialogEx::OnOK();
}


void PrinterRegDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}



void PrinterRegDlg::OnCbnSelchangeCombJimika()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	PRT_PRINTER_CFG_T *printerCfg = printerCfgGet();

	int iReadType = printerCfg->cardReaderType;
	int slot = m_listJimika.GetCurSel() + 1;	
	char jamiyinzi[PRINTER_JIMIKA_INFO_LEN + 1] = {0};
	char errData[RESULT_FAIL_STR_LEN + 1] = {0};

	CEdit*  pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//��ȡ��Ӧ�ı༭��ID
	pEdit->SetWindowText(jamiyinzi); //����Ĭ����ʾ������

	UpdateData(0);

	int ret = 0;

	ret = XpsGetJamiYinzi(iReadType, slot, jamiyinzi, errData);
	if (OK != ret)
	{
		MessageBox(_T(errData), _T("��ȡ��������ʧ��"));
	}
	else
	{
		pEdit=(CEdit*)GetDlgItem(IDC_EDIT_KH_X);//��ȡ��Ӧ�ı༭��ID
		pEdit->SetWindowText(jamiyinzi); //����Ĭ����ʾ������
	}
}


void PrinterRegDlg::OnCbnSelchangeComStation()
{
	// TODO: Add your control notification handler code here
	int ii = m_combStations.GetCurSel();

	PRT_PRINTER_CFG_T printerCfg;
	PRT_REG_CFG_T registerCfg;

	memset(&printerCfg, 0, sizeof(PRT_PRINTER_CFG_T));
	memset(&registerCfg, 0, sizeof(PRT_REG_CFG_T));

	if (OK != printerCfgRead(g_stations[ii], &printerCfg))
	{
		goto backSel;
	}
	if (OK != registerCfgRead(g_stations[ii], &registerCfg))
	{
		goto backSel;
	}

	this->init_show(&registerCfg, &printerCfg);
	UpdateData(0);
	return;

backSel:
	char *stationType = prtGetStationType();
	for(int i=0; i<g_station_cnt; i++)
	{
		if (strncmp(stationType, g_stations[i], MAX_STATION_NAME_LEN) == 0)
		{
			m_combStations.SetCurSel(i);
			break;
		}
	}
}
