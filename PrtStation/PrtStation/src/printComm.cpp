
#include "afxdisp.h"
#include "prtCommon.h"
#include "PrtTaskHdl.h"
#include "tempConfig.h"
#include "printComm.h"
#include "prtGlobal.h"

static Gy_pCleanPrinter Gy_CleanPrinter;
static Gy_pfGetPrinterCardCount Gy_GetPrinterCardCount;
static Gy_pfGetPrinterRibbonPercentRemaining Gy_GetPrinterRibbonPercentRemaining;

static Gy_pfStartJob			Gy_StartJob;
static Gy_pfEncodeMag			Gy_EncodeMag;
static Gy_pfReadMag				Gy_ReadMag;
static Gy_pfEndJob				Gy_EndJob;
static Gy_pfParkSmartCard		Gy_ParkSmartCard;
static Gy_pfUnParkSmartCard		Gy_UnParkSmartCard;

static Gy_pPrePrint				Gy_PrePrint;
static Gy_pPostPrint			Gy_PostPrint;
static Gy_pPrintText			Gy_PrintText;
static Gy_pPrintImage			Gy_PrintImage;
static Gy_pPrintPhoto 			Gy_PrintPhoto;
static Gy_pGetLastErrOperateMsg Gy_GetLastErrOperateMsg;
static Gy_pGetPrinterSerialNum 	Gy_GetPrinterSerialNum;

static Gy_pSetPrinterOrientation	Gy_SetPrinterOrientation;


static Gy_pWCard_ICC_DATA Gy_WCardICC;
static Gy_pGet_Jami_YinZi Get_Jami_YinZi;

static HINSTANCE	g_hXPSPrinterDll = NULL;
static HINSTANCE	g_hXPSWriteIcDll = NULL;

static BOOL g_dllLoaded = XPS_LOAD_INIT;

int XpsErrConvert(int ret)
{
	int errCode = 0;

	switch(ret)
	{
	case 0x9041:
		errCode = ERR_PRINTER_NOT_READY;
		break;

	case 0x9097:
		errCode = ERR_PRINTER_CARD_READER;
		break;

	case 0x9163:
		errCode = ERR_PRINTER_PHOTO_PARSE_FAIL;
		break;

	case 0x9162:
		errCode = ERR_PRINTER_PHOTO_PRINT_FAIL;
		break;

	case 0x9071:
		errCode = ERR_PRINTER_LOAD_DLL_FAIL;
		break;

	case 0x905A:
		errCode = ERR_PRINTER_CANT_GET_INFO;
		break;

	case 0x9058:
		errCode = ERR_PRINTER_IN_ABNORMAL;
		break;

	case 0x905B:
		errCode = ERR_PRINTER_LAST_TASK_ABNORMAL;
		break;

	default:
		errCode = ERR_PRINTER_NOT_READY;
		break;
	}

	return errCode;
}

int XpsCleanPrinter(char *prtName)
{
	if (Gy_CleanPrinter == NULL)
	{
		return -1;
	}

	int ret = Gy_CleanPrinter(prtName);
	if(0x9000 != ret)
	{
		PRT_LOG_WARN("Gy_CleanPrinter failed, %d", ret);
		return -1;
	}

	return 0;
}

int XpsGetPrtedCardCnt(char *prtName)
{
	if (Gy_GetPrinterCardCount == NULL)
	{
		return 0;
	}

	int cardCnt = 0;
	int ret = Gy_GetPrinterCardCount(prtName, &cardCnt);
	if(0x9000 != ret)
	{
		PRT_LOG_WARN("Gy_GetPrinterCardCount failed, %d", ret);
		return 0;
	}

	return cardCnt;
}

int XpsGetSdPercent(char *prtName)
{
	if (Gy_GetPrinterRibbonPercentRemaining == NULL)
	{
		return 0;
	}

	int percent = 0;
	int ret = Gy_GetPrinterRibbonPercentRemaining(prtName, &percent);
	if(0x9000 != ret)
	{
		PRT_LOG_WARN("Gy_GetPrinterRibbonPercentRemaining failed, %d", ret);
		return 0;
	}
	
	return percent;
}

int	XpsGetJamiYinzi(int iReadType,int iPrinterPsamPos,char*Jamiyinzi,char*ErrData)
{
	if (XPS_LOAD_SUCC != g_dllLoaded)
	{
		XpsDllLoad();
		if (XPS_LOAD_SUCC != g_dllLoaded)
		{
			SNPRINTF(ErrData, RESULT_FAIL_STR_LEN - 1, "动态库未加载！");
			return ET_ERROR;
		}
	}
#if 1
	if(TRUE == Get_Jami_YinZi(iReadType, iPrinterPsamPos, Jamiyinzi, ErrData))
	{
		return OK;
	}
#else
	strncpy(Jamiyinzi, "a00000e50a", PRINTER_JIMIKA_INFO_LEN);
	return OK;
#endif

	return ET_ERROR;
}

int XpsWriteIc(PRT_TASK_T *taskInfo)
{
	if (XPS_LOAD_SUCC != g_dllLoaded)
	{
		SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN - 1, "动态库未加载！");
		return ET_ERROR;
	}
	g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_WRITE_IC);

	PRT_LOG_INFO("WriteIc: indata:%s.", taskInfo->outIcStr);
	PRT_LOG_INFO("WriteIc: format:%s.", taskInfo->outIcFmtStr);

	char resultFailStr[RESULT_FAIL_STR_LEN] = { 0 };
	if (TRUE != Gy_WCardICC(taskInfo->outIcStr,
		taskInfo->outIcFmtStr,
		taskInfo->resultOkStr, resultFailStr))
	{
		SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN - 1, "%s|%s",
			getErrCodeDesc(ERR_PRINTER_WR_IC_ERROR), resultFailStr);

		/*设置写卡失败*/
		taskInfo->outIcSt = ST_FAILED;
		g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_WRITE_IC_END);
		return ET_ERROR;
	}

	/*设置写卡成功*/
	taskInfo->outIcSt = ST_SUCCSS;
	g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_WRITE_IC_END);
	PRT_LOG_INFO("WriteIc success.");
	return OK;
}

int XpsCardGoOut(char *prtName, int readType, char *errData)
{
	int ret = Gy_StartJob(prtName, readType);
	if (0x9000 != ret)
	{
		int errCode = XpsErrConvert(ret);
		SNPRINTF(errData, 512, "%s", getErrCodeDesc(errCode));

		PRT_LOG_ERROR("Gy_StartJob 失败: %04x, %s", ret, errData);
		return ET_ERROR;
	}

	ret = Gy_ParkSmartCard(1);
	if (0x9000 != ret)
	{
		int errCode = XpsErrConvert(ret);
		SNPRINTF(errData, 512, "%s", getErrCodeDesc(errCode));

		PRT_LOG_ERROR("Gy_ParkSmartCard 失败: %04x, %s", ret, errData);
		Gy_EndJob(0);
		return ET_ERROR;
	}

	return OK;
}

void XpsCardGoBack()
{
	Gy_UnParkSmartCard();
	Gy_EndJob(0);
}

int XpsPrint(char *prtName, int readType, PRT_TASK_T *taskInfo, PRT_TEMPLATE_T *prtFormat)
{
	//PRT_LOG_DEBUG("%s, %s.", taskInfo->outIcStr, taskInfo->outIcFmtStr);

	if (XPS_LOAD_SUCC != g_dllLoaded)
	{
		SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "动态库未加载！");
		return ET_ERROR;
	}

	PRT_INFO_T *cardInfo = &taskInfo->outPrtInfo;

	int errCode = 0;

	/*just for debug test*/
	if (OK != g_taskHdl->photoToFile(cardInfo->ownerPhoto, cardInfo->ownerPhotoLen, g_testTempDir))
	{
		SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN - 1, "图片解码失败！");
		return ET_ERROR;
	}

	g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_GET_JOB);

//	try
//	{
		int endRet = 0;
		int ret = Gy_StartJob(prtName, readType);
		if(0x9000 != ret)
		{
			errCode = XpsErrConvert(ret);
			SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|获取job失败",
					getErrCodeDesc(errCode), ret);

			PRT_LOG_ERROR("Gy_StartJob 失败:%s	%04x", taskInfo->resultFailStr, ret);

			return ET_ERROR;
		}

		/*判断是否写IC卡，有时候测试时不写*/
		if (taskInfo->operFlag & FLAG_WRITE_IC)
		{
			int retryCnt = 0;
retryWriteIc:
			ret = Gy_ParkSmartCard(1);
			if(0x9000 != ret)
			{
				errCode = XpsErrConvert(ret);
				SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|卡槽进入失败",
						getErrCodeDesc(errCode), ret);

				PRT_LOG_ERROR("Gy_ParkSmartCard 失败:%s	%04x", taskInfo->resultFailStr,ret);

				endRet = Gy_EndJob(0);
				return ET_ERROR;
			}

			if (OK != XpsWriteIc(taskInfo))
			{
				if (strstr(prtName, "4250e") != NULL || strstr(prtName, "1250e") != NULL)
				{
					retryCnt++;
					if (retryCnt < 3)
					{
						PRT_LOG_WARN("Gy_ParkSmartCard retry %d for printer %s", retryCnt, prtName);
						Gy_ParkSmartCard(0);
						goto retryWriteIc;
					}
				}

				/*卡槽退出*/
				Gy_UnParkSmartCard();
				endRet = Gy_EndJob(0);
				return ET_ERROR;
			}
			
			ret = Gy_UnParkSmartCard();
			if(0x9000 != ret)
			{
				errCode = XpsErrConvert(ret);
				SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|卡槽退出失败",
						getErrCodeDesc(errCode), ret);

				endRet = Gy_EndJob(0);
				return ET_ERROR;
			}
		}

		/*判断是否打印，有时候测试时不打印*/
		if (taskInfo->operFlag & FLAG_PRINT)
		{
			g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_PRINT);

			ret = Gy_PrePrint();
			if(0x9000 != ret)
			{
				errCode = XpsErrConvert(ret);
				SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|PrePrint失败",
								getErrCodeDesc(errCode), ret);
			}

			if (prtFormat->ownerPhotoAttr.isPrint)
			{
	#if 0
				ret = Gy_PrintImage((&prtFormat->ownerPhotoAttr)->X,
								(&prtFormat->ownerPhotoAttr)->Y,
								(&prtFormat->ownerPhotoAttr)->Weight,
								(&prtFormat->ownerPhotoAttr)->Height,
								_T(g_testTempDir), NULL, 0);
	#else
				ret = Gy_PrintImage((&prtFormat->ownerPhotoAttr)->X,
								(&prtFormat->ownerPhotoAttr)->Y,
								(&prtFormat->ownerPhotoAttr)->Height,
								(&prtFormat->ownerPhotoAttr)->Weight,
								NULL, (byte*)&cardInfo->ownerPhoto[0], 0);
	#endif
				if(0x9000 != ret)
				{
					if (taskInfo->resultFailStr[0] == 0)
					{
						errCode = XpsErrConvert(ret);
						SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|PrintImage失败",
										getErrCodeDesc(errCode), ret);
						goto printEnd;
					}
				}

				PRT_LOG_INFO("print Photo: x:%d, y:%d.",
										(&prtFormat->ownerPhotoAttr)->X,
										(&prtFormat->ownerPhotoAttr)->Y);
			}

			if (prtFormat->ownerNameAttr.isPrint)
			{
				ret = Gy_PrintText((&prtFormat->ownerNameAttr)->X,
								(&prtFormat->ownerNameAttr)->Y,
								(&prtFormat->ownerNameAttr)->Font,
								(&prtFormat->ownerNameAttr)->Size,
								 (&prtFormat->ownerNameAttr)->Attribute,
								 (&prtFormat->ownerNameAttr)->Color,
								 cardInfo->ownerName,
								 (&prtFormat->ownerNameAttr)->TransBkColor,
								 (&prtFormat->ownerNameAttr)->Space);
				if(0x9000 != ret)
				{
					if (taskInfo->resultFailStr[0] == 0)
					{
						errCode = XpsErrConvert(ret);
						SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|PrintText姓名失败",
										getErrCodeDesc(errCode), ret);
					}
				}

				PRT_LOG_INFO("print name:%s, x:%d, y:%d.",
						cardInfo->ownerName,
						(&prtFormat->ownerNameAttr)->X,
						(&prtFormat->ownerNameAttr)->Y);
			}

			if (prtFormat->ownerIdentityNumAttr.isPrint)
			{
				ret = Gy_PrintText((&prtFormat->ownerIdentityNumAttr)->X,
								(&prtFormat->ownerIdentityNumAttr)->Y,
								(&prtFormat->ownerIdentityNumAttr)->Font,
								(&prtFormat->ownerIdentityNumAttr)->Size,
								 (&prtFormat->ownerIdentityNumAttr)->Attribute,
								 (&prtFormat->ownerIdentityNumAttr)->Color,
								 cardInfo->ownerIdentityNum,
								 (&prtFormat->ownerIdentityNumAttr)->TransBkColor,
								 (&prtFormat->ownerIdentityNumAttr)->Space);
				if(0x9000 != ret)
				{
					if (taskInfo->resultFailStr[0] == 0)
					{
						errCode = XpsErrConvert(ret);
						SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|PrintText保障号失败",
										getErrCodeDesc(errCode), ret);
					}
				}

				PRT_LOG_INFO("print IdentityNum:%s, x:%d, y:%d.",
								cardInfo->ownerIdentityNum,
								(&prtFormat->ownerIdentityNumAttr)->X,
								(&prtFormat->ownerIdentityNumAttr)->Y);
			}

			if (prtFormat->ownerCardNumAttr.isPrint)
			{
				ret = Gy_PrintText((&prtFormat->ownerCardNumAttr)->X,
								(&prtFormat->ownerCardNumAttr)->Y,
								(&prtFormat->ownerCardNumAttr)->Font,
								(&prtFormat->ownerCardNumAttr)->Size,
								 (&prtFormat->ownerCardNumAttr)->Attribute,
								 (&prtFormat->ownerCardNumAttr)->Color,
								 cardInfo->ownerCardNum,
								 (&prtFormat->ownerCardNumAttr)->TransBkColor,
								 (&prtFormat->ownerCardNumAttr)->Space);
				if(0x9000 != ret)
				{
					if (taskInfo->resultFailStr[0] == 0)
					{
						errCode = XpsErrConvert(ret);
						SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|PrintText卡号失败",
										getErrCodeDesc(errCode), ret);
					}
				}

				PRT_LOG_INFO("print CardNum:%s, x:%d, y:%d.",
									cardInfo->ownerCardNum,
									(&prtFormat->ownerCardNumAttr)->X,
									(&prtFormat->ownerCardNumAttr)->Y);
			}

			if (prtFormat->ownerSexAttr.isPrint)
			{
				ret = Gy_PrintText((&prtFormat->ownerSexAttr)->X,
								(&prtFormat->ownerSexAttr)->Y,
								(&prtFormat->ownerSexAttr)->Font,
								(&prtFormat->ownerSexAttr)->Size,
								 (&prtFormat->ownerSexAttr)->Attribute,
								 (&prtFormat->ownerSexAttr)->Color,
								 cardInfo->ownerSex,
								 (&prtFormat->ownerSexAttr)->TransBkColor,
								 (&prtFormat->ownerSexAttr)->Space);
				if(0x9000 != ret)
				{
					if (taskInfo->resultFailStr[0] == 0)
					{
						errCode = XpsErrConvert(ret);
						SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|PrintText性别失败",
										getErrCodeDesc(errCode), ret);
					}
				}

				PRT_LOG_INFO("print Sex:%s, x:%d, y:%d.",
										cardInfo->ownerSex,
										(&prtFormat->ownerSexAttr)->X,
										(&prtFormat->ownerSexAttr)->Y);
			}

			if (prtFormat->cardLaunchTimeAttr.isPrint)
			{
				ret = Gy_PrintText((&prtFormat->cardLaunchTimeAttr)->X,
								(&prtFormat->cardLaunchTimeAttr)->Y,
								(&prtFormat->cardLaunchTimeAttr)->Font,
								(&prtFormat->cardLaunchTimeAttr)->Size,
								 (&prtFormat->cardLaunchTimeAttr)->Attribute,
								 (&prtFormat->cardLaunchTimeAttr)->Color,
								 cardInfo->cardLaunchTime,
								 (&prtFormat->cardLaunchTimeAttr)->TransBkColor,
								 (&prtFormat->cardLaunchTimeAttr)->Space);
				if(0x9000 != ret)
				{
					if (taskInfo->resultFailStr[0] == 0)
					{
						errCode = XpsErrConvert(ret);
						SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|PrintText发卡日期失败",
										getErrCodeDesc(errCode), ret);
					}
				}

				PRT_LOG_INFO("print LaunchTime:%s, x:%d, y:%d.",
										cardInfo->cardLaunchTime,
										(&prtFormat->cardLaunchTimeAttr)->X,
										(&prtFormat->cardLaunchTimeAttr)->Y);
			}

			if (prtFormat->cardLaunchPlaceAttr.isPrint)
			{
				ret = Gy_PrintText((&prtFormat->cardLaunchPlaceAttr)->X,
								(&prtFormat->cardLaunchPlaceAttr)->Y,
								(&prtFormat->cardLaunchPlaceAttr)->Font,
								(&prtFormat->cardLaunchPlaceAttr)->Size,
								 (&prtFormat->cardLaunchPlaceAttr)->Attribute,
								 (&prtFormat->cardLaunchPlaceAttr)->Color,
								 cardInfo->cardLaunchPlace,
								 (&prtFormat->cardLaunchPlaceAttr)->TransBkColor,
								 (&prtFormat->cardLaunchPlaceAttr)->Space);
				if(0x9000 != ret)
				{
					if (taskInfo->resultFailStr[0] == 0)
					{
						errCode = XpsErrConvert(ret);
						SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|PrintText发卡日期失败",
										getErrCodeDesc(errCode), ret);
					}
				}

				PRT_LOG_INFO("print cardLaunchPlace:%s, x:%d, y:%d.",
										cardInfo->cardLaunchPlace,
										(&prtFormat->cardLaunchPlaceAttr)->X,
										(&prtFormat->cardLaunchPlaceAttr)->Y);
			}

printEnd:
			ret = Gy_PostPrint(0);
			if(0x9000 != ret)
			{
				errCode = XpsErrConvert(ret);
				SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|PostPrint失败",
								getErrCodeDesc(errCode), ret);

				endRet = Gy_EndJob(0);

				/*设置打印失败*/
				taskInfo->printSt = ST_FAILED;
				g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_PRINT_END);
				return ET_ERROR;
			}

			/*设置打印成功*/
			taskInfo->printSt = ST_SUCCSS;
			g_taskHdl->setTaskStatus(taskInfo, PRT_TASK_PRINT_END);
			PRT_LOG_INFO("print job posted.");
		}

		ret = Gy_EndJob(366);
		if(0x9000 != ret)
		{
			errCode = XpsErrConvert(ret);
			SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "%s|0x%x|注销job失败",
							getErrCodeDesc(errCode), ret);

			return ET_ERROR;
		}

		PRT_LOG_INFO("print job ended.");
//	}
//	catch(_com_error &e)
//	{
//		SNPRINTF(taskInfo->resultFailStr, RESULT_FAIL_STR_LEN-1, "打印机打印失败:%s", e.ErrorMessage());
//		return ET_ERROR;
//	}

	return OK;
}

int XpsDllLoad()
{
	//CreatDir(XPS_TEST_PATH);

	//AfxOleInit();

	/*必须加这个，否则startjob和退出的时候都会异常. 可能是在多线程的缘故吧*/
	CoInitialize(NULL);

	g_hXPSPrinterDll = LoadLibraryEx(_T(g_dllPrtDir), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if(g_hXPSPrinterDll)
	{
		Gy_CleanPrinter					= (Gy_pCleanPrinter)GetProcAddress(g_hXPSPrinterDll, "Gy_CleanPrinter");
		Gy_GetPrinterCardCount					= (Gy_pfGetPrinterCardCount)GetProcAddress(g_hXPSPrinterDll, "Gy_GetPrinterCardCount");
		Gy_GetPrinterRibbonPercentRemaining		= (Gy_pfGetPrinterRibbonPercentRemaining)GetProcAddress(g_hXPSPrinterDll, "Gy_GetPrinterRibbonPercentRemaining");

		Gy_StartJob		= (Gy_pfStartJob)GetProcAddress(g_hXPSPrinterDll, "Gy_StartJob");
		Gy_EncodeMag	= (Gy_pfEncodeMag)GetProcAddress(g_hXPSPrinterDll, "Gy_EncodeMag");
		Gy_ReadMag		= (Gy_pfReadMag)GetProcAddress(g_hXPSPrinterDll, "Gy_ReadMag");
		Gy_EndJob		= (Gy_pfEndJob)GetProcAddress(g_hXPSPrinterDll, "Gy_EndJob");

		Gy_ParkSmartCard	= (Gy_pfParkSmartCard)GetProcAddress(g_hXPSPrinterDll, "Gy_ParkSmartCard");	
		Gy_UnParkSmartCard	= (Gy_pfUnParkSmartCard)GetProcAddress(g_hXPSPrinterDll, "Gy_UnParkSmartCard");	
		Gy_PrePrint			= (Gy_pPrePrint)GetProcAddress(g_hXPSPrinterDll, "Gy_PrePrint");
		Gy_PostPrint		= (Gy_pPostPrint)GetProcAddress(g_hXPSPrinterDll, "Gy_PostPrint");
		Gy_PrintText		= (Gy_pPrintText)GetProcAddress(g_hXPSPrinterDll, 	"Gy_PrintText");
		Gy_PrintImage		= (Gy_pPrintImage)GetProcAddress(g_hXPSPrinterDll, "Gy_PrintImage");

		Gy_GetLastErrOperateMsg = (Gy_pGetLastErrOperateMsg)GetProcAddress(g_hXPSPrinterDll, "Gy_GetLastErrOperateMsg");
		Gy_GetPrinterSerialNum  = ( Gy_pGetPrinterSerialNum)GetProcAddress(g_hXPSPrinterDll, "Gy_GetPrinterSerialNum");
		Gy_SetPrinterOrientation 	= (Gy_pSetPrinterOrientation)GetProcAddress(g_hXPSPrinterDll,"Gy_SetPrinterOrientation");

	}
	else
	{
		g_dllLoaded = XPS_LOAD_FAIL;

		CString str;
		str.Format(_T("打印机动态库加载失败: %d"), GetLastError());
		MessageBox(NULL, str, "错误", MB_OK);
		return ET_ERROR;
	}


	g_hXPSWriteIcDll = LoadLibraryEx(CString(g_dllWrIcDir), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
	if(g_hXPSWriteIcDll)
	{
		Gy_WCardICC		= (Gy_pWCard_ICC_DATA)GetProcAddress(g_hXPSWriteIcDll, "WCard_ICC_DATA");
		Get_Jami_YinZi = (Gy_pGet_Jami_YinZi)GetProcAddress(g_hXPSWriteIcDll,"Get_Jami_YinZi");
	}
	else
	{
		g_dllLoaded = XPS_LOAD_FAIL;

		CString str;
		str.Format(_T("打印机写卡动态库加载失败: %d"), GetLastError());
		MessageBox(NULL, str, "错误", MB_OK);
		return ET_ERROR;
	}

	PRT_LOG_INFO("动态库加载完成.");

	g_dllLoaded = XPS_LOAD_SUCC;
	return OK;
}


void XpsDllUnLoad()
{
	g_dllLoaded = XPS_LOAD_INIT;

	PRT_LOG_INFO("动态库卸载完成.");

	if(g_hXPSPrinterDll)
	{
		FreeLibrary(g_hXPSPrinterDll);
	}
	g_hXPSPrinterDll = NULL;

	if(g_hXPSWriteIcDll)
	{
		FreeLibrary(g_hXPSWriteIcDll);
	}
	g_hXPSWriteIcDll = NULL;

	::CoUninitialize();  //释放 COM 公寓
}

int XpsWaitLoaded()
{
	while(g_dllLoaded == XPS_LOAD_INIT)
	{
		sleep_s(1);
	}

	if (g_dllLoaded == XPS_LOAD_SUCC)
	{
		return OK;
	}

	return ET_ERROR;
}

