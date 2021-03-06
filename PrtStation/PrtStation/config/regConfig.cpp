#include "StdAfx.h"

#include "prtCommon.h"
#include "PrtConfig.h"
#include "PrtGlobal.h"
#include "regConfig.h"

static PRT_REG_CFG_T g_register_cfg;
static char g_configRegisterDir[MAX_PATH + 1];


PRT_REG_CFG_T* registerCfgGet(void)
{
	return &g_register_cfg;
}

int registerCfgSave(PRT_REG_CFG_T* regInfo)
{
	strncpy(regInfo->serverRegUrl, g_register_cfg.serverRegUrl, REG_URL_MAX_LEN);
	strncpy(regInfo->serverResultUrl, g_register_cfg.serverResultUrl, RESULT_URL_MAX_LEN);
	memcpy(&g_register_cfg, regInfo, sizeof(PRT_REG_CFG_T));

	char strTemp[16] = {0};

	/*server 配置*/
	WritePrivateProfileString(_T("Server"), _T("Ip"), g_register_cfg.serverAddrDesc, _T(g_configRegisterDir));
	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_register_cfg.serverPort);
	WritePrivateProfileString(_T("Server"), _T("Port"), strTemp, _T(g_configRegisterDir));
	WritePrivateProfileString(_T("Server"), _T("RegUrl"), g_register_cfg.serverRegUrl, _T(g_configRegisterDir));
	WritePrivateProfileString(_T("Server"), _T("ResultUrl"), g_register_cfg.serverResultUrl, _T(g_configRegisterDir));

	/*local 配置*/
	WritePrivateProfileString(_T("Client"), _T("Ip"), g_register_cfg.localAddrDesc, _T(g_configRegisterDir));
	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_register_cfg.localPort);
	WritePrivateProfileString(_T("Client"), _T("Port"), strTemp, _T(g_configRegisterDir));

	PRT_LOG_INFO("write register config file.");
	return OK;
}

int registerCfgRead(char *stationName, PRT_REG_CFG_T *registerCfg)
{
	CString str;
	char registerCfgFile[MAX_PATH + 1] = {0};
	SNPRINTF(registerCfgFile, MAX_PATH, "%sconfig\\station\\%s\\%s", 
								g_localModDir, stationName, PRT_CFG_REGISTER_FILE);
	
	/*server 配置*/
	if (0 == ::GetPrivateProfileString(_T("Server"), _T("Ip"), _T(""), registerCfg->serverAddrDesc,
							sizeof(registerCfg->serverAddrDesc), _T(registerCfgFile)))
	{
		str.Format(_T("读服务器IP失败, 路径:%s"), registerCfgFile);
		MessageBox(NULL, str, "加载配置错误", MB_OK);
		return ET_ERROR;
	}
	if (registerCfg->serverAddrDesc[0] != 0)
	{
		registerCfg->serverAddr = inet_addr(registerCfg->serverAddrDesc);
		if (INADDR_NONE == registerCfg->serverAddr)
		{
			str.Format("服务器IP转换失败: %s", registerCfg->serverAddrDesc);
			MessageBox(NULL, str, "加载配置错误", MB_OK);
			return ET_ERROR;
		}
		registerCfg->serverAddr = ntohl(registerCfg->serverAddr);
	}
	registerCfg->serverPort = ::GetPrivateProfileInt(_T("Server"), _T("Port"), DEF_SERVER_PORT, _T(registerCfgFile));
	
	if(0 == ::GetPrivateProfileString(_T("Server"), _T("RegUrl"), _T(""), registerCfg->serverRegUrl,
								REG_URL_MAX_LEN, _T(registerCfgFile)))
	{
		str.Format(_T("读服务器RegUrl失败, 路径:%s"), registerCfgFile);
		MessageBox(NULL, str, "加载配置错误", MB_OK);
		return ET_ERROR;
	}
	if(0 == ::GetPrivateProfileString(_T("Server"), _T("ResultUrl"), _T(""), registerCfg->serverResultUrl,
								RESULT_URL_MAX_LEN, _T(registerCfgFile)))
	{
		str.Format(_T("读服务器ResultUrl失败, 路径:%s"), registerCfgFile);
		MessageBox(NULL, str, "加载配置错误", MB_OK);
		return ET_ERROR;
	}

	/*local 配置*/
	if(0 == ::GetPrivateProfileString(_T("Client"), _T("Ip"), _T(""), registerCfg->localAddrDesc, 
							sizeof(registerCfg->localAddrDesc), _T(registerCfgFile)))
	{
		str.Format(_T("读本地IP失败, 路径:%s"), registerCfgFile);
		MessageBox(NULL, str, "加载配置错误", MB_OK);
		return ET_ERROR;
	}
	if (registerCfg->localAddrDesc[0] != 0)
	{
		registerCfg->localAddr = inet_addr(registerCfg->localAddrDesc);
		if (INADDR_NONE == registerCfg->localAddr)
		{
			str.Format(_T("读本地IP失败: %s"), registerCfg->localAddrDesc);
			MessageBox(NULL, str, "加载配置错误", MB_OK);
			return ET_ERROR;
		}
		registerCfg->localAddr = ntohl(registerCfg->localAddr);
	}
	registerCfg->localPort = ::GetPrivateProfileInt(_T("Client"), _T("Port"), DEF_LOCAL_PORT, _T(registerCfgFile));

	registerCfg->max_cnt_per_time = ::GetPrivateProfileInt(_T("Client"), _T("CntPerTime"), 1, _T(registerCfgFile));

	return OK;
}

int registerCfgInit(char *stationName)
{
	memset(&g_register_cfg, 0, sizeof(g_register_cfg));

	SNPRINTF(g_configRegisterDir, MAX_PATH, "%sconfig\\station\\%s\\%s", 
								g_localModDir, stationName, PRT_CFG_REGISTER_FILE);

	return registerCfgRead(stationName, &g_register_cfg);
}
