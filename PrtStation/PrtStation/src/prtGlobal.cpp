
#include "prtGlobal.h"
#include "Winspool.h"

#include "etpLib.h"
#include "PrtConfig.h"
#include "PrtService.h"
#include "PrtTaskHdl.h"
#include "jiamijiProxy.h"

char g_localModDir[MAX_PATH + 1] = {0};

char g_logDir[MAX_PATH + 1] = {0};

char g_testPhotoDir[MAX_PATH + 1] = {0};
char g_testTempDir[MAX_PATH + 1] = {0};
char g_testUserDir[MAX_PATH + 1] = { 0 };


PrtService *g_prtService = NULL;
PrtTaskHdl *g_taskHdl = NULL;
BOOL g_isTestPrinting = FALSE;

CBrowserOper g_browserOper;
CExcelOper *g_excelOper = NULL;
HWND g_parentWnd;

static char g_netStatusDesc[NET_STATUS_DESC_LEN + 1] = {0};

const char* g_errCodeDesc[ERR_TYPE_MAX] =
{
		"���ݸ�ʽ����",

		"��ǰ��ӡ��æ",
		"��Ӵ�ӡ����ʧ��",

		"��ӡ���������쳣",
		"��ӡ������׼��״̬",

		"��ӡ��ͼƬ����ʧ��",
		"��ӡ��ͼƬ��ӡʧ��",
		"��ӡ�����ض�̬��ʧ��",
		"���ܻ�ȡ��ӡ����Ϣ",
		"��ӡ�����ڹ���״̬",
		"�ϴδ�ӡ����������˳�",

		"��ӡ��дIC��ʧ��",
		"��ӡ��������Ϣ����"
};

PRT_DEV_T* getWinPrinters(int *count)
{
	DWORD Flags = PRINTER_ENUM_FAVORITE | PRINTER_ENUM_LOCAL;
    DWORD cbBuf; 
    DWORD pcReturned ;
    CString str;

    DWORD Level = 2; 
    TCHAR Name[500]={0} ; 
    
    ::EnumPrinters(Flags,
                   Name, 
                   Level, 
                   NULL, 
                   0, 
                   &cbBuf, //��Ҫ�����ڴ� 
                   &pcReturned) ; 
    const LPPRINTER_INFO_2 pPrinterEnum = (LPPRINTER_INFO_2)LocalAlloc(LPTR, cbBuf + 4) ; 

    if (!pPrinterEnum) 
    { 
        str.Format(_T("error is %d"), ::GetLastError());
		MessageBox(NULL, str, "����", MB_OK);
		return NULL;
    } 

    if (!EnumPrinters( 
        Flags, 
        Name,
        Level, 
        (LPBYTE)pPrinterEnum, 
        cbBuf, 
        &cbBuf, 
        &pcReturned)
        ) 
    { 
        str.Format(_T("error is %d"), ::GetLastError());
		MessageBox(NULL, str, "����", MB_OK);
        return NULL;
    }

	PRT_DEV_T *prtDev = (PRT_DEV_T*)malloc(sizeof(PRT_DEV_T) * pcReturned);
	memset(prtDev, 0, sizeof(PRT_DEV_T) * pcReturned);

	for(unsigned int i=0;i<pcReturned;i++)
    {
		LPPRINTER_INFO_2 pInfo = &pPrinterEnum[i];

		strncpy(prtDev[i].printerName, pInfo->pPrinterName, sizeof(prtDev[i].printerName));
		strncpy(prtDev[i].printerDriver, pInfo->pDriverName, sizeof(prtDev[i].printerDriver));
    }
    LocalFree(pPrinterEnum);
	
	*count = pcReturned;
	return prtDev;
}

void prtSetTaskInfoStatus(PRT_TASK_T *taskInfo, int status)
{
	taskInfo->status = status;

	PRT_LOG_INFO("print task status of user %s set to %s",
			taskInfo->outPrtInfo.ownerName, g_taskStatusDesc[status]);

	PRT_TASK_LIST_ITEM_T *newItem = (PRT_TASK_LIST_ITEM_T*)malloc(sizeof(PRT_TASK_LIST_ITEM_T));
	if (NULL == newItem)
	{
		PRT_LOG_ERROR("malloc failed.");
		return;
	}
	memset(newItem, 0, sizeof(PRT_TASK_LIST_ITEM_T));

	if (PRT_TASK_INIT == taskInfo->status)
	{
		newItem->isNew = TRUE;
	}
	else
	{
		newItem->isNew = FALSE;
	}
	strncpy(newItem->timeBuf, taskInfo->timeBuf, sizeof(newItem->timeBuf) - 1);
	strncpy(newItem->name, taskInfo->outPrtInfo.ownerName, sizeof(newItem->name) - 1);
	strncpy(newItem->identity, taskInfo->outPrtInfo.ownerIdentityNum, sizeof(newItem->identity) - 1);

	int totallen = 0;
	int tmplen = 0;
	int sparelen = 0;

	tmplen = strlen(g_taskStatusDesc[status]);
	strncpy(newItem->desc, g_taskStatusDesc[status], tmplen);
	totallen += tmplen;

	if (taskInfo->resultOkStr[0] != 0)
	{
		newItem->desc[totallen] = '@';
		totallen += 1;
		sparelen = TASK_STATUS_DESC_LEN - totallen;

		tmplen = strlen(taskInfo->resultOkStr);
		if (tmplen > (sparelen - 1))
		{
			tmplen = sparelen - 1;
		}

		strncpy(&newItem->desc[totallen], taskInfo->resultOkStr, tmplen);
		totallen += tmplen;
	}
	else if (taskInfo->resultFailStr[0] != 0)
	{
		newItem->desc[totallen] = '@';
		totallen += 1;
		sparelen = TASK_STATUS_DESC_LEN - totallen;

		tmplen = strlen(taskInfo->resultFailStr);
		if (tmplen > (sparelen - 1))
		{
			tmplen = sparelen - 1;
		}

		strncpy(&newItem->desc[totallen], taskInfo->resultFailStr, tmplen);
		totallen += tmplen;
	}

	if (taskInfo->sendResultStr[0] != 0)
	{
		newItem->desc[totallen] = '@';
		totallen += 1;
		sparelen = TASK_STATUS_DESC_LEN - totallen;

		strncpy(&newItem->desc[totallen], taskInfo->sendResultStr, sparelen);
	}

	::SendMessage(g_parentWnd, WM_UPDATE_TASK_LIST, (WPARAM)newItem, NULL);
}

void prtSetNetStatus(int index, char *netStatus)
{
	strncpy(g_netStatusDesc, netStatus, NET_STATUS_DESC_LEN);

	::SendMessage(g_parentWnd, WM_UPDATE_STATUS_BAR, (WPARAM)index, (LPARAM)g_netStatusDesc);
}


int prtGlobalInit()
{
	int ret = OK;

	HMODULE module = GetModuleHandle(0);
	GetModuleFileName(module, g_localModDir, MAX_PATH);
	char *ch = _tcsrchr(g_localModDir, _T('\\')); //�������һ��\���ֵ�λ�ã�������\������ַ�������\��
	ch[1] = 0;//NULL  ͨ������������szFilePath = ��szFilePath�ضϣ��ض����һ��\������ַ���������\��

	SNPRINTF(g_logDir, MAX_PATH, "%s%s", g_localModDir, PRINT_LOG_DIR);

	SNPRINTF(g_testPhotoDir, MAX_PATH, "%s%s", g_localModDir, XPS_TEST_PHOTO_FILE);
	SNPRINTF(g_testTempDir, MAX_PATH, "%s%s", g_localModDir, XPS_TEMP_PHOTO_FILE); 
	SNPRINTF(g_testUserDir, MAX_PATH, "%s%s", g_localModDir, XPS_USER_PHOTO_FILE);

	loggger_init(g_logDir, "runlog", 1 * 1024, 6, TRUE);
	logger_set_level(L_DEBUG);
	PRT_LOG_INFO("cur dir %s.", g_localModDir);

	g_parentWnd = AfxGetMainWnd()->m_hWnd;

	if (OK != sysConfigInit())
	{
		loggger_exit();
		MessageBox(NULL, "���ó�ʼ��ʧ�ܣ�����ϵ����Ա��������ļ�!", "����", MB_OK);
		return ET_ERROR;
	}

	g_taskHdl = PrtTaskHdl::instance();

	g_prtService = PrtService::instance();
	ret = g_prtService->init();
	if (OK != ret)
	{
		loggger_exit();
		MessageBox(NULL, "�����ʼ��ʧ��,��ȷ�ϴ˿ͻ����Ƿ�������!", "����", MB_OK);
		return ET_ERROR;
	}

	ret = g_browserOper.init();
	if (1 != ret)
	{
		loggger_exit();
		return ET_ERROR;
	}

	if (g_is_use_proxy)
	{
		if (OK != jiamijiProxyInit())
		{
			loggger_exit();
			MessageBox(NULL, "�ڲ������ʼ��ʧ��!", "����", MB_OK);
			return ET_ERROR;
		}
	}

	get_local_ipaddress();
	return OK;
}

void prtGlobalFree()
{
	if (g_excelOper != NULL)
	{
		g_excelOper->free();
		delete g_excelOper;
		g_excelOper = NULL;
	}

	if (g_is_use_proxy)
	{
		jiamijiProxyFree();
	}
	
	g_prtService->cleanup();

	loggger_exit();
}