#include "StdAfx.h"

#include "prtCommon.h"
#include "PrtConfig.h"
#include "PrtGlobal.h"
#include "printerConfig.h"
#include "tempConfig.h"
#include "printComm.h"

static PRT_PRINTER_CFG_T g_printer_cfg;
static char g_configPrinterDir[MAX_PATH + 1];

static char g_jiamiyinzi[PRINTER_JIMIKA_INFO_LEN + 1] = {0};

char* jiamiyinziGet()
{
	return g_jiamiyinzi;
}

PRT_PRINTER_CFG_T* printerCfgGet(void)
{
	return &g_printer_cfg;
}

void saveJiamiyinzi(char *jiamiyinzi)
{
	strncpy(g_jiamiyinzi, jiamiyinzi, PRINTER_JIMIKA_INFO_LEN);
}

int updateJiamiyinzi()
{
#if 1
	int ret = OK;
	char jamiyinzi[PRINTER_JIMIKA_INFO_LEN + 1] = {0};
	char errData[RESULT_FAIL_STR_LEN + 1] = {0};

	ret = XpsGetJamiYinzi(g_printer_cfg.cardReaderType, g_printer_cfg.jiamiSamSlot, jamiyinzi, errData);
	if (OK != ret)
	{
		//MessageBox(NULL, _T(errData), _T("获取加密因子失败"), MB_OK);
		PRT_LOG_WARN("启动时获取加密因子失败!");
	}
	else
	{
		strncpy(g_jiamiyinzi, jamiyinzi, PRINTER_JIMIKA_INFO_LEN);
	}
#else
	SNPRINTF(g_printer_cfg.jimikaInfo, PRINTER_JIMIKA_INFO_LEN + 1, "a00000e50a");
#endif

	return OK;
}

int printerCfgSave(PRT_PRINTER_CFG_T* printParam)
{
	memcpy(&g_printer_cfg, printParam, sizeof(PRT_PRINTER_CFG_T));

	char strTemp[64] = {0};

	/*打印机配置*/
	WritePrivateProfileString(_T("Printer"), _T("Name"), g_printer_cfg.printer, _T(g_configPrinterDir));

	/*jiamiji 配置*/
	WritePrivateProfileString(_T("Jiamiji"), _T("Ip"), g_printer_cfg.jimijiAddrDesc, _T(g_configPrinterDir));
	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.jimijiPort);
	WritePrivateProfileString(_T("Jiamiji"), _T("Port"), strTemp, _T(g_configPrinterDir));
	WritePrivateProfileString(_T("Jiamiji"), _T("Username"), g_printer_cfg.jimijiUserName, _T(g_configPrinterDir));
	WritePrivateProfileString(_T("Jiamiji"), _T("Password"), g_printer_cfg.jimijiPasswd, _T(g_configPrinterDir));

	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.shebaoSamSlot);
	WritePrivateProfileString(_T("ShebaoSam"), _T("Slot"), strTemp, _T(g_configPrinterDir));
	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.shebaoSamType);
	WritePrivateProfileString(_T("ShebaoSam"), _T("Type"), strTemp, _T(g_configPrinterDir));

	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.jiamiSamSlot);
	WritePrivateProfileString(_T("JiamiSam"), _T("Slot"), strTemp, _T(g_configPrinterDir));
	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.jiamiSamType);
	WritePrivateProfileString(_T("JiamiSam"), _T("Type"), strTemp, _T(g_configPrinterDir));

	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.isUsePin);
	WritePrivateProfileString(_T("Pin"), _T("Check"), strTemp, _T(g_configPrinterDir));
	WritePrivateProfileString(_T("Pin"), _T("PinCode"), g_printer_cfg.pinCode, _T(g_configPrinterDir));

	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.userkaType);
	WritePrivateProfileString(_T("UserCard"), _T("Type"), strTemp, _T(g_configPrinterDir));

	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.cardReaderType);
	WritePrivateProfileString(_T("CardReader"), _T("Type"), strTemp, _T(g_configPrinterDir));
//	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.isNotWrIc);
//	WritePrivateProfileString(_T("CardReader"), _T("Debug"), strTemp, _T(g_configPrinterDir));

	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.isWrMatchBank);
	WritePrivateProfileString(_T("WriteIc"), _T("MatchBankCode"), strTemp, _T(g_configPrinterDir));
	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.isWrMatchAtr);
	WritePrivateProfileString(_T("WriteIc"), _T("MatchAtr"), strTemp, _T(g_configPrinterDir));
	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.isWrForceMiyao);
	WritePrivateProfileString(_T("WriteIc"), _T("ForceMiyao"), strTemp, _T(g_configPrinterDir));

	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.printDoType);
	WritePrivateProfileString(_T("DoPrtType"), _T("DoType"), strTemp, _T(g_configPrinterDir));

	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.tishiThrd);
	WritePrivateProfileString(_T("THRESHOLD"), _T("WarningCnt"), strTemp, _T(g_configPrinterDir));

	SNPRINTF(strTemp, sizeof(strTemp) - 1, "%d", g_printer_cfg.stopThrd);
	WritePrivateProfileString(_T("THRESHOLD"), _T("StopCnt"), strTemp, _T(g_configPrinterDir));
	return OK;
}

int printerCfgRead(char *stationName, PRT_PRINTER_CFG_T *printerCfg)
{
	CString strTemp;
	char printerCfgFile[MAX_PATH + 1] = {0};
	SNPRINTF(printerCfgFile, MAX_PATH, "%sconfig\\station\\%s\\%s", 
								g_localModDir, stationName, PRT_CFG_PRINTER_FILE);

	/*打印机配置*/
	if(0 == ::GetPrivateProfileString(_T("Printer"), _T("Name"), _T(""), 
							printerCfg->printer, PRINTER_MAX_LEN, _T(printerCfgFile)))
	{
		CString str;
		str.Format(_T("读打印机配置失败, 路径:%s"), printerCfgFile);
		MessageBox(NULL, str, "加载配置错误", MB_OK);
		return ET_ERROR;
	}

	/*jiamiji 配置*/
	if(0 == ::GetPrivateProfileString(_T("Jiamiji"), _T("Ip"), _T("0.0.0.0"), printerCfg->jimijiAddrDesc,
							sizeof(printerCfg->jimijiAddrDesc), _T(printerCfgFile)))
	{
		strTemp.Format(_T("读加密卡IP失败, 路径:%s"), printerCfgFile);
		MessageBox(NULL, strTemp, "加载配置错误", MB_OK);
		return ET_ERROR;
	}
	printerCfg->jimijiAddr = inet_addr(printerCfg->jimijiAddrDesc);
	if (INADDR_NONE == printerCfg->jimijiAddr)
	{
		strTemp.Format(_T("读加密卡IP失败: %s"), printerCfg->jimijiAddrDesc);
		MessageBox(NULL, strTemp, "加载配置错误", MB_OK);
		return ET_ERROR;
	}
	printerCfg->jimijiAddr = ntohl(printerCfg->jimijiAddr);
	printerCfg->jimijiPort = ::GetPrivateProfileInt(_T("Jiamiji"), _T("Port"), DEF_JIAMIJI_PORT, _T(printerCfgFile));

	if(0 == ::GetPrivateProfileString(_T("Jiamiji"), _T("Username"), _T(""), printerCfg->jimijiUserName,
						JIMIJI_USER_MAX_LEN, _T(printerCfgFile)))
	{
		strTemp.Format(_T("读加密机用户名失败, 路径:%s"), printerCfgFile);
		MessageBox(NULL, strTemp, "加载配置错误", MB_OK);
		return ET_ERROR;
	}
	if(0 == ::GetPrivateProfileString(_T("Jiamiji"), _T("Password"), _T(""), printerCfg->jimijiPasswd,
						JIMIJI_PASSWD_MAX_LEN, _T(printerCfgFile)))
	{
		strTemp.Format(_T("读加密机密码失败, 路径:%s"), printerCfgFile);
		MessageBox(NULL, strTemp, "加载配置错误", MB_OK);
		return ET_ERROR;
	}


	unsigned int retInt = 0;

	retInt = ::GetPrivateProfileInt(_T("Pin"), _T("Check"), 0, _T(printerCfgFile));
	printerCfg->isUsePin = retInt == 0 ? FALSE : TRUE;

	if(0 == ::GetPrivateProfileString(_T("Pin"), _T("PinCode"), _T(""), printerCfg->pinCode,
								sizeof(printerCfg->pinCode), _T(printerCfgFile)))
	{
		strTemp.Format(_T("读PIN配置失败, 路径:%s"), printerCfgFile);
		MessageBox(NULL, strTemp, "加载配置错误", MB_OK);
		return ET_ERROR;
	}
	printerCfg->shebaoSamSlot = ::GetPrivateProfileInt(_T("ShebaoSam"), _T("Slot"), 2, _T(printerCfgFile));
	printerCfg->shebaoSamType = ::GetPrivateProfileInt(_T("ShebaoSam"), _T("Type"), 1, _T(printerCfgFile));

	printerCfg->jiamiSamSlot = ::GetPrivateProfileInt(_T("JiamiSam"), _T("Slot"), 1, _T(printerCfgFile));
	printerCfg->jiamiSamType = ::GetPrivateProfileInt(_T("JiamiSam"), _T("Type"), PRT_JIAMI_TYPE_VER2, _T(printerCfgFile));

	printerCfg->userkaType = ::GetPrivateProfileInt(_T("UserCard"), _T("Type"),
					USERKA_TYPE_DATANG, _T(printerCfgFile));

	printerCfg->cardReaderType = ::GetPrivateProfileInt(_T("CardReader"), _T("Type"), 1, _T(printerCfgFile));
	retInt = ::GetPrivateProfileInt(_T("CardReader"), _T("Debug"), 0, _T(printerCfgFile));
	printerCfg->isNotWrIc = retInt == 0 ? FALSE : TRUE;

	retInt = ::GetPrivateProfileInt(_T("WriteIc"), _T("MatchBankCode"), 1, _T(printerCfgFile));
	printerCfg->isWrMatchBank =  retInt == 0 ? FALSE : TRUE;
	
	retInt = ::GetPrivateProfileInt(_T("WriteIc"), _T("MatchAtr"), 1, _T(printerCfgFile));
	printerCfg->isWrMatchAtr =  retInt == 0 ? FALSE : TRUE;
	
	retInt = ::GetPrivateProfileInt(_T("WriteIc"), _T("ForceMiyao"), 1, _T(printerCfgFile));
	printerCfg->isWrForceMiyao =  retInt == 0 ? FALSE : TRUE;

	printerCfg->printDoType = ::GetPrivateProfileInt(_T("DoPrtType"), _T("DoType"), DO_PRT_AND_WR_IC, _T(printerCfgFile));

	printerCfg->tishiThrd = ::GetPrivateProfileInt(_T("THRESHOLD"), _T("WarningCnt"), DO_PRT_AND_WR_IC, _T(printerCfgFile));
	printerCfg->stopThrd = ::GetPrivateProfileInt(_T("THRESHOLD"), _T("StopCnt"), DO_PRT_AND_WR_IC, _T(printerCfgFile));
	
	return OK;
}

int printerCfgInit(char *stationName)
{
	memset(&g_printer_cfg, 0, sizeof(PRT_PRINTER_CFG_T));

	SNPRINTF(g_configPrinterDir, MAX_PATH, "%sconfig\\station\\%s\\%s", 
								g_localModDir, stationName, PRT_CFG_PRINTER_FILE);

	return printerCfgRead(stationName, &g_printer_cfg);
}
