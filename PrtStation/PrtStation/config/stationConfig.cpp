#include "StdAfx.h"
#include <windows.h>

#include "prtCommon.h"
#include "PrtConfig.h"
#include "PrtGlobal.h"
#include "stationConfig.h"


#include <WinSock2.h>  
#pragma  comment (lib,"WS2_32")  


#define MAX_SERIAL_NUM_MASK  0xfffff
#define MAX_STATION_NUM_MASK  0xfff
#define STATSION_NUM_BITS (4*5)

static UINT32 g_cur_serial_num = 0;
static UINT32 g_station_num = 0;

static char g_stationCfgDir[MAX_PATH + 1] = {0};

int g_sales_cnt = 0;
char g_sales_phone[10][MAX_LINE_LEN];


int g_station_cnt = 0;
char g_stations[100][MAX_STATION_NAME_LEN + 1];
static char g_stationType[MAX_STATION_NAME_LEN + 1] = { 0 };

int g_dll_cnt = 3;
char g_writeIcDlls[3][64] =
{
	"广元",
	"绵商行",
	"通用"
};
char g_writeIcDllTypes[64] = {0};

int g_localip_cnt = 0;
char g_localip_desc[10][256];


char g_dllPrtDir[MAX_PATH + 1] = { 0 };
char g_dllWrIcDir[MAX_PATH + 1] = { 0 };


void get_local_ipaddress()
{
    char szHost[256] = {0};  
    ::gethostname(szHost,256);  
  
    hostent *pHost = ::gethostbyname(szHost);  
	if (pHost == NULL)
	{
		return;
	}

    in_addr addr;  
    int ipcnt = 0;
    for(int i= 0; ;i++)  
    {  
        char *p = pHost->h_addr_list[i];  
        if(p == NULL)  
            break;  

        memcpy(&addr.S_un.S_addr,p,pHost->h_length);  
        char *slzp = ::inet_ntoa(addr);  

        strncpy(g_localip_desc[i], slzp, 256); 
        g_localip_desc[i][255] = 0;

        ipcnt++;
        if (ipcnt >= 8)
        {
            break;
        }
    }

    g_localip_cnt = ipcnt;
}


UINT32 prtGetStationNum()
{
	return g_station_num;
}

UINT32 prtGetCurSerailNum()
{
	return g_cur_serial_num;
}

char* prtGetStationType()
{
	return g_stationType;
}

int prtSetStationType(char *stationType)
{
	strncpy(g_stationType, stationType, MAX_STATION_NAME_LEN);
	g_stationType[MAX_STATION_NAME_LEN] = 0;

	WritePrivateProfileString(_T("ServerType"), _T("Name"), g_stationType, _T(g_stationCfgDir));
	return OK;
}


int prtSetWrDllType(char *wrIcDllType)
{
	strncpy(g_writeIcDllTypes, wrIcDllType, sizeof(g_writeIcDllTypes)-1);
	g_writeIcDllTypes[sizeof(g_writeIcDllTypes) - 1] = 0;

	WritePrivateProfileString(_T("wrIcDllType"), _T("Name"), g_writeIcDllTypes, _T(g_stationCfgDir));
	return OK;
}

static int _getStations()
{
	g_station_cnt = 0;
	memset(&g_stations[0], 0, sizeof(g_stations));

	bool has_default = false;
	WIN32_FIND_DATA findData;
 
	CString strTemp;
	strTemp.Format(_T("%s/config/station/*.*"), g_localModDir);//查找指定目录下的直接的所有文件和目录

	HANDLE hFile = FindFirstFile(strTemp, &findData);
	while (hFile != INVALID_HANDLE_VALUE)
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//如果是目录
		{
			if(findData.cFileName[0] != _T('.'))//排除.和..文件夹
			{
				strncpy(g_stations[g_station_cnt], findData.cFileName, 255);//获得文件名
				g_station_cnt++;

				if (strncmp(findData.cFileName, DEFI_STATION_NAME, strlen(DEFI_STATION_NAME)) == 0)
				{
					has_default = true;
				}

				PRT_LOG_INFO("get station %s", findData.cFileName);
			}
		}

		if(!FindNextFile(hFile, &findData))
		{
			break;
		}
	}

	if (has_default == false)
	{
		CString str;
		str.Format(_T("station目录没有default配置"));
		MessageBox(NULL, str, "加载配置错误", MB_OK);
		return ET_ERROR;
	}

	return OK;
}

int prtStationInfoInit()
{
	/*遍历目录*/
	_getStations();

	SNPRINTF(g_stationCfgDir, MAX_PATH, "%s%s", g_localModDir, PRT_CFG_STATION_FILE);

	/*stationcode 初始化*/
	UINT32 stationNum = 0;
	UINT32 serialNum = 0;
	stationNum = ::GetPrivateProfileInt(_T("CardIdGenerate"), _T("StationCode"), 0, _T(g_stationCfgDir));
	serialNum = ::GetPrivateProfileInt(_T("CardIdGenerate"), _T("SerialCode"), 0, _T(g_stationCfgDir));
	
	/*服务器类型配置*/
	if(0 == ::GetPrivateProfileString(_T("ServerType"), _T("Name"), _T(""), 
							g_stationType, sizeof(g_stationType)-1, _T(g_stationCfgDir)))
	{
		CString str;
		str.Format(_T("读服务器类型配置失败"));
		MessageBox(NULL, str, "加载配置错误", MB_OK);
		return ET_ERROR;
	}

	if (0 == ::GetPrivateProfileString(_T("wrIcDllType"), _T("Name"), _T(""),
		g_writeIcDllTypes, sizeof(g_writeIcDllTypes) - 1, _T(g_stationCfgDir)))
	{
		CString str;
		str.Format(_T("读dll类型配置失败"));
		MessageBox(NULL, str, "加载配置错误", MB_OK);
		return ET_ERROR;
	}	

	if (stationNum == 0)
	{
		time_t t;
		srand((unsigned) time(&t));

		int randNum = rand();
		randNum &= MAX_STATION_NUM_MASK;

		g_station_num = randNum;
		g_cur_serial_num = 1;

		PRT_LOG_INFO("generate local station num 0x%x.", g_station_num);

		char strTemp[16] = {0};
		SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_station_num);
		WritePrivateProfileString(_T("CardIdGenerate"), _T("stationCode"), strTemp, _T(g_stationCfgDir));

		SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_cur_serial_num);
		WritePrivateProfileString(_T("CardIdGenerate"), _T("serialCode"), strTemp, _T(g_stationCfgDir));
	}
	else
	{
		g_station_num = stationNum;
		g_cur_serial_num = serialNum;

		PRT_LOG_INFO("get local station num 0x%x, cur serial num 0x%x.", g_station_num, g_cur_serial_num);
	}


	g_sales_cnt = 0;
    memset(g_sales_phone, 0, sizeof(g_sales_phone));

    char sales_path[MAX_PATH + 1] = {0};
    SNPRINTF(sales_path, MAX_PATH, "%sconfig\\售后支持.txt", g_localModDir);
    FILE *fp = fopen(sales_path, "r");
    if (NULL == fp)
    {
    	PRT_LOG_INFO("open sales config failed, %s", sales_path);
    }
    else
    {
		while (fgets(g_sales_phone[g_sales_cnt], MAX_LINE_LEN, fp))
		{
			int len = strlen(g_sales_phone[g_sales_cnt]);
			if (g_sales_phone[g_sales_cnt][len-1] == '\n')
			{
				g_sales_phone[g_sales_cnt][len-1] = 0;
			}

			g_sales_cnt++;
			if (g_sales_cnt >= 10)
			{
				break;
			}
		}
    }

	
	SNPRINTF(g_dllPrtDir, MAX_PATH, "%s%s", g_localModDir, XPS_PRINT_LIB_PATH);
	SNPRINTF(g_dllWrIcDir, MAX_PATH, "%slib\\SiChuanShebao(%s).dll", g_localModDir, g_writeIcDllTypes);

	return OK;
}

UINT32 prtGenCardIdCode()
{
	time_t timer;
	struct tm *now = NULL;
	time(&timer);
	now = localtime(&timer);
	int randNum = now->tm_yday;

	/*一年中的第几天与站号异或，再与流水号或*/
	UINT32 cardId =  (((g_station_num ^ randNum) << STATSION_NUM_BITS) | g_cur_serial_num);

	g_cur_serial_num++;
	g_cur_serial_num &= MAX_SERIAL_NUM_MASK;

	PRT_LOG_DEBUG("generate card id code 0x%x.", cardId);

	char strTemp[16] = {0};
	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_cur_serial_num);
	WritePrivateProfileString(_T("CardIdGenerate"), _T("serialCode"), strTemp, _T(g_stationCfgDir));

	return cardId;
}
