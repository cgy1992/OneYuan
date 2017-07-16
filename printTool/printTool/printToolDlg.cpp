
// printToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "printTool.h"
#include "printToolDlg.h"
#include "afxdialogex.h"

#include "etpLib.h"
#include "serverComm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CprintToolDlg �Ի���




CprintToolDlg::CprintToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CprintToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CprintToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CprintToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CprintToolDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CprintToolDlg ��Ϣ�������

BOOL CprintToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_PORT);
	pEdit->SetWindowText("6832");

	CIPAddressCtrl *pIpCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1);
	pIpCtrl->SetWindowText("127.0.0.1");

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CprintToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CprintToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CprintToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




int CprintToolDlg::getTestCardInfo(CARD_INFO_T *cardInfo)
{
	CString name;
	CString cardID;
	CString cardNum;


	memset(cardInfo, 0, sizeof(CARD_INFO_T));
	
	strncpy(cardInfo->dataSource, "519900", DATA_SOURCE_LEN);
	strncpy(cardInfo->ownerMedicalPlace, "001789", OWNER_MEDICAL_PLACE_MAX_LEN);
	strncpy(cardInfo->cardIdCode, "519900d156000005", CARD_ID_CODE_MAX_LEN);
	strncpy(cardInfo->cardType, "2", CARD_TYPE_MAX_LEN);
	strncpy(cardInfo->standardVersion, "0123", STANDARD_VERSION_MAX_LEN);
	strncpy(cardInfo->orginazeNum, "915600000236990036010a03", ORG_NUM_MAX_LEN);
	strncpy(cardInfo->cardLaunchTime, "20050712", CARD_LAUCHTIME_MAX_LEN);
	strncpy(cardInfo->cardValidTime, "20061713", CARD_VALIDTIME_MAX_LEN);
	strncpy(cardInfo->ownerCardNum, "123456789", OWNER_CARD_NUM_MAX_LEN);
	strncpy(cardInfo->ownerIdentityNum, "123456789012345678", OWNER_IDENTITY_NUM_MAX_LEN);
	strncpy(cardInfo->ownerName, "�ŭ|", OWNER_NAME_MAX_LEN);
	strncpy(cardInfo->ownerNameOther, "���ڹ�Է���ɫ", OWNER_NAME_MAX_LEN);
	strncpy(cardInfo->ownerSex, "A0123456", OWNER_SEX_MAX_LEN);
	strncpy(cardInfo->ownerVolk, "02", OWNER_VOLK_MAX_LEN);
	strncpy(cardInfo->ownerBirthPlace, "002003", OWNER_BIRTHPLACE_MAX_LEN);
	strncpy(cardInfo->ownerBirthDay, "19910901", OWNER_BIRTHDAY_MAX_LEN);

	strncpy(cardInfo->ownerBankCode, "", BANK_CODE_LEN);
	strncpy(cardInfo->ownerResidencePlace, "�ɶ��������", OWNER_RESIDENCE_PLACE_MAX_LEN);
	strncpy(cardInfo->ownerResidenceType, "023567", OWNER_RESIDENCE_TYPE_MAX_LEN);

	strncpy(cardInfo->ownerHousehold, "����ʡ�人��", OWNER_HOUSEHOLD_MAX_LEN);
	strncpy(cardInfo->ownerPhone, "18782351982", OWNER_PHONE_MAX_LEN);
	strncpy(cardInfo->ownerJob, "���", JOB_MAX_LEN);
	strncpy(cardInfo->ownerNation, "003", OWNER_NATION_MAX_LEN);
	
	CFile ImageFile;
	char * szImageBin = NULL;
	if (0 == ImageFile.Open(XPS_TEST_PHOTO_FILE, CFile::modeRead))
	{
		free(cardInfo);
		MessageBox(_T("�򿪲����ļ�ʧ�ܣ�����"), _T("����"));
		return ET_ERROR;
	}

	unsigned int iSrcLen = (int)ImageFile.GetLength();

	szImageBin = (char*)malloc(iSrcLen);
	if (NULL == szImageBin)
	{
		free(cardInfo);
		ImageFile.Close();
		MessageBox(_T("�ڴ治�㣡����"), _T("����"));
		return ET_ERROR;
	}
	ImageFile.Read((void*)szImageBin,iSrcLen);
	ImageFile.Close();

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

	unsigned int dstLen = 0;
	if (NULL == base64decode(imgBase64, baseLen,
					cardInfo->ownerPhoto, OWNER_PHOTO_MAX_LEN, &dstLen))
	{
		free(cardInfo);
		free(imgBase64);
		MessageBox(_T("base64 ����ʧ�ܣ�����"), _T("����"));
		return ET_ERROR;
	}
	cardInfo->ownerPhotoLen = dstLen;
	free(imgBase64);

	return OK;
}


void CprintToolDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	int ipaddr = 0;
	unsigned short port = 0;

	CString ipStr;
	CString portStr;

	CEdit *pEdit = (CEdit*)GetDlgItem(IDC_PORT);
	pEdit->GetWindowText(portStr);

	CIPAddressCtrl *pIpCtrl = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1);
	pIpCtrl->GetWindowText(ipStr);

	ipaddr = inet_addr(ipStr.GetBuffer(0));
	port = _tcstoul(portStr, NULL, 10);

	CARD_INFO_T *cardInfo = (CARD_INFO_T*)malloc(sizeof(CARD_INFO_T));
	if (NULL == cardInfo)
	{
		MessageBox(_T("�ڴ治��������"), _T("����"));
		return;
	}

	memset(cardInfo, 0, sizeof(cardInfo));
	if(OK != this->getTestCardInfo(cardInfo))
	{
		return;
	}

	int len = 0;
	char *buf = msgTaskDataBuild(cardInfo, &len);
	if(NULL == buf)
	{
		free(cardInfo);
		MessageBox(_T("��������ʧ�ܣ�"), _T("����"));
		return;
	}
	free(cardInfo);

	MSG_HDR_T msgHdr;
	msgHdr.type = 1;
	msgHdr.len = len;

	char err_buf[64] = {0};
	CString strTmp;
	int sock_fd;
	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
	{
		free(buf);
		strTmp.Format("socket error, %s!", str_error_s(err_buf, sizeof(err_buf), errno));
		MessageBox(strTmp, _T("����"));
		return;
	}

	int iTimeOut = 3000;
	setsockopt(sock_fd,SOL_SOCKET,SO_RCVTIMEO,(char*)&iTimeOut,sizeof(iTimeOut));
	setsockopt(sock_fd,SOL_SOCKET,SO_SNDTIMEO,(char*)&iTimeOut,sizeof(iTimeOut));
	
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family= AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = ipaddr;
	if(connect(sock_fd,(struct sockaddr *)&server,sizeof(server))==-1)
	{
		free(buf);
		strTmp.Format("connect error, %s!", str_error_s(err_buf, sizeof(err_buf), errno));
		MessageBox(strTmp, _T("����"));
		closesocket(sock_fd);
		return;
	}

	etp_sock_set_unblock(sock_fd);

	bool isExpire = FALSE;
	int sendLen = etp_sock_safe_send(sock_fd, (char*)&msgHdr, sizeof(msgHdr), &isExpire);
	if (sendLen != sizeof(msgHdr))
	{
		free(buf);
		strTmp.Format("send hdr error, %s!", str_error_s(err_buf, sizeof(err_buf), errno));
		MessageBox(strTmp, _T("����"));
		closesocket(sock_fd);
		return;
	}

	sendLen = etp_sock_safe_send(sock_fd, buf, msgHdr.len, &isExpire);
	if (sendLen != msgHdr.len)
	{
		free(buf);
		strTmp.Format("send body error, %s!", str_error_s(err_buf, sizeof(err_buf), errno));
		MessageBox(strTmp, _T("����"));
		closesocket(sock_fd);
		return;
	}
	closesocket(sock_fd);

	free(buf);
	MessageBox("���Ͳ������ݳɹ�", _T("�ɹ�"));
	return;
}
