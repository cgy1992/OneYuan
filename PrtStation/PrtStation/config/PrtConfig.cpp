#include "StdAfx.h"
#include <Winsock2.h>

#include "prtCommon.h"
#include "PrtTaskHdl.h"
#include "prtGlobal.h"
#include "PrtConfig.h"


char g_configIcDataFmtDir[MAX_PATH + 1] = {0};
char g_configAutoFillDir[MAX_PATH + 1] = {0};

char g_configPlaceCodeDir[MAX_PATH + 1] = {0};
char g_configBankCodeDir[MAX_PATH + 1] = {0};
char g_configBankWangdianCodeDir[MAX_PATH + 1] = {0};
char g_configMingzuCodeDir[MAX_PATH + 1] = {0};
char g_configZengjianCodeDir[MAX_PATH + 1] = {0};

/*是否使用代理*/
bool g_is_use_proxy = false;

int sysConfigReload(char *stationType)
{
	prtSetStationType(stationType);

	//intianlize XML Parser COM
	if (OK != registerCfgInit(stationType))
	{
		MessageBox(NULL, "Reload register config failed", "Error", MB_OK);
		return ET_ERROR;
	}

	if (OK != printerCfgInit(stationType))
	{
		MessageBox(NULL, "Reload printer config failed", "Error", MB_OK);
		return ET_ERROR;
	}

	if (OK != templateCfgInit(stationType))
	{
		MessageBox(NULL, "Reload template config failed", "Error", MB_OK);
		return ET_ERROR;
	}

	if (OK != icDataFmtValidChk())
	{
		MessageBox(NULL, "IC data format check failed!", "Error", MB_OK);
		return ET_ERROR;
	}

	PRT_LOG_INFO("reload config success.");
	return OK;	
}

int sysConfigInit(void)
{
	if(OK != prtStationInfoInit())
	{
		return ET_ERROR;
	}

	char *stationType = prtGetStationType();

	SNPRINTF(g_configIcDataFmtDir, MAX_PATH, "%s%s", g_localModDir, PRT_CFG_ICDATAFMT_FILE);	
	SNPRINTF(g_configAutoFillDir, MAX_PATH, "%s%s", g_localModDir, PRT_CFG_AUTOFILLFMT_FILE);	

	SNPRINTF(g_configPlaceCodeDir, MAX_PATH, "%s%s", g_localModDir, PRT_CFG_PLACE_FILE);
	SNPRINTF(g_configBankCodeDir, MAX_PATH, "%s%s", g_localModDir, PRT_CFG_BANK_FILE);
	SNPRINTF(g_configBankWangdianCodeDir, MAX_PATH, "%s%s", g_localModDir, PRT_CFG_BANKWD_FILE);
	SNPRINTF(g_configMingzuCodeDir, MAX_PATH, "%s%s", g_localModDir, PRT_CFG_MINGZU_FILE);
	SNPRINTF(g_configZengjianCodeDir, MAX_PATH, "%s%s", g_localModDir, PRT_CFG_XINGZHENG_FILE);
	
	//intianlize XML Parser COM
	if (OK != registerCfgInit(stationType))
	{
		MessageBox(NULL, "Load register config failed", "Error", MB_OK);
		return ET_ERROR;
	}

	if (OK != printerCfgInit(stationType))
	{
		MessageBox(NULL, "Load printer config failed", "Error", MB_OK);
		return ET_ERROR;
	}

	if (OK != templateCfgInit(stationType))
	{
		MessageBox(NULL, "Load template config failed", "Error", MB_OK);
		return ET_ERROR;
	}

	if (OK != icDataFmtInit())
	{
		MessageBox(NULL, "Load IC data format failed", "Error", MB_OK);
		return ET_ERROR;
	}

	if (OK != icDataFmtValidChk())
	{
		MessageBox(NULL, "IC data format check failed", "Error", MB_OK);
		return ET_ERROR;
	}

	if (OK != placeCodeInit())
	{
		return ET_ERROR;
	}

	if (OK != updateJiamiyinzi())
	{
		return ET_ERROR;
	}

	char proxyCfgFile[MAX_PATH + 1] = {0};
	SNPRINTF(proxyCfgFile, MAX_PATH, "%s代理工具\\GoProxy.ini", g_localModDir);
	FILE *fp = fopen(proxyCfgFile, "r");
    if (NULL == fp)
    {
    	PRT_LOG_INFO("proxy config file(%s) not exist", proxyCfgFile);
    }
    else
    {
    	fclose(fp);

		int result = ::GetPrivateProfileInt(_T("Status"), _T("IsProxyStart"), 0, _T(proxyCfgFile));
		if (result)
		{
			g_is_use_proxy = true;
		}
		else
		{
			g_is_use_proxy = false;
		}
    }

	PRT_LOG_INFO("load config success.");
	return OK;
}


void sysConfigUninit(void)
{
}

#include <TlHelp32.h> 
#include <io.h>
#include <tchar.h>

typedef struct tagWNDINFO
{
	DWORD dwProcessId;
	HWND hWnd;
} WNDINFO, *LPWNDINFO;

static BOOL CALLBACK YourEnumProc(HWND hWnd,LPARAM lParam)
{
	DWORD dwProcessId;
	GetWindowThreadProcessId(hWnd, &dwProcessId);
	LPWNDINFO pInfo = (LPWNDINFO)lParam;
	if(dwProcessId == pInfo->dwProcessId)
	{
		pInfo->hWnd = hWnd;
		return FALSE;
	}
	return TRUE;
}

static HWND GetProcessMainWnd(DWORD dwProcessId)
{
	WNDINFO wi;
	wi.dwProcessId = dwProcessId;
	wi.hWnd = NULL;
	EnumWindows(YourEnumProc,(LPARAM)&wi);
	return wi.hWnd;
}

HWND get_proc_hwnd(const char *proc_name)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		PRT_LOG_ERROR(_T("CreateToolhelp32Snapshot failed, %d"), GetLastError());
		return NULL;
	}

	DWORD proc_id = 0;
	PROCESSENTRY32 pe32;//声明进程入口对象 
	pe32.dwSize = sizeof(PROCESSENTRY32);//填充进程入口对象大小 
	Process32First(hSnapShot, &pe32);//遍历进程列表 
	do
	{
		if (!lstrcmp(pe32.szExeFile, proc_name))//查找指定进程名的PID 
		{
			proc_id = pe32.th32ProcessID;
			break;
		}
	} while (Process32Next(hSnapShot, &pe32));

	::CloseHandle(hSnapShot);
	if (0 == proc_id)
	{
		return NULL;
	}


	return GetProcessMainWnd(proc_id);
}